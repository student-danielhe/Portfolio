#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "App.hpp"
#define UNUSED(x) (void)(x);

App* g_theApp = nullptr; // Created and owned by Main_Windows.cpp

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

