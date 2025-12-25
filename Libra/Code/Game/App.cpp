#include "App.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine//Core/NamedStrings.hpp"

//#include <windows.h>
Game* g_theGame = nullptr;
Renderer* g_theRenderer = nullptr;
Window* g_theWindow=nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
DevConsole* g_theDevConsole = nullptr;
// #SD1ToDo: This will eventually go away once we add a Window engine class later on.
// 
//constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Startup()
//
void App::Startup()
{
	XmlDocument gameConfig;
	XmlResult result=gameConfig.LoadFile("Data/GameConfig.xml");
	if (result== XmlResult::XML_SUCCESS) {
		XmlElement const* gameElement=gameConfig.RootElement();
		g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*gameElement);
		std::string str=g_gameConfigBlackboard.GetValue("GRID_SIZE","Error");
	}
	EventSystemConfig eventConfig;
	g_theEventSystem = new EventSystem(eventConfig);

	

	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_windowTitle = "Libra";
	windowConfig.m_input = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	AudioSystemConfig audioConfig;
	g_theAudio = new AudioSystem(audioConfig);
	g_theEventSystem->StartUp();
	
	g_theInput->StartUp();
	g_theWindow->StartUp();
	g_theRenderer->Startup();
	g_theAudio->Startup();
	
	g_theGame = new Game();
	g_theGame->StartUp();
	DevConsoleConfig devConfig;
	devConfig.m_font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	devConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConfig);
	g_theDevConsole->StartUp();

	//----------------------event system test
	//g_theEventSystem->SubscribeEventCallbackFunction("Test", DevConsole::Command_Test);
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Shutdown()
//
void App::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
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
	g_theGame->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Update()
//
void App::Update(float deltaSeconds)
{
	float musicSpd = 1;
	g_theInput->Update();
	HandleKeysPressed();
	if (!App::m_isPaused) {
		if (m_isSlowMo) {
			deltaSeconds*=0.1f;
			musicSpd = 0.1f;
		}
		if (m_isFastMo) {
			deltaSeconds *= 4.f;
			musicSpd = 4.f;
		}
	}
	else {
		deltaSeconds *= 0.f;
		musicSpd = 0.f;
	}
	g_theWindow->Update();
	g_theGame->Update(deltaSeconds);

	//handle O key
	if (m_waitingToPause) {
		m_isPaused = true;
		m_waitingToPause = false;
	}
	g_theAudio->SetSoundPlaybackSpeed(g_theGame->m_BGMPlayBack, musicSpd);

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
	Rgba8 darkOrange =Rgba8(0,0,0);
	g_theRenderer->ClearScreen(darkOrange);

	//render ship
	//RenderShip();
	g_theGame->Render();
	

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
	g_theGame->EndFrame();
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::RunFrame()
//
void App::RunFrame()
{
	m_now = static_cast<float>(GetCurrentTimeSeconds());
	float deltaSeconds = m_now - m_lastFrame;
	m_lastFrame = m_now;
	if (deltaSeconds == 0) {
		deltaSeconds = 16;
	}
	BeginFrame();	// #SD1ToDo: ...becomes just BeginFrame();	once this function becomes App::RunFrame()
	Update(deltaSeconds);		// #SD1ToDo: ...becomes just Update();		once this function becomes App::RunFrame()
	m_now = static_cast<float>(GetCurrentTimeSeconds());

	Render();		// #SD1ToDo: ...becomes just Render();		once this function becomes App::RunFrame()
	EndFrame();		// #SD1ToDo: ...becomes just EndFrame();	once this function becomes App::RunFrame()
	
}

bool App::HandleKeyPressed(unsigned char keyCode) {
	g_theInput->HandleKeyJustPressed(keyCode);
	return 0;
}
bool App::HandleKeyReleased(unsigned char keyCode) {
	g_theInput->HandleKeyJustReleased(keyCode);
	return 0;
}
bool App::HandleQuitRequested() {
	m_isQuitting = true;
	return 0;
}



void App::HandleKeysPressed() {
	XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_BACK)) {
		if (g_theGame->m_attractMode) {
			
			m_isQuitting = true;
			
		}
		else {
			g_theGame->EnterAttractMode();
		}
	}
	//esc
	else if (g_theInput->WasKeyJustReleased(27)) {
		if (g_theGame->m_attractMode) {
			
			m_isQuitting = true;
				
		}
		else {
			g_theGame->EnterAttractMode();
		}

	}
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
	else if (g_theInput->IsKeyJustDown('T')) // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		m_isSlowMo = true;
	}
	else {
		m_isSlowMo = false;
	}
	if (g_theInput->IsKeyJustDown('Y')) // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		m_isFastMo = true;
	}
	else {
		m_isFastMo = false;
	}
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_DOWN)) {
		m_isPaused = false;
		m_waitingToPause = true;
	}
	else if (g_theInput->WasKeyJustPressed('O')) {
		m_isPaused = false;
		m_waitingToPause = true;
	}
	//f8
	if (g_theInput->WasKeyJustPressed(0x77)) {
		delete g_theGame;
		g_theGame = new Game();
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_DEV_CONSOLE)) // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		g_theDevConsole->ToggleMode(DevConsoleMode::OPENFULL);

	}
	if (g_theInput->WasKeyJustReleased('F')) {
		//FireEvent("Test");
	}
}
void App::RunMainloop() {
	// Program main loop; keep running frames until it's time to quit
	while (!IsQuitting())			
	{


		RunFrame(); 

	}
}

