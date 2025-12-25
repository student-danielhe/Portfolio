#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "TileDefinition.hpp"
#include "ObjectDefinition.hpp"
#include "StairInteraction.hpp"
#include "MonsterInteraction.hpp"
#include "ShortcutInteraction.hpp"
#include "MapGenerator.hpp"

float ENEMYSPAWN_MAINPATH_BIAS = .5f;
float ENEMYSPAWN_SPLITPATH_BIAS = .2f;
float ENEMYSPAWN_ENEMY_BIAS = -1.f;
float ENEMYSPAWN_LOOT_BIAS = 1.5f;
float ENEMYSPAWN_OPENESS_BIAS = .2f;
int ENEMY_APART = 5; 

float LOOTSPAWN_MAINPATH_BIAS = -.5f;
float LOOTSPAWN_SPLITPATH_BIAS = 1.f;
float LOOTSPAWN_LOOT_BIAS = -2.f;
float LOOTSPAWN_OPENESS_BIAS = .2f;
int LOOT_APART = 5;


int GAME_SEED = 0;
class Clock;
Clock* g_gameClock=nullptr;
void Game::StartUp() {
	LoadSound();
	 g_ScreenCamera = new Camera();
	 g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	
	//g_RNG = new RandomNumberGenerator();
	g_gameClock = new Clock();
	ObjectDefinition::InitializeObjectDefinition();
	TileDefinition::InitializeTileDefinition();
	MapDefinition::InitializeDefinition("Data/Definition/MapDefinition.xml");
	EnterAttractMode();
}
void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	HandleKeyPress();
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
		
		m_currentMap->Update(deltaSecond);
	}
	
}


void Game::Render() {
	if (m_attractMode) {
	    g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
		g_theRenderer->BeginCamera(*g_ScreenCamera);
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
		g_theRenderer->SetSampleMode(SamplerMode::POINT_CLAMP);
		RenderAttractMode();
		g_theRenderer->EndCamera(*g_ScreenCamera);
	}
	else {
		g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
		g_theRenderer->BeginCamera(*g_ScreenCamera);
	    m_currentMap->Render();
		g_theRenderer->EndCamera(*g_ScreenCamera);
	}

	//Render Dev Console
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(30, 15));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::DebugRender() {

}

void Game::RenderAttractMode() {
	
	RenderImGui();
}

void Game::RenderImGui()
{
    

    ImGui::Begin("Settings");

	bool globalSetting = ImGui::CollapsingHeader("Global Setting");
	
	if (globalSetting) {
	    ImGuiGlobalSetting();
	}

	ImGui::NewLine();
	if (ImGui::CollapsingHeader("Current Level Setting")) {
		ImGuiLocalSetting();
	}

	ImGui::NewLine();
	if (ImGui::CollapsingHeader("Gameplay Setting")) {
		ImGuiGameplaySetting();
	}
	ImGui::NewLine();

	if (ImGui::Button("Generate Dungeon")) {
	    ExitAttractMode();
	}


	ImGui::End();
}

void Game::ImGuiGlobalSetting()
{
	ImGui::InputInt("Game Seed", &GAME_SEED);

	if (ImGui::Button("Randomize")) {
		RandomNumberGenerator rng;
		GAME_SEED = rng.RollRandomIntInRange(0, 999999);
	}

	ImGui::NewLine();

	std::string levelsText = "Dungeon Levels: " + std::to_string(m_fullMetrix->m_dungeonMatrix.size());
	ImGui::Text(levelsText.c_str());
	if (ImGui::Button("Add More Levels")) {
		m_fullMetrix->m_dungeonMatrix.push_back(new DungeonMetrics());
	}

	ImGui::SameLine();

	if (m_fullMetrix->m_dungeonMatrix.size() > 1) {
		if (ImGui::Button("Remove Last Level")) {
		    m_fullMetrix->m_dungeonMatrix.pop_back();
		}
	}


	ImGui::NewLine();

	ImGui::SliderInt("Current Level", &m_selectedMetrix, 0, m_fullMetrix->m_dungeonMatrix.size() - 1);
	
}

void Game::ImGuiLocalSetting()
{
	DungeonMetrics* currentMetrix = m_fullMetrix->m_dungeonMatrix[m_selectedMetrix];

	std::string roomTypeText;

	if (ImGui::SliderInt("Level Dimensions", &currentMetrix->m_dimensions.x, 80, 300)) {
		currentMetrix->m_dimensions. y = currentMetrix->m_dimensions.x;
	}


	ImGui::NewLine();


	int maxRoomSize = currentMetrix->m_dimensions.x * currentMetrix->m_dimensions.x / currentMetrix->m_roomPerMap;
	maxRoomSize = floorf(sqrtf(maxRoomSize)/1.5) - 5;



	std::string roomCountText = "Main Path Room Count: " + std::to_string(currentMetrix->m_pathLen);
	ImGui::Text(roomCountText.c_str());

	if (maxRoomSize>=3) {
		if (ImGui::Button("Add Main Path")) {
			currentMetrix->m_pathLen += 1;
			currentMetrix->m_roomPerMap += 1;
		}
		ImGui::SameLine();
	}
	else {
		ImGui::Text("Can't Add more Rooms, increase level dimension or reduce room size.");
	}
	if (currentMetrix->m_pathLen > 1) {
		if (ImGui::Button("Remove Main Path")) {
			currentMetrix->m_pathLen -= 1;
			currentMetrix->m_roomPerMap -= 1;
		}
	}
	else {
		ImGui::NewLine();
		ImGui::Text("Can't remove the last room");
	}
	


	ImGui::NewLine();

	std::string sideCountText = "Side Path Room Count: " + std::to_string(currentMetrix->m_roomPerMap - currentMetrix->m_pathLen);
	ImGui::Text(sideCountText.c_str());

	if (maxRoomSize>=3) {
		if (ImGui::Button("Add Side Path")) {
			currentMetrix->m_roomPerMap += 1;
		}
		ImGui::SameLine();
	}
	else {
		ImGui::Text("Can't Add more Rooms, increase level dimension or reduce room size.");
	}
	if (currentMetrix->m_roomPerMap - 1 > currentMetrix->m_pathLen) {
		if (ImGui::Button("Remove Side Path")) {

			currentMetrix->m_roomPerMap -= 1;
		}
	}
	else {
	    ImGui::NewLine();
		ImGui::Text("Can't reduce split path room anymore");
	}


	ImGui::NewLine();


	
	if (ImGui::SliderInt("Minimum Room Size", &currentMetrix->m_minimumRoomSize.x, 3, maxRoomSize)){
	    currentMetrix->m_minimumRoomSize. y = currentMetrix->m_minimumRoomSize.x;
	}

	ImGui::NewLine();

	ImGui::InputInt("Loot Amount", &currentMetrix->m_lootAmount);

	ImGui::NewLine();

	ImGui::InputInt("Enemy Amount", &currentMetrix->m_enemyAmount);

	ImGui::NewLine();

	if (currentMetrix->m_method == GenerationMethod::CELLULAR_AUTOMATA) {
		ImGui::SliderInt("Room Openness", &currentMetrix->m_openness, 0, 99);

		std::string replacementText = "Current Mix Rule: ";
		
		if (currentMetrix->m_replacementRule == ReplacementRule::NO_REPLACEMENT) {
			replacementText += "No Replacement";
		}

		if (currentMetrix->m_replacementRule == ReplacementRule::SQUARE) {
			replacementText += "Replace with Square Room";
		}

		if (currentMetrix->m_replacementRule == ReplacementRule::DRUNKARD) {
			replacementText += "Replace With Drunkard Cave";
		}


		ImGui::NewLine();

		ImGui::Text(replacementText.c_str());

		if (ImGui::Button("Switch Rule")) {
			if (currentMetrix->m_replacementRule == ReplacementRule::NO_REPLACEMENT) {
				currentMetrix->m_replacementRule = ReplacementRule::SQUARE;
			}

			else if (currentMetrix->m_replacementRule == ReplacementRule::SQUARE) {
				currentMetrix->m_replacementRule = ReplacementRule::DRUNKARD;
			}

			else if (currentMetrix->m_replacementRule == ReplacementRule::DRUNKARD) {
				currentMetrix->m_replacementRule = ReplacementRule::NO_REPLACEMENT;
			}
		}

		if (currentMetrix->m_replacementRule != ReplacementRule::NO_REPLACEMENT) {
			ImGui::NewLine();
			int maxAmount = currentMetrix->m_roomPerMap;

			ImGui::SliderInt("Replace Amount", &currentMetrix->m_replaceAmount, 0, maxAmount);
		}
	}




}

void Game::ImGuiGameplaySetting()
{
	ImGui::InputFloat("Enemy Spawn Rate Main Path Bias", &ENEMYSPAWN_MAINPATH_BIAS);
	ImGui::InputFloat("Enemy Spawn Rate Split Path Bias", &ENEMYSPAWN_SPLITPATH_BIAS);
	ImGui::InputFloat("Enemy Spawn Rate Other Enemy Bias", &ENEMYSPAWN_ENEMY_BIAS);
	ImGui::InputFloat("Enemy Spawn Rate Loot Bias", &ENEMYSPAWN_LOOT_BIAS);
	ImGui::InputFloat("Enemy Spawn Rate Openness Bias", &ENEMYSPAWN_OPENESS_BIAS);
	ImGui::InputInt("Enemy Spawn Rate Enemy Apart", &ENEMY_APART);

	ImGui::NewLine();

	ImGui::InputFloat("Loot Spawn Main Path Bias", &LOOTSPAWN_MAINPATH_BIAS);
	ImGui::InputFloat("Loot Spawn Split Path Bias", &LOOTSPAWN_SPLITPATH_BIAS);
	ImGui::InputFloat("Loot Spawn Other Loot Bias", &LOOTSPAWN_LOOT_BIAS);
	ImGui::InputFloat("Loot Spawn Openness Bias", &LOOTSPAWN_OPENESS_BIAS);
	ImGui::InputInt("Loot Spawn Loot Apart", &LOOT_APART);
}
void Game::HandleKeyPress() {

}
void Game::EnterAttractMode() {
	m_attractMode = true;
	delete m_currentMap;
	m_currentMap = nullptr;
	m_fullMetrix = new FullDungeonMatrix();
	m_fullMetrix->type = MapConnectionType::LINEAR;
	m_fullMetrix->m_dungeonMatrix.push_back(new DungeonMetrics());
}

void Game::ExitAttractMode() {
	m_attractMode = false;
	MapGenerator* testGenerator = new MapGenerator();
	std::vector<Map*> maps =  testGenerator->CreateFullDungeon(*m_fullMetrix);
	m_currentMap = maps[0];
}



void Game::LoadSound() {
	

}


void Game::ChangeMap(Map* map, IntVec2 startPos, bool retainDirction)
{
    Object* player = m_currentMap->m_player;
    
    m_currentMap->OnLeave();

	m_currentMap = map;
	
	if(retainDirction)
	    m_currentMap->OnEnter(player, startPos, player->m_currentDirection);
	else
	    m_currentMap->OnEnter(player, startPos);
}

Game::~Game() {

}