#include "Game.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Map.hpp"
#include "Engine/Window/Window.hpp"
#include "Player.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include <math.h>
RandomNumberGenerator* g_RNG = nullptr;
SpriteSheet* g_terrainSheet = nullptr;
BitmapFont* g_testFont = nullptr;

void Game::StartUp() {
	 Texture* terrainSheet = g_theRenderer->CreateOrGetTextureFromFile("Images/Terrain_8x8.png");
	 g_terrainSheet = new SpriteSheet(*terrainSheet, IntVec2(8, 8));
	 LoadSoundAndTexture();
	 g_ScreenCamera = new Camera();
	 g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	 g_RNG = new RandomNumberGenerator();
	 
	
	 

	 m_player = new Player(m_currentMap, Vec2(2.5, 2.5), 0);
	 g_theRenderer->Set2DDefault();
	 
	 EnterAttractMode();
}
void Game::Update(float deltaSecond) {
	
	if (m_attractMode) {
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		if (controller.IsConnected() && (controller.WasButtonReleased(XBOX_BUTTON_A) || controller.WasButtonReleased(XBOX_BUTTON_START))) {
			if (m_victory_Scene) {
				m_victory_Scene = false;
			}
			else {
				ExitAttractMode();
			}
		}
		else if (g_theInput->WasKeyJustReleased(' ')|| g_theInput->WasKeyJustReleased('N')) {
			if (m_victory_Scene) {
				m_victory_Scene = false;
			}
			else {
				ExitAttractMode();
			}
			
		}	
	}
	else {
		//not in attract mode
		if (m_currentMap) {
			HandleKeyPress();
			if (!m_victory_Scene) {
				if (m_currentMap->m_player->m_isDead) {
					if (!m_deathScene) {
						m_deathCountDown -= deltaSecond;
						if (m_deathCountDown <= 0) {
							m_deathScene = true;
							m_deathCountDown = 3;
						}
					}
				}
				if (!m_deathScene) {
					m_currentMap->Update(deltaSecond);
					//check player going to next map
					if (playerReachedGoal()) {
						if (m_currentMap->m_mapIndex + 1 == m_mapList.size()) {
							m_victory_Scene = true;
						}
						else {
							MapTransition(m_mapList[m_currentMap->m_mapIndex + 1]);
						}

					}
				}
			}
			
		}
		

		
	}
}


void Game::Render() {
	if (m_attractMode) {
		g_theRenderer->BeginCamera(*g_ScreenCamera);
		RenderAttractMode();
		g_theRenderer->EndCamera(*g_ScreenCamera);
	}
	else {
		if (m_currentMap) {
			if (m_victory_Scene) {
				RenderScene();
			}
			else {
				g_ScreenCamera->SetOrthoView(m_currentMap->CameraPosition(), Vec2(m_currentMap->CameraPosition().x + g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f), m_currentMap->CameraPosition().y + g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f)));
				if (m_debugCamera) {
					float aspectRatio = g_theWindow->GetConfig().m_aspectRatio;
					float WorldSizeRatio = WORLD_SIZE_X / WORLD_SIZE_Y;
					if (WorldSizeRatio > aspectRatio) {
						//horizontally filled
						g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(m_currentMap->m_dimensions.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), m_currentMap->m_dimensions.y * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f) * (aspectRatio / WorldSizeRatio)));
					}
					else if (WorldSizeRatio < aspectRatio) {
						//vertically filled
						g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(m_currentMap->m_dimensions.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f) * (aspectRatio / WorldSizeRatio), m_currentMap->m_dimensions.y * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f)));
					}
					else {
						g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(m_currentMap->m_dimensions.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), m_currentMap->m_dimensions.y * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f)));
					}


				}
				g_theRenderer->BeginCamera(*g_ScreenCamera);
				m_currentMap->Render();
				g_theRenderer->EndCamera(*g_ScreenCamera);
				if (m_deathScene) {
					RenderScene();
				}
			}
			
		}
		
		
	}

	//-----------------------------------
	//render dev console
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	g_theRenderer->EndCamera(*g_ScreenCamera);
	

}


void Game::RenderAttractMode() {
	Texture* attractTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/AttractScreen.png");
	g_theRenderer->BindTexture(attractTexture);
	Vertex_PCU* startIcon = new Vertex_PCU[4];
	for (int i = 0; i < 4; i++) {
		startIcon[i].m_color = Rgba8(255, 255, 255);
	}
	float x = g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f);
	float y = g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f);
	startIcon[0].m_position = Vec3(0, 0, 0);
	startIcon[0].m_uvTexCoords = Vec2(0, 0);
	startIcon[1].m_position = Vec3(x, 0, 0);
	startIcon[1].m_uvTexCoords = Vec2(1,0);
	startIcon[2].m_position = Vec3(x, y, 0);
	startIcon[2].m_uvTexCoords = Vec2(1, 1);
	startIcon[3].m_position = Vec3(0, y, 0);
	startIcon[3].m_uvTexCoords = Vec2(0, 1);
	std::vector<Vertex_PCU> toRender;
	AddVertsForRect(toRender, startIcon[0], startIcon[1], startIcon[2], startIcon[3]);
	g_theRenderer->DrawVertexArray(toRender);
}

void Game::EnterAttractMode() {
	g_ScreenCamera->SetOrthoView(Vec2(0,0), Vec2(g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f), g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f)));
	m_attractMode = true;
	m_BGMPlayBack = g_theAudio->StartSound(m_BGM, true);
	g_theAudio->StopSound(m_InGamelayBack);
	g_theAudio->StartSound(m_Button);
}
void Game::ExitAttractMode() {
	g_theAudio->StartSound(m_Button);
	m_attractMode = false;
	g_theAudio->StopSound(m_BGMPlayBack);
	m_InGamelayBack = g_theAudio->StartSound(m_InGame, true);
	Map* map1 = new Map(16, 16);
	map1->StartUp();
	map1->m_mapIndex = 0;
	Map* map2 = new Map(24,16);
	map2->StartUp();
	map2->m_mapIndex = 1;
	Map* map3 = new Map(16, 24);
	map3->StartUp();
	map3->m_mapIndex = 2;
	m_mapList.push_back(map1);
	m_mapList.push_back(map2);
	m_mapList.push_back(map3);
	MapTransition(m_mapList[0]);
	
	//m_currentMap->spawnStartingEnemies();
	TileDefinition::InitiateTileType();
}
void Game::HandleKeyPress() {
	if (m_deathScene) {
		if (g_theInput->WasKeyJustReleased('N')) {
			//respawn
			g_theAudio->StartSound(m_Respawn);
			m_currentMap->m_player->m_isDead = false;
			m_currentMap->m_player->m_health = g_gameConfigBlackboard.GetValue("PLAYER_MAX_HP", 30);
			m_deathScene = false;
		}
	}
	if (m_victory_Scene) {
		if (g_theInput->WasKeyJustReleased(' ') || g_theInput->WasKeyJustReleased('p') || g_theInput->WasKeyJustReleased(27)) {
			EnterAttractMode();
		}
	}
	if (g_theInput->WasKeyJustReleased(0x70)) {
		m_debugLine = !m_debugLine;
	}
	if (g_theInput->WasKeyJustReleased(0x72)) {
		m_noclip = !m_noclip;
	}
	if (g_theInput->WasKeyJustReleased(0x73)) {
		m_debugCamera = !m_debugCamera;
	}

	if (g_theInput->WasKeyJustReleased('M')) {
		m_renderHeaMap = !m_renderHeaMap;
	}
}

void Game::LoadSoundAndTexture() {
	m_BGM = g_theAudio->CreateOrGetSound("Audio/Attract.mp3");
	m_InGame= g_theAudio->CreateOrGetSound("Audio/Game.mp3");
	m_Explosion = g_theAudio->CreateOrGetSound("Audio/Explosion.wav");
	m_Respawn   = g_theAudio->CreateOrGetSound("Audio/Respawn.mp3");
	m_Shoot     = g_theAudio->CreateOrGetSound("Audio/Shoot.mp3");
	m_Button    = g_theAudio->CreateOrGetSound("Audio/Button.mp3");
	m_Vicotry   = g_theAudio->CreateOrGetSound("Audio/Victory.wav");
	m_defeat    = g_theAudio->CreateOrGetSound("Audio/Defeat.wav");
	//---------------------------------------
	g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyCannon.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyShell.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyTank4.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyTank1.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyTurretBase.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/PlayerTankTop.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/PlayerTankBase.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/Terrain_8x8.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/VictoryScreen.jpg");
	g_theRenderer->CreateOrGetTextureFromFile("Images/YouDiedScreen.png");
	g_theRenderer->CreateOrGetTextureFromFile("Images/Test_StbiFlippedAndOpenGL.png");
}
void Game:: RenderScene()const {
	Texture* Scene;
	if (m_victory_Scene) {
		Scene = g_theRenderer->CreateOrGetTextureFromFile("Images/VictoryScreen.jpg");
	}
	else if(m_deathScene){
		Scene = g_theRenderer->CreateOrGetTextureFromFile("Images/YouDiedScreen.png");
	}
	else {
		Scene = g_theRenderer->CreateOrGetTextureFromFile("Images/AttractScreen.png");
	}

	g_theRenderer->BindTexture(Scene);
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(static_cast<float>(Scene->GetDimensions().x),static_cast<float>(Scene->GetDimensions().y)));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	std::vector<Vertex_PCU>verts;
	Vertex_PCU BL(Vec3(0, 0,0), Rgba8(255, 255, 255), Vec2(0, 0));
	Vertex_PCU BR(Vec3(static_cast<float>(Scene->GetDimensions().x), 0, 0), Rgba8(255, 255, 255), Vec2(1, 0));
	Vertex_PCU TR(Vec3(static_cast<float>(Scene->GetDimensions().x), static_cast<float>(Scene->GetDimensions().y), 0), Rgba8(255, 255, 255), Vec2(1, 1));
	Vertex_PCU TL(Vec3(0, static_cast<float>(Scene->GetDimensions().y), 0), Rgba8(255, 255, 255), Vec2(0, 1));
	AddVertsForRect(verts, BL, BR, TR, TL);
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}


Game::~Game() {

}

void Game::BeginFrame() 
{
	if(m_currentMap)
	m_currentMap->BeginFrame();
}
void Game::EndFrame() {
	if (m_currentMap)
	m_currentMap->EndFrame();
}

void Game::MapTransition(Map* targetMap) {
	m_currentMap = targetMap;
	m_currentMap->AddEntityToMap(m_player);
	WORLD_SIZE_X = m_currentMap->m_dimensions.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
	WORLD_SIZE_Y = m_currentMap->m_dimensions.y * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
}
bool Game::playerReachedGoal() {
	float x = static_cast<float>(m_currentMap->m_dimensions.x - 2.5) * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
	float y = static_cast<float>(m_currentMap->m_dimensions.y - 2.5) * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
	return IsPointInsideDisc2D(Vec2(x, y), m_player->m_position, m_player->m_cosmeticRadius+1.f)||g_theInput->WasKeyJustReleased(KEYCODE_F9);
}