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

#include "game.h"

#include <cstring>

#include <algorithm>

//#include <GL3/gl3w.h>


class CCharacter2
{
public:
	Point position;
	Vector velocity;
	Vector gravity;
};
CCharacter2 box;
#if defined(__APPLE__)
#include <OpenGL/glu.h>
#include <unistd.h>
#else
#include <GL/glu.h>
#endif

 
#include <math/collision.h>
#include <math/frustum.h>
#include <maths.h>
#include <math/quaternion.h>
#include <math/physics.h>

#include <renderer/renderer.h>
#include <renderer/renderingcontext.h>
#include <renderer/shaders.h>

 

 
 
 
 
 
CGame::CGame(int argc, char** argv)
	: CApplication(argc, argv)
{
 

	
 
}
 
 

 // This method gets called when the user presses a key
bool CGame::KeyPress(int c)
{
	if (c == 'W')
	{
		box.velocity.z = 1;
		return true;
	}
	else if (c == 'A')
	{
		box.velocity.x = 1;
		return true;
	}
	else if (c == 'S')
	{
		box.velocity.z = -1;
		return true;
	}
	else if (c == 'D')
	{
		box.velocity.x = -1;
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
		box.velocity.z = 0;
	}
	else if (c == 'A')
	{
		box.velocity.x = 0;
	}
	else if (c == 'S')
	{
		box.velocity.z = 0;
	}
	else if (c == 'D')
	{
		box.velocity.x = 0;
	}
	else
		CApplication::KeyPress(c);
}
 
 

static Vector VelocityField(Vector position, float time)
{
	return Vector(-(position.x+1)*(position.x+3), 0, -5*position.z+5*time*time+2*time);
}

void CGame::Update(float dt){
		// Update position and velocity.
	box.position = box.position + box.velocity * dt;
	box.velocity = box.velocity + box.gravity * dt;

	// Make sure the player doesn't fall through the floor. The y dimension is up/down, and the floor is at 0.
	if (box.position.y < 0)
		box.position.y = 0;
}
 
void CGame::Draw()
{  

	// Cross-product http://www.youtube.com/watch?v=FT7MShdqK6w
	// Vector vecRight = vecUp.Cross(vecForward).Normalized();

	CRenderer* pRenderer = GetRenderer();

	pRenderer->SetCameraPosition(box.position + Vector(0, 4, -6));
	pRenderer->SetCameraDirection(Vector(box.position - pRenderer->GetCameraPosition()).Normalized()); // Look at the box
	pRenderer->SetCameraUp(Vector(0, 1, 0));
	pRenderer->SetCameraFOV(90);
	pRenderer->SetCameraNear(0.1f);
	pRenderer->SetCameraFar(1000);

	CRenderingContext r(pRenderer);
	 
	r.ClearDepth();
	r.ClearColor(Color(210, 230, 255));

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

 

// The Game Loop http://www.youtube.com/watch?v=c4b9lCfSDQM
void CGame::GameLoop()
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

		Draw();
	}
}


