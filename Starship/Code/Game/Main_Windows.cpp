#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "App.hpp"

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Eventually, we'll remove most OpenGL references out of Main_Win32.cpp
// Both of the following lines will eventually move to the top of Engine/Renderer/Renderer.cpp
//
/*
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
*/

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Later we will move this useful macro to a more central place, e.g. Engine/Core/EngineCommon.hpp
//
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: This will eventually go away once we add a Window engine class later on.
// 
//constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move each of these items to its proper place, once that place is established later on
// 
App* g_theApp = nullptr; // Created and owned by Main_Windows.cpp
//HWND g_hWnd = nullptr;								// ...becomes void* Window::m_windowHandle
//HDC g_displayDeviceContext = nullptr;				// ...becomes void* Window::m_displayContext
//HGLRC g_openGLRenderingContext = nullptr;			// ...becomes void* Renderer::m_apiRenderingContext
//char const* APP_NAME = "SD1 A4: Starship: Gold";	// ...becomes ??? (Change this per project!)







//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: Eventually, we will move this function to a more appropriate place later on... (Engine/Renderer/Window.cpp)
//





//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(applicationInstanceHandle);
	UNUSED(commandLineString);
	g_theApp = new App();
	g_theApp->Startup();


	g_theApp->RunMainloop();

	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	return 0;
}

