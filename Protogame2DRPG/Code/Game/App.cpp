#include "App.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/FileUtils.hpp"
#include"Engine/Core/EventSystem.hpp"
#include "engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
Game* g_game = nullptr;
Renderer* g_theRenderer = nullptr;
Window* g_theWindow=nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
DevConsole* g_theDevConsole = nullptr;
Clock* sysClock = nullptr;
BitmapFont* g_squirrelFont = nullptr;
//RandomNumberGenerator* g_RNG = nullptr;
// #SD1ToDo: This will eventually go away once we add a Window engine class later on.
// 
//constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Startup()
//
void App::Startup()
{

	sysClock = new Clock();

	//g_RNG = new RandomNumberGenerator();

	EventSystemConfig eventConfig;
	g_theEventSystem = new EventSystem(eventConfig);
	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_windowTitle = "Protogame2D";
	windowConfig.m_input = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	
	AudioSystemConfig audioConfig;
	g_theAudio = new AudioSystem(audioConfig);

	g_theInput->StartUp();
	g_theWindow->StartUp();
	g_theRenderer->Startup();
	g_theAudio->Startup();
	


	g_game = new Game();
	g_game->StartUp();

	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	
	DevConsoleConfig devConfig;
	devConfig.m_font =g_squirrelFont;
	devConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConfig);
	g_theDevConsole->StartUp();
	
	
	SubscribeEventCallbackFunction("Quit", App::HandleQuitRequested);

	ImGui_ImplWin32_Init(g_theWindow->m_windowHandle);
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Shutdown()
//
void App::Shutdown()
{
	delete g_game;
	g_game = nullptr;
	g_theWindow->ShutDown();
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theWindow;
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::BeginFrame()
//
void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	g_game->BeginFrame();
	g_theDevConsole->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Update()
//
void App::Update()
{
	
	g_theInput->Update();
	HandleKeysPressed();
	if (!App::m_isPaused) {
		if (m_isSlowMo) {
			Clock::GetSystemClock().SetTimeScale(0.1f);
		}
	}
	else {
		Clock::GetSystemClock().Pause();
	}
	Clock::TickSystemClock();
	g_theWindow->Update();
	g_game->Update((float)Clock::GetSystemClock().GetDeltaSeconds());

	

}



//-----------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Render()
// #SD1ToDo: Move *ALL* OpenGL code to RenderContext.cpp (only).
//
// Ultimately this function (App::Render) will only call methods on Renderer (like Renderer::DrawVertexArray)
//	to draw things, never calling OpenGL (nor DirectX) functions directly.
//
void App::Render()
{
	//clear screen
	Rgba8 darkOrange =Rgba8(255,0,255);
	g_theRenderer->ClearScreen(darkOrange);

	//render ship
	//RenderShip();
	g_game->Render();
	//-----------------------------------
	//render dev console
	
}



//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::EndFrame()
//
void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudio->EndFrame();
	g_game->EndFrame();
	g_theDevConsole->EndFrame();
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::RunFrame()
//
void App::RunFrame()
{
	BeginFrame();	// #SD1ToDo: ...becomes just BeginFrame();	once this function becomes App::RunFrame()
	Update();		// #SD1ToDo: ...becomes just Update();		once this function becomes App::RunFrame()
	Render();		// #SD1ToDo: ...becomes just Render();		once this function becomes App::RunFrame()
	EndFrame();		// #SD1ToDo: ...becomes just EndFrame();	once this function becomes App::RunFrame()	
}

bool App::HandleKeyPressed(unsigned char keyCode) {
	// #SD1ToDo: Tell the App (or InputSystem later) about this key-pressed event...
	g_theInput->HandleKeyJustPressed(keyCode);
	return 0;
}
bool App::HandleKeyReleased(unsigned char keyCode) {
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		if (controller.IsConnected() && !controller.WasButtonReleased(XBOX_BUTTON_LSTICK)) {
			m_isSlowMo = false;
		}
		else if (keyCode == 'T') // #SD1ToDo: move this "check for ESC pressed" code to App
		{
			m_isSlowMo = false;
		}
	}
	
	g_theInput->HandleKeyJustReleased(keyCode);
	return 0;
}
bool App::HandleQuitRequested(EventArgs& args) {
	UNUSED(args);
	g_theApp->m_isQuitting = true;
	return 0;
}



void App::HandleKeysPressed() {
	XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_BACK)) {
		g_game->EnterAttractMode();
	}
	//esc
	else if (g_theInput->WasKeyJustReleased(27)) {
		if (g_game->m_attractMode) {
			m_isQuitting = true;
		}
		else {
			g_game->EnterAttractMode();
		}

	}
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_UP)) {
			m_isPaused = !m_isPaused;
		}
		else if (g_theInput->WasKeyJustReleased('P')) // #SD1ToDo: move this "check for ESC pressed" code to App
		{
			m_isPaused = !m_isPaused;
		}
		if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_LSTICK)) {
			m_isSlowMo = true;
		}
		else if (g_theInput->WasKeyJustPressed('T')) // #SD1ToDo: move this "check for ESC pressed" code to App
		{
			m_isSlowMo = true;
		}
		if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_DOWN)) {
			m_isPaused = false;
			m_waitingToPause = true;
		}
		else if (g_theInput->WasKeyJustPressed('O')) {
			m_isPaused = false;
			m_waitingToPause = true;
		}
	}
	
	//f8
	if (g_theInput->WasKeyJustPressed(0x77)) {
		delete g_game;
		g_game = new Game();
	}
}
void App::RunMainloop() {
	// Program main loop; keep running frames until it's time to quit
	while (!IsQuitting())			
	{


		RunFrame(); 

	}
}