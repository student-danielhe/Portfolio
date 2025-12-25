#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/DebugProp.hpp"
extern std::vector<DebugProp*> m_debugRenderProps;
extern std::vector<DebugProp*> m_debugRenderScreenProps;
Camera* g_ScreenCamera = nullptr;
BitmapFont* g_font = nullptr;
Map* Game::g_map = nullptr;
Clock* Game::g_gameClock = nullptr;
SoundID Game::m_CubePlacement;
SoundID Game::m_Damage;
SoundID Game::m_Heal;
SoundID Game::m_Melee;
SoundID Game::m_Shoot;
SoundID Game::m_Explosion;
SoundID Game::m_Tick;
SoundID Game::m_Tock;

RandomNumberGenerator* g_RNG = nullptr;
void Game::StartUp() {
	g_font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLDSIZEX, WORLDSIZEY));
	
	g_RNG = new RandomNumberGenerator();

	g_gameClock = new Clock();
	
	DebugRenderConfig config;
	config.m_renderer = g_theRenderer;
	DebugRenderSystemStartUp(config);
	SubscribeEventCallbackFunction("DEBUG CLEAR", Command_DebugRenderClear);
	SubscribeEventCallbackFunction("DEBUG TOGGLE", Command_DebugRenderToggle);
	EnterAttractMode();
	LoadSound();
}
void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	if (m_attractMode) {
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		if (controller.IsConnected() && (controller.WasButtonReleased(XBOX_BUTTON_A)|| controller.WasButtonReleased(XBOX_BUTTON_START))) {
			ExitAttractMode();

		}
		else if (g_theInput->WasKeyJustReleased(' ')|| g_theInput->WasKeyJustReleased('N')) {
			if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
				ExitAttractMode();
			}
		}
	}
	else {
		//not in attract mode
		HandleKeyPress();
		float gameDeltaSecond = (float)g_gameClock->GetDeltaSeconds();
		g_map->Update(gameDeltaSecond);
		for (int i = 0; i < m_debugRenderProps.size(); i++) {
			DebugProp* prop = m_debugRenderProps[i];
			prop->Update(gameDeltaSecond);
			prop->m_currentDuration += gameDeltaSecond;
			if (prop->m_duration != -1.f) {
				if (prop->m_currentDuration >= prop->m_duration) {
					m_debugRenderProps.erase(m_debugRenderProps.begin() + i);
				}
			}
		}
		
		for (int i = 0; i < m_debugRenderScreenProps.size();i++) {
			DebugProp* prop = m_debugRenderScreenProps[i];
			prop->Update(gameDeltaSecond);
			prop->m_currentDuration += gameDeltaSecond;
			if (prop->m_duration != -1.f) {
				if (prop->m_currentDuration >= prop->m_duration) {
					m_debugRenderScreenProps.erase(m_debugRenderScreenProps.begin() + i);
				}
			}
		}

		if (m_state == AttractState::LOOSE || m_state == AttractState::WIN) {
			EnterAttractMode();
		}
	}

}


void Game::Render() {
	if (m_attractMode) {
		//clear screen
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);
		
		RenderAttractMode();
		
	}
	else {
		//render game mode
		//clear screen

		Rgba8 blue = Rgba8(0, 148, 255);
		g_theRenderer->ClearScreen(blue);
		g_theRenderer->BeginCamera(*g_ScreenCamera);
		g_map->Render();
		g_theRenderer->EndCamera(*g_ScreenCamera);
		
	}
	//g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	DebugRenderScreen(*g_ScreenCamera);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::DebugRender() {

}

void Game::RenderAttractMode() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theRenderer->SetSampleMode(SamplerMode::POINT_CLAMP);
	std::vector<Vertex_PCU> verts;
	if (m_state == AttractState::ATTRACT) {
	    g_theRenderer->BindTexture(&g_font->GetTexture());
		g_font->AddVertsForTextInBox2D(verts,"CUBE CHAOS", AABB2(Vec2(), Vec2(WORLDSIZEX, WORLDSIZEY)), 10.f);
	}
	else {
		AddVertsForRect(verts, AABB2(Vec2(), Vec2(WORLDSIZEX, WORLDSIZEY)));
		if (m_state == AttractState::WIN) {
			g_theRenderer->BindTexture(m_victoryTexture);
		}if (m_state == AttractState::LOOSE) {
			g_theRenderer->BindTexture(m_looseTexture);
		}
	}
	
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::EnterAttractMode() {
	m_attractMode = true;
	if(m_MainPlayback!=MISSING_SOUND_ID)
	    g_theAudio->StopSound(m_MainPlayback);
	DebugRenderSetHidden();
	delete g_map;
	g_map = nullptr;
}
void Game::ExitAttractMode() {
	m_attractMode = false;
	m_MainPlayback = g_theAudio->StartSound(m_MainSong, true);
	DebugRenderSetVisible();
	g_map= new Map(this);
	g_map->StartUp();
	m_state = AttractState::ATTRACT;
}
void Game::HandleKeyPress() {
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		if (g_theInput->WasKeyJustReleased('P')) {
			g_gameClock->TogglePause();
		}

		if (g_theInput->WasKeyJustReleased('O')) {
			g_gameClock->StepSingleFrame();
		}

		if (g_theInput->WasKeyJustPressed('T')) {
			g_gameClock->SetTimeScale(.1f);
		}
		if (g_theInput->WasKeyJustReleased('T'))
		{
			g_gameClock->SetTimeScale(1.f);
		}

		if (g_theInput->WasKeyJustPressed('Y')) {
			g_gameClock->SetTimeScale(10.f);
		}
		if (g_theInput->WasKeyJustReleased('Y'))
		{
			g_gameClock->SetTimeScale(1.f);
		}
	}
	
}

void Game::LoadSound() {
	Game::m_MainSong      = g_theAudio->CreateOrGetSound("Data/Sounds/Main_Song.wav");
	Game::m_CubePlacement = g_theAudio->CreateOrGetSound("Data/Sounds/Cube_Placement.wav");
	Game::m_Damage        = g_theAudio->CreateOrGetSound("Data/Sounds/Damageg.Wav");
	Game::m_Heal          = g_theAudio->CreateOrGetSound("Data/Sounds/Heal.wav");
	Game::m_Melee         = g_theAudio->CreateOrGetSound("Data/Sounds/Melee_Swing.wav");
	Game::m_Shoot         = g_theAudio->CreateOrGetSound("Data/Sounds/Shoot.Wav");
	Game::m_Explosion     = g_theAudio->CreateOrGetSound("Data/Sounds/Small_Explosion.wav");
	Game::m_Tick          = g_theAudio->CreateOrGetSound("Data/Sounds/Tick.wav");
	Game::m_Tock          = g_theAudio->CreateOrGetSound("Data/Sounds/Tock.wav");
	m_victoryTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/VictoryScreen.jpg");
	m_looseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/GameOver.png");
}


Game::~Game() {
	DebugRenderSystemShutdown();
}


void Game::BeginFrame() {
	DebugRenderBeginFrame();
}
void Game::EndFrame() {
	DebugRenderEndFrame();
}