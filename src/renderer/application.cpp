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

#include "application.h"

#include <time.h>

#include <iostream>
#include <fstream>
#include <string.h>
#ifdef __APPLE__
#include <unistd.h>
#endif



#include <strutils.h>
#include <common_platform.h>

#include "renderer.h"
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width*FACTOR_VIEW_PORT, height*FACTOR_VIEW_PORT);
}
// #include "glad.h"
CApplication* CApplication::s_pApplication = NULL;

CApplication::CApplication(int argc, char** argv)
{
	s_pApplication = this;

	for (int i = 0; i < argc; i++)
		m_apszCommandLine.push_back(argv[i]);

	m_bIsOpen = false;
	m_bMultisampling = true;

	m_pRenderer = NULL;
	m_pConsole = NULL;

	//SetMouseCursorEnabled(true);
	m_flLastMousePress = -1;
}

bool CApplication::OpenWindow(size_t iWidth, size_t iHeight, bool bFullscreen, bool bResizeable)
{
    #ifdef __APPLE__
    // On macOS, glfwInit() can change the current directory.
    // See http://www.glfw.org/docs/latest/group__init.html
    char *cwd = getcwd(0, 0);
    int ret = glfwInit();
    chdir(cwd);
    free(cwd);
    #else
    int ret = glfwInit();
    #endif

    if (!ret)
    {
        printf("glfwInit failed\n");
        exit(1);
    }

    m_bFullscreen = bFullscreen;

    if (HasCommandLineSwitch("--fullscreen"))
        m_bFullscreen = true;

    if (HasCommandLineSwitch("--windowed"))
        m_bFullscreen = false;

    m_iWindowWidth = iWidth;
    m_iWindowHeight = iHeight;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    if (m_bMultisampling)
        glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (!(m_pWindow = glfwCreateWindow(iWidth, iHeight, "Math for Game Developers", NULL, NULL)))
    {
        glfwTerminate();
        return false;
    }

    glfwSetWindowTitle(m_pWindow, "Math for Game Developers");

    int iScreenWidth;
    int iScreenHeight;

    GetScreenSize(iScreenWidth, iScreenHeight);

    glfwMakeContextCurrent(m_pWindow);
    glfwSetFramebufferSizeCallback(m_pWindow, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } 

    if (!m_bFullscreen)
    {
        // The taskbar is at the bottom of the screen. Pretend the screen is smaller so the window doesn't clip down into it.
        // Also the window's title bar at the top takes up space.
        iScreenHeight -= 70;

        int iWindowX = (int)(iScreenWidth/2-m_iWindowWidth/2);
        int iWindowY = (int)(iScreenHeight/2-m_iWindowHeight/2);
        iWindowY -= 40;    // Move it a tad so that we can see it better in the videos.
        iWindowX -= 80;
        glfwSetWindowPos(m_pWindow, iWindowX, iWindowY);
    }

      // Configurar os callbacks do GLFW utilizando lambdas
    glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) { static_cast<CApplication*>(glfwGetWindowUserPointer(window))->WindowCloseCallback(); });
    glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow* window, int width, int height) { static_cast<CApplication*>(glfwGetWindowUserPointer(window))->WindowResizeCallback(width, height); });
    glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) { static_cast<CApplication*>(glfwGetWindowUserPointer(window))->KeyEventCallback(key, action); });
    glfwSetCharCallback(m_pWindow, [](GLFWwindow* window, unsigned int codepoint) { static_cast<CApplication*>(glfwGetWindowUserPointer(window))->CharEventCallback(codepoint,0); });
    glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xpos, double ypos) { static_cast<CApplication*>(glfwGetWindowUserPointer(window))->MouseMotionCallback(xpos, ypos); });
    glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods) { static_cast<CApplication*>(glfwGetWindowUserPointer(window))->MouseInputCallback(button, action); });

    glfwSwapInterval(1);
    glfwSetTime(0.0);

    SetMouseCursorEnabled(true);

    // GLenum err = gl3wInit();
    // if (0 != err)
    //     exit(0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0);

    m_bIsOpen = true;

    m_pRenderer = CreateRenderer();
    m_pRenderer->Initialize();

    return true;
}
CApplication::~CApplication()
{
	glfwTerminate();
}

void CApplication::SwapBuffers()
{
	glfwSwapBuffers(m_pWindow);
	glfwPollEvents();
}

float CApplication::GetTime()
{
	return (float)glfwGetTime();
}

bool CApplication::IsOpen()
{
	return m_bIsOpen;
}

void CApplication::Close()
{
	m_bIsOpen = false;
}

bool CApplication::HasFocus()
{
	return glfwGetWindowAttrib(m_pWindow, GLFW_FOCUSED) == 1;
}

void CApplication::Render()
{
}

int CApplication::WindowClose()
{
	return GL_TRUE;
}

void CApplication::WindowResize(int w, int h)
{
	m_iWindowWidth = w;
	m_iWindowHeight = h;

	if (m_pRenderer)
		m_pRenderer->WindowResize(w, h);

	Render();

	SwapBuffers();
}

void CApplication::MouseMotion(int x, int y)
{
}

bool CApplication::MouseInput(int iButton, tinker_mouse_state_t iState)
{
	return false;
}

tinker_keys_t MapKey(int c)
{
	switch (c)
	{
	case GLFW_KEY_ESCAPE:
		return TINKER_KEY_ESCAPE;

	case GLFW_KEY_F1:
		return TINKER_KEY_F1;

	case GLFW_KEY_F2:
		return TINKER_KEY_F2;

	case GLFW_KEY_F3:
		return TINKER_KEY_F3;

	case GLFW_KEY_F4:
		return TINKER_KEY_F4;

	case GLFW_KEY_F5:
		return TINKER_KEY_F5;

	case GLFW_KEY_F6:
		return TINKER_KEY_F6;

	case GLFW_KEY_F7:
		return TINKER_KEY_F7;

	case GLFW_KEY_F8:
		return TINKER_KEY_F8;

	case GLFW_KEY_F9:
		return TINKER_KEY_F9;

	case GLFW_KEY_F10:
		return TINKER_KEY_F10;

	case GLFW_KEY_F11:
		return TINKER_KEY_F11;

	case GLFW_KEY_F12:
		return TINKER_KEY_F12;

	case GLFW_KEY_UP:
		return TINKER_KEY_UP;

	case GLFW_KEY_DOWN:
		return TINKER_KEY_DOWN;

	case GLFW_KEY_LEFT:
		return TINKER_KEY_LEFT;

	case GLFW_KEY_RIGHT:
		return TINKER_KEY_RIGHT;

	case GLFW_KEY_LEFT_SHIFT:
		return TINKER_KEY_LSHIFT;

	case GLFW_KEY_RIGHT_SHIFT:
		return TINKER_KEY_RSHIFT;

	case GLFW_KEY_LEFT_CONTROL:
		return TINKER_KEY_LCTRL;

	case GLFW_KEY_RIGHT_CONTROL:
		return TINKER_KEY_RCTRL;

	case GLFW_KEY_LEFT_ALT:
		return TINKER_KEY_LALT;

	case GLFW_KEY_RIGHT_ALT:
		return TINKER_KEY_RALT;

	case GLFW_KEY_TAB:
		return TINKER_KEY_TAB;

	case GLFW_KEY_ENTER:
		return TINKER_KEY_ENTER;

	case GLFW_KEY_BACKSPACE:
		return TINKER_KEY_BACKSPACE;

	case GLFW_KEY_INSERT:
		return TINKER_KEY_INSERT;

	case GLFW_KEY_DELETE:
		return TINKER_KEY_DEL;

	case GLFW_KEY_PAGE_UP:
		return TINKER_KEY_PAGEUP;

	case GLFW_KEY_PAGE_DOWN:
		return TINKER_KEY_PAGEDOWN;

	case GLFW_KEY_HOME:
		return TINKER_KEY_HOME;

	case GLFW_KEY_END:
		return TINKER_KEY_END;

	case GLFW_KEY_KP_0:
		return TINKER_KEY_KP_0;

	case GLFW_KEY_KP_1:
		return TINKER_KEY_KP_1;

	case GLFW_KEY_KP_2:
		return TINKER_KEY_KP_2;

	case GLFW_KEY_KP_3:
		return TINKER_KEY_KP_3;

	case GLFW_KEY_KP_4:
		return TINKER_KEY_KP_4;

	case GLFW_KEY_KP_5:
		return TINKER_KEY_KP_5;

	case GLFW_KEY_KP_6:
		return TINKER_KEY_KP_6;

	case GLFW_KEY_KP_7:
		return TINKER_KEY_KP_7;

	case GLFW_KEY_KP_8:
		return TINKER_KEY_KP_8;

	case GLFW_KEY_KP_9:
		return TINKER_KEY_KP_9;

	case GLFW_KEY_KP_DIVIDE:
		return TINKER_KEY_KP_DIVIDE;

	case GLFW_KEY_KP_MULTIPLY:
		return TINKER_KEY_KP_MULTIPLY;

	case GLFW_KEY_KP_SUBTRACT:
		return TINKER_KEY_KP_SUBTRACT;

	case GLFW_KEY_KP_ADD:
		return TINKER_KEY_KP_ADD;

	case GLFW_KEY_KP_DECIMAL:
		return TINKER_KEY_KP_DECIMAL;

	case GLFW_KEY_KP_EQUAL:
		return TINKER_KEY_KP_EQUAL;

	case GLFW_KEY_KP_ENTER:
		return TINKER_KEY_KP_ENTER;
	}

	if (c < 256)
		return (tinker_keys_t)TranslateKeyToQwerty(c);

	return TINKER_KEY_UNKNOWN;
}


tinker_keys_t MapMouseKey(int c)
{
	switch (c)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		return TINKER_KEY_MOUSE_LEFT;

	case GLFW_MOUSE_BUTTON_RIGHT:
		return TINKER_KEY_MOUSE_RIGHT;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		return TINKER_KEY_MOUSE_MIDDLE;
	}

	return TINKER_KEY_UKNOWN;
}

tinker_keys_t MapJoystickKey(int c)
{
	switch (c)
	{
	case GLFW_JOYSTICK_1:
		return TINKER_KEY_JOYSTICK_1;

	case GLFW_JOYSTICK_2:
		return TINKER_KEY_JOYSTICK_2;

	case GLFW_JOYSTICK_3:
		return TINKER_KEY_JOYSTICK_3;

	case GLFW_JOYSTICK_4:
		return TINKER_KEY_JOYSTICK_4;

	case GLFW_JOYSTICK_5:
		return TINKER_KEY_JOYSTICK_5;

	case GLFW_JOYSTICK_6:
		return TINKER_KEY_JOYSTICK_6;

	case GLFW_JOYSTICK_7:
		return TINKER_KEY_JOYSTICK_7;

	case GLFW_JOYSTICK_8:
		return TINKER_KEY_JOYSTICK_8;

	case GLFW_JOYSTICK_9:
		return TINKER_KEY_JOYSTICK_9;

	case GLFW_JOYSTICK_10:
		return TINKER_KEY_JOYSTICK_10;
	}

	return TINKER_KEY_UKNOWN;
}

void CApplication::MouseInputCallback(int iButton, int iState)
{
	Get()->MouseInputCallback(MapMouseKey(iButton), (tinker_mouse_state_t)iState);
}

void CApplication::MouseInputCallback(int iButton, tinker_mouse_state_t iState)
{
	if (iState == 1)
	{
		if (m_flLastMousePress < 0 || GetTime() - m_flLastMousePress > 0.25f)
			MouseInput(iButton, iState);
		else
			MouseInput(iButton, TINKER_MOUSE_DOUBLECLICK);
		m_flLastMousePress = GetTime();
	}
	else
		MouseInput(iButton, iState);
}

void CApplication::KeyEvent(int c, int e)
{
	if (e == GLFW_PRESS)
		KeyPress(MapKey(c));
	else if(e == GLFW_RELEASE)
		KeyRelease(MapKey(c));
}

void CApplication::CharEvent(int c, int e)
{
	DoCharPress(c);
}

bool CApplication::KeyPress(int c)
{
	if (c == TINKER_KEY_ESCAPE )
		exit(0);

	return DoKeyPress(c);
}

void CApplication::KeyRelease(int c)
{
	DoKeyRelease(c);
}

//


bool CApplication::IsCtrlDown()
{
    return glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(m_pWindow, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
}

bool CApplication::IsAltDown()
{
    return glfwGetKey(m_pWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(m_pWindow, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
}

bool CApplication::IsShiftDown()
{
    return glfwGetKey(m_pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_pWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

bool CApplication::IsMouseLeftDown()
{
    return glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

bool CApplication::IsMouseRightDown()
{
    return glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

bool CApplication::IsMouseMiddleDown()
{
    return glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
}

void CApplication::GetMousePosition(double& x, double& y)
{
    glfwGetCursorPos(m_pWindow, &x, &y);
}

void CApplication::SetMouseCursorEnabled(bool bEnabled)
{
    if (bEnabled)
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_bMouseEnabled = bEnabled;
}


//

bool CApplication::IsMouseCursorEnabled()
{
	return m_bMouseEnabled;
}

inline class CRenderer* CApplication::CreateRenderer()
{
	return new CRenderer(m_iWindowWidth, m_iWindowHeight);
}

bool CApplication::HasCommandLineSwitch(const char* pszSwitch)
{
	for (size_t i = 0; i < m_apszCommandLine.size(); i++)
	{
		if (strcmp(m_apszCommandLine[i], pszSwitch) == 0)
			return true;
	}

	return false;
}

const char* CApplication::GetCommandLineSwitchValue(const char* pszSwitch)
{
	// -1 to prevent buffer overrun
	for (size_t i = 0; i < m_apszCommandLine.size()-1; i++)
	{
		if (strcmp(m_apszCommandLine[i], pszSwitch) == 0)
			return m_apszCommandLine[i+1];
	}

	return NULL;
}
