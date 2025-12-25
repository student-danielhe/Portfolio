#include "App.hpp"
#include "GameNearestPoint.hpp"
#include "GameRayCastVSDiscs.hpp"
#include "GameRayCastVSAABB2.hpp"
#include "GameRayCastVSLine.hpp"
#include "GamePachinko2D.hpp"
#include "Game2DCurve.hpp"
#include "Game3D.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Window/Window.hpp"
#include "Game.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"

Game* g_theGame = nullptr;
Renderer* g_theRenderer = nullptr;
Window* g_theWindow=nullptr;
InputSystem* g_theInput = nullptr;
DevConsole* g_theDevConsole = nullptr;

// #SD1ToDo: This will eventually go away once we add a Window engine class later on.
// 
//constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Startup()
//
void App::Startup()
{
	Clock* sysClock = new Clock();
	UNUSED(sysClock);

	EventSystemConfig eventConfig;
	g_theEventSystem = new EventSystem(eventConfig);
	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_windowTitle = "Math Visual Test";
	windowConfig.m_input = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	g_theInput->StartUp();
	g_theWindow->StartUp();
	g_theRenderer->Startup();
	
	g_theGame = new Game();
	g_theGame->StartUp();

	DevConsoleConfig devConfig;
	devConfig.m_font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	devConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConfig);
	g_theDevConsole->StartUp();
	g_theGame = new GameNearestPoint();
	g_theGame->StartUp();

	m_camera = new Camera();
	m_camera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	SubscribeEventCallbackFunction("QUIT", App::HandleQuitRequested);
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
	//g_theGame->BeginFrame();
	g_theDevConsole->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Update()
//
void App::Update()
{
	
	g_theInput->Update();
	HandleKeyCommand();
	Clock::TickSystemClock();
	g_theWindow->Update();
	g_theGame->Update((float)Clock::GetSystemClock().GetDeltaSeconds());

	//Set mouse
	if (!g_theWindow->HasFocus()
		|| g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL
		|| m_gamemode!=GAME_MODE_3D) {
		g_theInput->SetCurcorMode(CursorMode::POINTER);
	}
	else {
		g_theInput->SetCurcorMode(CursorMode::FPS);
	}
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

	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->BindTexture(nullptr);
	g_theGame->Render();
	RenderText();

}



//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::EndFrame()
//
void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	//g_theGame->EndFrame();
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
	g_theInput->HandleKeyJustPressed(keyCode);
	return 0;
}
bool App::HandleKeyReleased(unsigned char keyCode) {

	g_theInput->HandleKeyJustReleased(keyCode);
	return 0;
}
bool App::HandleQuitRequested(EventArgs& args) {
	UNUSED(args);
	g_theApp->m_isQuitting = true;
	return 0;
}



void App::HandleKeyCommand() {
	//F7
	if (g_theInput->WasKeyJustReleased(KEYCODE_F7)) {
		m_gamemode++;
		m_gamemode = m_gamemode % NUM_GAME_MODE;
		RestartGame();
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_F6)) {
		if (m_gamemode <= 0) {
			m_gamemode = NUM_GAME_MODE;
		}
		m_gamemode--;
		m_gamemode = m_gamemode % NUM_GAME_MODE;
		RestartGame();
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_F8)) {
		RestartGame();
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_ESCAPE)) {
		FireEvent("QUIT");
	}
	
}
void App::RunMainloop() {
	// Program main loop; keep running frames until it's time to quit
	while (!IsQuitting())			
	{


		RunFrame(); 

	}
}

void App::RestartGame() {
	delete g_theGame;
	g_theGame = nullptr;
	if (m_gamemode == GAME_MODE_NEAREST_POINT) {
		g_theGame = new GameNearestPoint();
		g_theGame->StartUp();
	}
	else if (m_gamemode == GAME_MODE_RAYCAST_VS_DISCS) {
		g_theGame = new GameRayCastVSDiscs();
		g_theGame->StartUp();
	}
	else if (m_gamemode == GAME_MODE_RAYCAST_VS_AABB2) {
		g_theGame = new GameRayCastVSAABB2();
		g_theGame->StartUp();
	}
	else if (m_gamemode == GAME_MODE_RAYCAST_VS_LINE) {
		g_theGame = new GameRayCastVSLine();
		g_theGame->StartUp();
	}
	else if (m_gamemode == GAME_MODE_3D) {
		g_theGame = new Game3D();
		g_theGame->StartUp();
	}
	else if (m_gamemode == GAME_MODE_2D_CURVE) {
		g_theGame = new Game2DCurve();
		g_theGame->StartUp();
	}
	else if (m_gamemode == GAME_MDOE_PACHINKO) {
		g_theGame = new GamePachinko2D();
		g_theGame->StartUp();
	}
}

void App::RenderText() {
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	
	g_theRenderer->BeginCamera(*m_camera);
	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	std::vector<Vertex_PCU> verts;
	std::string modeName;
	if (m_gamemode == GAME_MODE_NEAREST_POINT) {
		modeName = "Nearest Point";
	}
	else if (m_gamemode == GAME_MODE_RAYCAST_VS_DISCS) {
		modeName = "Ray Cast Disc";
	}
	else if (m_gamemode == GAME_MODE_RAYCAST_VS_AABB2) {
		modeName = "Ray Cast AABB2";
	}
	else if (m_gamemode == GAME_MODE_RAYCAST_VS_LINE) {
		modeName = "Ray Cast Line";
	}
	else if (m_gamemode == GAME_MODE_3D) {
		modeName = "3D Test Shapes";
	}
	else if(m_gamemode == GAME_MODE_2D_CURVE) {
		modeName = "2D Curve";
	}
	else if (m_gamemode == GAME_MDOE_PACHINKO) {
		modeName = "Pachinko 2D";
	}
	font->AddVertsForText2D(verts, Vec2(0.f, 190.f), 5.f, modeName, Rgba8(255, 255, 0));
	font->AddVertsForText2D(verts, Vec2(0.f, 180.f), 5.f, "F6:Prev F7:Next F8:Randomize FPS:"+std::to_string(Clock::GetSystemClock().GetDeltaSeconds()), Rgba8(0, 255, 255));
	font->AddVertsForText2D(verts, Vec2(0.f, 175.f- g_theGame->m_textSize), g_theGame->m_textSize, g_theGame->m_extraText, Rgba8(255, 255, 255));
	g_theRenderer->BindTexture(&font->GetTexture());
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*m_camera);
}