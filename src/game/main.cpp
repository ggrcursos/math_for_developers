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

 
// We'll create a single character named "box"
 

// This method gets called when the user presses a key
bool CGame::KeyPress(int c)
{
	 
}

// This method gets called when the player releases a key.
void CGame::KeyRelease(int c)
{
	 
}
 
void Update(float dt)
{
 
}

void Draw(CRenderer* pRenderer)
{
	 

	// This rendering context is a tool for rendering things to the screen.
	// All of our drawing commands are part of it.
	CRenderingContext r(pRenderer);

	// Clear the depth buffer and set a background color.
	r.ClearDepth();
	r.ClearColor(Color(210, 230, 255));

	 

	pRenderer->FinishRendering(&r);

	// Call this last. Your rendered stuff won't appear on the screen until you call this.
	Application()->SwapBuffers();
}

void GameLoop(CRenderer* pRenderer)
{
 

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
	game.OpenWindow(640, 480, false, false);

	// Run the game loop!
	GameLoop(game.GetRenderer());

	return 0;
}