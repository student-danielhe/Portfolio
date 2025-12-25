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
#include "BlockDefinition.hpp"
#include "Chunk.hpp"
#include "Map.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "PlayerCharacter.hpp"
extern std::vector<DebugProp*> m_debugRenderProps;
extern std::vector<DebugProp*> m_debugRenderScreenProps;
RandomNumberGenerator* g_RNG = nullptr;
Clock* g_gameClock = nullptr;
SpriteSheet* g_sprite = nullptr;


void Game::StartUp() {
    

	m_IsDebugging = true;
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));
	
	g_RNG = new RandomNumberGenerator();

	m_player = new Player(this);
	g_thePlayerCamera = new Camera();
	g_thePlayerCamera->SetOrthoView(Vec2(-1, -1), Vec2(1, 1));
	
	g_gameClock = new Clock();
	
	DebugRenderConfig config;
	config.m_renderer = g_theRenderer;
	DebugRenderSystemStartUp(config);
	SubscribeEventCallbackFunction("DEBUG CLEAR", Command_DebugRenderClear);
	SubscribeEventCallbackFunction("DEBUG TOGGLE", Command_DebugRenderToggle);
	EnterAttractMode();

	Texture* sprite = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/SpriteSheet_Faithful_64x.png");
	g_sprite = new SpriteSheet(*sprite, IntVec2(8, 8));

	BlockDefinition::InitializeBlockDefinitions();

	Chunk::InitializeBiomeRules();

	if (!DoFolderExists("Saves")) {
		CreateFolder("Saves");
	}
	
}
void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	HandleKeyPress();
	if (m_attractMode) {
		if (g_theInput->WasKeyJustReleased(' ')|| g_theInput->WasKeyJustReleased('N')) {
			if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
				ExitAttractMode();
			}
		}
	}
	else {
		//not in attract mode

		if(!g_gameClock->IsPaused())
		    m_player->Update(deltaSecond);
		
		float gameDeltaSecond = (float)g_gameClock->GetDeltaSeconds();
		if(!g_gameClock->IsPaused())
		    m_currentMap->Update(gameDeltaSecond);

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
		
		float skyColorfraction = fabsf(m_currentMap->m_worldTime - .5f)*2.f;

		m_skyColor = Rgba8((unsigned char)(200 - skyColorfraction*(200-20)), (unsigned char)(230 - skyColorfraction*(230-20)), (unsigned char)(255-skyColorfraction*(255-40)));

		BindWorldConstant();
		g_theRenderer->ClearScreen(m_skyColor);
		g_thePlayerCamera->SetPerspectiveView(g_theWindow->GetAspectRatio(), 60.f, .1f, 1000.f);
		Mat44 CamToRender(Vec4(0.f, -1.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 1.f, 0.f),
			Vec4(1.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 1.f));
		CamToRender.Transpose();
		g_thePlayerCamera->SetCameraToRenderTransform(CamToRender);
		g_theRenderer->BeginCamera(*g_thePlayerCamera);
		g_theRenderer->BindTexture(&g_sprite->GetTexture());

		m_currentMap->Render();

		DebugRenderWorld(*g_thePlayerCamera);

		g_theRenderer->EndCamera(*g_thePlayerCamera);
		
	}
	g_ScreenCamera->SetOrthoView(Vec2(),Vec2(48,24));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);

	g_theRenderer->SetBuffers();

	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);

	if (!m_attractMode) {
	    

		std::string FPSInfo = "FPS: " + std::to_string(1 / g_systemClock->GetDeltaSeconds());
		std::string ChunkInfo = "Chunk: " + std::to_string(m_currentMap->m_activeChunks.size()) + " Vets: " + std::to_string(m_currentMap->GetVertexCount()) + " Indices: " + std::to_string(m_currentMap->GetIndexCount());
		std::string CameraInfo = CameraState();
		std::string PlayerInfo = PlayerState();
		
		g_theRenderer->BindTexture(&g_font->GetTexture());
		std::vector<Vertex_PCU> verts;
		g_font->AddVertsForText2D(verts, Vec2(36.f, 23.f), .7f, FPSInfo);
		g_font->AddVertsForText2D(verts, Vec2(28.f, 22.f), .7f, CameraInfo);
		g_font->AddVertsForText2D(verts, Vec2(0.f, 23.f), .7f, ChunkInfo);
		g_font->AddVertsForText2D(verts, Vec2(0.f, 22.f), .7f, PlayerInfo);
		g_theRenderer->DrawVertexArray(verts);
	}
	
	
	DebugRenderScreen(*g_ScreenCamera);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}


void Game::RenderAttractMode() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	std::vector<Vertex_PCU> verts;
	AddVertsForRect(verts, Vec2(4, 4), Vec2(12, 4), Vec2(12, 12), Vec2(4, 12), Rgba8(255, 255, 255),AABB2());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::EnterAttractMode() {
    delete m_player;
	m_player = nullptr;
	m_attractMode = true;
	DebugRenderSetHidden();
	delete m_currentMap;
	m_currentMap = nullptr;
}
void Game::ExitAttractMode() {
    m_player = new Player(this);
	m_attractMode = false;
	DebugRenderSetVisible();
	m_currentMap= new Map();
	m_player->m_character = new PlayerCharacter(m_currentMap);
	m_player->m_character->m_bottomPosition = m_player->m_position;
	m_player->m_cameraMode = CAMERAMODE_FIRST_PERSON;
	m_player->m_character->m_physicsState = PhysicsStates::WALKING;
	m_player->m_character->m_camera = m_player;
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

		if (g_theInput->WasKeyJustReleased(KEYCODE_F8)) {
			EnterAttractMode();
			ExitAttractMode();

		}
		if (g_theInput->WasKeyJustReleased(KEYCODE_F2)) {
			m_currentMap->m_debugging = !m_currentMap->m_debugging;

		}
	}
	
}

void Game::LoadSound() {

}


void Game::BindWorldConstant()
{
    WorldConstants wc;
	wc.CameraPosition = Vec4(m_player->GetPosition().x, m_player->GetPosition().y, m_player->GetPosition().z, 0);
	wc.OutdoorLightColor = Vec4(1.f,1.f,1.f,1.f);
	wc.IndoorLightColor = Vec4(1.f, 230.f/255.f, 204.f/255.f, 1.f);
	wc.FogFarDistance = CHUNK_ACTIVATION_RANGE - 2.f * CHUNK_SIZE_X;
	wc.FogNearDistance = wc.FogFarDistance/2.f;
	wc.SkyColor = Vec4(m_skyColor.r/255.f, m_skyColor.g/255.f, m_skyColor.b/255.f, 1.f);
	g_theRenderer->SetWorldConstants(wc);
}

std::string Game::CameraState()
{
	if (m_player->m_cameraMode == CAMERAMODE_FIRST_PERSON) {
		return "Camera Mode: First Person";
	}

	if (m_player->m_cameraMode == CAMERAMODE_OVER_SHOULDER) {
		return "Camera Mode: Over Shoulder";
	}

	if (m_player->m_cameraMode == CAMERAMODE_SPECTATOR_FULL) {
		return "Camera Mode: Spectator Full";
	}

	if (m_player->m_cameraMode == CAMERAMODE_SPECTATOR_XY) {
		return "Camera Mode: Spectator XY";
	}

	if (m_player->m_cameraMode == CAMERAMODE_INDEPENDENT) {
		return "Camera Mode: Independent";
	}
	return"";
}

std::string Game::PlayerState()
{
	if (m_player->m_character->m_physicsState == PhysicsStates::WALKING) {
		return "Physics State: Walking";
	}

	if (m_player->m_character->m_physicsState == PhysicsStates::FLYING) {
		return "Physics State: Flying";
	}

	if (m_player->m_character->m_physicsState == PhysicsStates::NOCLIP) {
		return "Physics State: No Clip";
	}
	return "";
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