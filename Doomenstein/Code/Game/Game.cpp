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
BitmapFont* g_squirrelFont = nullptr;
void Game::StartUp() {
	Texture* fontSheet = g_theRenderer->CreateOrGetTextureFromFile("Data/Fonts/SquirrelFixedFont.png");
	m_font = new SpriteSheet(*fontSheet, IntVec2(16, 16));
	g_screenCamera = new Camera();
	g_screenCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	
	g_RNG = new RandomNumberGenerator();

	g_gameClock = new Clock();
	
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	
	MapDefinition::InitializeDefinition("Data/Definitions/MapDefinitions.xml");
	TileDefinition::InitiateTileType("Data/Definitions/TileDefinitions.xml");
	ActorDefinition::InitializeDefinition("Data/Definitions/ActorDefinitions.xml","Data/Definitions/ProjectileActorDefinitions.xml");
	WeaponDefinition::InitializeDefinition("Data/Definitions/WeaponDefinitions.xml");

	EnterState(GameState::ATTRACT);
}
void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	if (m_currentState==GameState::ATTRACT) {
		if (g_theInput->WasKeyJustReleased(' ')) {
			if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
				EnterState(GameState::PLAYING);
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
		if (m_currentState == GameState::PLAYING) {
			float gameDeltaSecond = (float)g_gameClock->GetDeltaSeconds();
			m_currentLoadedMap->Update(gameDeltaSecond);
		}
	}
}


void Game::Render() {
	if (m_currentState==GameState::ATTRACT) {
		//clear screen
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);
		
		RenderAttractMode();
		
	}
	else if (m_currentState == GameState::GAMEOVER) {
		//clear screen
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);

		RenderGameOver();

	}
	else if (m_currentState == GameState::VICTORY) {
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);
		RenderVictory();
	}
	else {
		//render game mode
		//clear screen
		if (m_currentLoadedMap&&g_player1Controller) {
			Rgba8 darkOrange = Rgba8(100, 100, 100);
			g_theRenderer->ClearScreen(darkOrange);
			g_player1Controller->UpdateCamera();
			m_currentLoadedMap->Render();
		}
		

		
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
	AddVertsForRect(verts, Vec2(SCREEN_SIZE_X, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y), Vec2(0.f, SCREEN_SIZE_Y), Vec2(0.f, 0.f), Rgba8::WHITE, AABB2::DEFAULT);
	g_theRenderer->BindTexture(g_theRenderer->CreateOrGetTextureFromFile("Data/Gold/Devil.png"));
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();
	//AddVertsForRect(verts, Vec2(4, 4), Vec2(12, 4), Vec2(12, 12), Vec2(4, 12), Rgba8(255, 255, 255),AABB2());
	
	std::string mode = "Press G for Gold";
	g_squirrelFont->AddVertsForText2D(verts, Vec2(16.f, 6.f), 1.f, mode);
	g_theRenderer->BindTexture(&g_squirrelFont->GetTexture());
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_screenCamera);
}
void Game::RenderGameOver() {
	g_theRenderer->BeginCamera(*g_screenCamera);
	std::vector<Vertex_PCU> verts;
	AddVertsForRect(verts, Vec2(SCREEN_SIZE_X,0.f),Vec2(SCREEN_SIZE_X,SCREEN_SIZE_Y),Vec2(0.f,SCREEN_SIZE_Y),Vec2(0.f,0.f),Rgba8::WHITE,AABB2::DEFAULT);
	g_theRenderer->BindTexture(g_theRenderer->CreateOrGetTextureFromFile("Data/Gold/GameOverGold.png"));
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();

	std::string mode = "Press Space to continue";
	g_squirrelFont->AddVertsForText2D(verts, Vec2(2.f, 1.f), 1.f, mode);
	g_theRenderer->BindTexture(&g_squirrelFont->GetTexture());
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_screenCamera);
}

void Game::RenderVictory()
{
	g_theRenderer->BeginCamera(*g_screenCamera);
	std::vector<Vertex_PCU> verts;
	AddVertsForRect(verts, Vec2(SCREEN_SIZE_X, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y), Vec2(0.f, SCREEN_SIZE_Y), Vec2(0.f, 0.f), Rgba8::WHITE, AABB2::DEFAULT);
	g_theRenderer->BindTexture(g_theRenderer->CreateOrGetTextureFromFile("Data/Gold/Victory.png"));
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();

	std::string mode = "Press Space to continue";
	g_squirrelFont->AddVertsForText2D(verts, Vec2(14.f, 3.f), 1.f, mode);
	g_theRenderer->BindTexture(&g_squirrelFont->GetTexture());
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_screenCamera);
}

void Game::EnterState(GameState state) {
	m_currentState = state;
	if (state == GameState::ATTRACT||state==GameState::VICTORY) {
		SoundID menu = g_theAudio->CreateOrGetSound("Data/Audio/Music/MainMenu_InTheDark.mp2");
		m_menu = g_theAudio->StartSound(menu, true);
	}
	else {
		if(m_menu!=MISSING_SOUND_ID)
		g_theAudio->StopSound(m_menu);
	}
	if (state == GameState::GAMEOVER) {
		SoundID over = g_theAudio->CreateOrGetSound("Data/Gold/Acceptance.mp3");
		m_gameOver = g_theAudio->StartSound(over,true);
	}
	else {
		if (m_gameOver != MISSING_SOUND_ID)
			g_theAudio->StopSound(m_gameOver);
	}
	if (state == GameState::PLAYING) {
		MapDefinition* defaultDef = MapDefinition::s_definitions[0];
		for (MapDefinition* def : MapDefinition::s_definitions) {
			if (def->m_name.c_str() == m_defaultMapName.c_str()) {
				defaultDef = def;
				break;
			}
		}
	
		m_currentLoadedMap = new Map(this, defaultDef);
		if (m_gold) {
			m_currentLoadedMap->m_gold = true;
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
		if (g_theInput->WasKeyJustReleased(' ')||g_theInput->WasKeyJustReleased(27)||g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
			if(m_currentState==GameState::GAMEOVER||m_currentState==GameState::VICTORY)
			    EnterState(GameState::ATTRACT);
		}
	}
	
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
	if (m_currentLoadedMap){
		delete m_currentLoadedMap;
		m_currentLoadedMap = nullptr;
	}

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