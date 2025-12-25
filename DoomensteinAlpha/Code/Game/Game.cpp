#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"

Camera* g_screenCamera = nullptr;
RandomNumberGenerator* g_RNG = nullptr;
Clock* g_gameClock = nullptr;
PlayerController* g_player1Controller = nullptr;
PlayerController* g_player2Controller = nullptr;

void Game::StartUp() {
	Texture* fontSheet = g_theRenderer->CreateOrGetTextureFromFile("Data/Fonts/SquirrelFixedFont.png");
	m_font = new SpriteSheet(*fontSheet, IntVec2(16, 16));
	g_screenCamera = new Camera();
	g_screenCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	
	g_RNG = new RandomNumberGenerator();

	g_gameClock = new Clock();
	

	
	MapDefinition::InitializeDefinition("Data/Definitions/MapDefinitions.xml");
	TileDefinition::InitiateTileType("Data/Definitions/TileDefinitions.xml");
	ActorDefinition::InitializeDefinition("Data/Definitions/ActorDefinitions.xml","Data/Definitions/ProjectileActorDefinitions.xml");
	WeaponDefinition::InitializeDefinition("Data/Definitions/WeaponDefinitions.xml");

/*
	m_currentLoadedMap = new Map(this, FindDefinition());
	m_currentLoadedMap->StartUp();
	m_currentLoadedMap->m_lightConstants = new LightConstants();*/
	EnterState(GameState::ATTRACT);
}
void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	if (m_currentState==GameState::ATTRACT) {
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		if (controller.IsConnected() && ( controller.WasButtonReleased(XBOX_BUTTON_START))) {
			if (m_player2) {
				EnterState(GameState::PLAYING);
			}
			else {
				m_player2 = true;
			}

		}
		else if (g_theInput->WasKeyJustReleased(' ')) {
			if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
				if (m_player1) {
					EnterState(GameState::PLAYING);
				}
				else {
					m_player1 = true;
				}
				
			}
		}
		if (g_theInput->WasKeyJustReleased('G')) {
			m_gold = true;
			EnterState(GameState::PLAYING);
		}
	}
	else {
		//not in attract mode
		HandleKeyPress();
		float gameDeltaSecond = (float)g_gameClock->GetDeltaSeconds();

		m_currentLoadedMap->Update(gameDeltaSecond);
		
		

		
	}

}


void Game::Render() {
	if (m_currentState==GameState::ATTRACT) {
		//clear screen
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);
		
		RenderAttractMode();
		
	}
	else {
		//render game mode
		//clear screen

		Rgba8 darkOrange = Rgba8(100, 100, 100);
		g_theRenderer->ClearScreen(darkOrange);
		g_player1Controller->UpdateCamera();
		//-----------------------------------------------------
		if (m_currentLoadedMap->m_splitScreen) {
			g_player2Controller->UpdateCamera();
		}
		m_currentLoadedMap->Render();

		
	}
	//g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));
	g_theRenderer->BeginCamera(*g_screenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theDevConsole->Render(AABB2(g_screenCamera->GetOrthoBottomLeft(), g_screenCamera->GetOrthoTopRight()), g_theRenderer);

	g_theRenderer->EndCamera(*g_screenCamera);
}


void Game::RenderAttractMode() {
	g_theRenderer->BeginCamera(*g_screenCamera);
	std::vector<Vertex_PCU> verts;
	//AddVertsForRect(verts, Vec2(4, 4), Vec2(12, 4), Vec2(12, 12), Vec2(4, 12), Rgba8(255, 255, 255),AABB2());
	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	std::string mode = "Press Space/Start to join, Press G for Gold";
	if (m_player1 && m_player2) {
		mode = "Multiplayer enabled";
	}
	else if (m_player1 || m_player2) {
		mode = "Single player enabled";
	}

	font->AddVertsForText2D(verts, Vec2(6.f, 6.f), 1.f, mode);
	g_theRenderer->BindTexture(&font->GetTexture());
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_screenCamera);
}

void Game::EnterState(GameState state) {
	//ExitState(m_currentState);
	if (state == GameState::PLAYING) {
		MapDefinition* defaultDef = MapDefinition::s_definitions[0];
		for (MapDefinition* def : MapDefinition::s_definitions) {
			if (def->m_name.c_str() == m_defaultMapName.c_str()) {
				defaultDef = def;
				break;
			}
		}
		m_currentState = state;
		m_currentLoadedMap = new Map(this, defaultDef);
		if (m_gold) {
			m_currentLoadedMap->m_gold = true;
		}
		else if (m_player1 && m_player2) {
			m_currentLoadedMap->m_splitScreen = true;
		}
		m_currentLoadedMap->StartUp();
	}
	
}
void Game::ExitState(GameState state) {
	if (state == GameState::PLAYING) {
		delete m_currentLoadedMap;
		m_currentLoadedMap = nullptr;
		m_currentState = GameState::ATTRACT;
	}
	//m_currentState = m_nextState;
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
		
	}
	
}

void Game::LoadAll() {
	MapDefinition::InitializeDefinition("Data/Definitions/MapDefinitions.xml");
	TileDefinition::InitiateTileType("Data/Definitions/TileDefinitions.xml");
}


Game::~Game() {
	MapDefinition::s_definitions.clear();
	TileDefinition::s_definitions.clear();
	delete g_RNG;
	g_RNG = nullptr;
	delete g_gameClock;
	g_gameClock = nullptr;
	delete m_font;
	m_font = nullptr;
}

void Game::BeginFrame() {
}
void Game::EndFrame() {
}

MapDefinition* Game::FindDefinition() {
	XmlDocument doc;
	XmlResult result = doc.LoadFile("Data/GameConfig.xml");
	UNUSED(result);
	XmlElement* rootElement = doc.RootElement();
	XmlElement* gameConfigElement = rootElement;

	MapDefinition* finalDefinition = MapDefinition::s_definitions[0];
	for (MapDefinition* def : MapDefinition::s_definitions) {
		if (def->m_name.compare(ParseXmlAttribute(*gameConfigElement, "defaultMap", "TestMap")) == 0) {
			finalDefinition = def;
		}
	}
	return finalDefinition;
}