/*
Copyright (c) 2012, Lunar Workshop, Inc.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software must display the following acknowledgement:
   This product includes software developed by Lunar Workshop, Inc.
4. Neither the name of the Lunar Workshop nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY LUNAR WORKSHOP INC ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LUNAR WORKSHOP BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "common/common.h"
#include "renderer/application.h"
#include "renderer/renderer.h"
#include "renderer/renderingcontext.h"

// CGame is the "application" class. It creates the window and handles user input.
// It extends CApplication, which does all of the dirty work. All we have to do
// is override functions like KeyPress and KeyRelease, and CApplication will call
// those functions when the player presses a key on the keyboard.

float Approach(float flGoal, float flCurrent, float dt)
{
	float flDifference = flGoal - flCurrent;

	if(flDifference > dt)
		return flCurrent + dt;
	if(flDifference < -dt)
		return flCurrent - dt;

	return flGoal;
}
class CGame : public CApplication
{
	DECLARE_CLASS(CGame, CApplication);

public:
	CGame(int argc, char** argv)
		: CApplication(argc, argv)
	{
	}

public:
	virtual bool KeyPress(int c);
	virtual void KeyRelease(int c);
};

// This class holds information for a single character - eg the position and velocity of the player
class CCharacter
{
public:
	Point position;
	Vector velocity;
	Vector gravity;
	Vector velocityGoal;
};

// We'll create a single character named "box"
CCharacter box;

// This method gets called when the user presses a key
bool CGame::KeyPress(int c)
{
	if (c == 'W')
	{
		box.velocityGoal.z = 15;
		return true;
	}
	else if (c == 'A')
	{
		box.velocityGoal.x = 15;
		return true;
	}
	else if (c == 'S')
	{
		box.velocityGoal.z = -15;
		return true;
	}
	else if (c == 'D')
	{
		box.velocityGoal.x = -15;
		return true;
	}
	else if (c == ' ')
	{
		box.velocity.y = 2;
		return true;
	}
	else
		return CApplication::KeyPress(c);
}

// This method gets called when the player releases a key.
void CGame::KeyRelease(int c)
{
	if (c == 'W')
	{
		box.velocityGoal.z = 0;
	}
	else if (c == 'A')
	{
		box.velocityGoal.x = 0;
	}
	else if (c == 'S')
	{
		box.velocityGoal.z = 0;
	}
	else if (c == 'D')
	{
		box.velocityGoal.x = 0;
	}
	else
		CApplication::KeyPress(c);
}

// In this Update() function we need to update all of our characters. Move them around or whatever we want to do.
//https://www.youtube.com/watch?v=qJq7I2DLGzI&ab_channel=JorgeRodriguez
void Update(float dt)
{
	// Update position and velocity.
	box.velocity.x = Approach(box.velocityGoal.x, box.velocity.x, dt * 70);
	box.velocity.z = Approach(box.velocityGoal.z, box.velocity.z, dt * 70);
	box.position = box.position + box.velocity * dt;
	box.velocity = box.velocity + box.gravity * dt;

	// Make sure the player doesn't fall through the floor. The y dimension is up/down, and the floor is at 0.
	if (box.position.y < 0)
		box.position.y = 0;
}

void Draw(CRenderer* pRenderer)
{

//	glViewport(0, 0,200,600);
	// Tell the renderer how to set up the camera.
	pRenderer->SetCameraPosition(box.position + Vector(0, 4, -6));
	pRenderer->SetCameraDirection(Vector(box.position - pRenderer->GetCameraPosition()).Normalized()); // Look at the box
	pRenderer->SetCameraUp(Vector(0, 1, 0));
	pRenderer->SetCameraFOV(90);
	pRenderer->SetCameraNear(0.1f);
	pRenderer->SetCameraFar(1000);

	// This rendering context is a tool for rendering things to the screen.
	// All of our drawing commands are part of it.
	CRenderingContext r(pRenderer);

	// Clear the depth buffer and set a background color.
	r.ClearDepth();
	r.ClearColor(Color(210, 230, 255));

	// CRenderer::StartRendering() - This function sets up OpenGL with the
	// camera information that we passed it before.
	pRenderer->StartRendering(&r);

	// First tell OpenGL what "shader" or "program" to use.
	r.UseProgram("model");

	// Set the sunlight direction. The y component is -1 so the light is pointing down.
	r.SetUniform("vecSunlight", Vector(-1, -1, 0.5f).Normalized());

	// Render the player-box
	r.SetUniform("vecColor", Vector4D(0.8f, 0.4f, 0.2f, 1));
	r.RenderBox(box.position - Vector(0.5f, 0, 0.5f), box.position + Vector(0.5f, 2, 0.5f));

	// Render some other boxes, so that we can tell when we're moving.
	r.SetUniform("vecColor", Vector4D(0.3f, 0.9f, 0.5f, 1));

	Vector vecBoxPosition = Vector(6, 0, 4);
	r.RenderBox(vecBoxPosition - Vector(0.5f, 0, 0.5f), vecBoxPosition + Vector(0.5f, 1, 0.5f));

	vecBoxPosition = Vector(3, 0, -2);
	r.RenderBox(vecBoxPosition - Vector(0.5f, 0, 0.5f), vecBoxPosition + Vector(0.5f, 1, 0.5f));

	vecBoxPosition = Vector(-5, 0, 8);
	r.RenderBox(vecBoxPosition - Vector(0.5f, 0, 0.5f), vecBoxPosition + Vector(0.5f, 1, 0.5f));

	// Render the ground.
	r.SetUniform("vecColor", Vector4D(0.6f, 0.7f, 0.9f, 1));
	r.BeginRenderTriFan();
		r.Normal(Vector(0, 1, 0));
		r.Vertex(Vector(-30, 0, -30));
		r.Vertex(Vector(-30, 0, 30));
		r.Vertex(Vector(30, 0, 30));
		r.Vertex(Vector(30, 0, -30));
	r.EndRender();

	pRenderer->FinishRendering(&r);

	// Call this last. Your rendered stuff won't appear on the screen until you call this.
	Application()->SwapBuffers();
}

void GameLoop(CRenderer* pRenderer)
{
	// Initialize the box's position etc
	box.position = Point(0, 0, 0);
	box.velocity = Vector(0, 0, 0);
	box.gravity = Vector(0, -4, 0);

	float flPreviousTime = 0;
	float flCurrentTime = Application()->GetTime();

	while (true)
	{
		// flCurrentTime will be lying around from last frame. It's now the previous time.
		flPreviousTime = flCurrentTime;
		flCurrentTime = Application()->GetTime();

		float dt = flCurrentTime - flPreviousTime;

		// Keep dt from growing too large.
		if (dt > 0.15f)
			dt = 0.15f;

		Update(dt);

		Draw(pRenderer);
	 
	}
}

int main(int argc, char* argv[])
{
	// Create a game
	CGame game(argc, argv);

	// Open the game's window
	game.OpenWindow(800, 600, false, false);

	// Run the game loop!
	GameLoop(game.GetRenderer());

	return 0;
}