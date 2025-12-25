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

Game* g_theGame = nullptr;
Renderer* g_theRenderer = nullptr;
Window* g_theWindow=nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
DevConsole* g_theDevConsole = nullptr;
BitmapFont* g_font = nullptr;
JobSystem* g_jobSystem = nullptr;


void App::Startup()
{

	if (!DoFolderExists("Saves")) {
		CreateFolder("Saves");
	}

	Clock* sysClock = new Clock();
	UNUSED(sysClock);

	EventSystemConfig eventConfig;
	g_theEventSystem = new EventSystem(eventConfig);
	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_windowTitle = "SimpleMiner";
	windowConfig.m_input = g_theInput;
	windowConfig.m_fullScreen =false;
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
	
	g_jobSystem = new JobSystem();

	g_theGame = new Game();
	g_theGame->StartUp();

	g_font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");

	DevConsoleConfig devConfig;
	devConfig.m_font = g_font;
	devConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConfig);
	g_theDevConsole->StartUp();
	std::vector<std::string> controls;
	controls.push_back("Controls");
	controls.push_back("Mouse - Aim");
	controls.push_back("W / A - Move");
	controls.push_back("S / D - Strafe");
	controls.push_back("Q / E - Elevate");
	controls.push_back("Shift - Sprint");
	controls.push_back("~- Open Dev Console");
	controls.push_back("Escape - Exit Game");
	controls.push_back("Space - Start Game");
	controls.push_back("F2 - Toggle Debug");
	controls.push_back("F8 - Restart");

	for (std::string line : controls) {
		g_theDevConsole->AddLine(line,Rgba8(0,255,255));
	}
	
	SubscribeEventCallbackFunction("QUIT", App::HandleQuitRequested);
}


//-----------------------------------------------------------------------------------------------
void App::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_jobSystem;
	g_jobSystem = nullptr;
}


//-----------------------------------------------------------------------------------------------
void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	g_theGame->BeginFrame();
	g_theDevConsole->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
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
	g_theGame->Update((float)Clock::GetSystemClock().GetDeltaSeconds());

	//Set mouse
	if (!g_theWindow->HasFocus()
		||g_theDevConsole->GetMode()==DevConsoleMode::OPENFULL
		||g_theGame->m_attractMode) {
		g_theInput->SetCurcorMode(CursorMode::POINTER);
	}
	else {
		g_theInput->SetCurcorMode(CursorMode::FPS);
	}

}


void App::Render()
{
	g_theGame->Render();	
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudio->EndFrame();
	g_theGame->EndFrame();
	g_theDevConsole->EndFrame();
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void App::RunFrame()
{
	BeginFrame();	
	Update();		
	Render();		
	EndFrame();		
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
    if (g_theInput->WasKeyJustReleased(27)) {
		if (g_theGame->m_attractMode) {
			m_isQuitting = true;
		}
		else {
			g_theGame->EnterAttractMode();
		}

	}
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		
	}

}
void App::RunMainloop() {
	// Program main loop; keep running frames until it's time to quit
	while (!IsQuitting())			
	{


		RunFrame(); 

	}
}