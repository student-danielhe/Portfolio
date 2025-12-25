#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "GameCommon.hpp"
#include "Map.hpp"
#include "MapGenerator.hpp"
//#include "Engine/Input/InputSystem.hpp"
class Game
{
public:
	std::vector<bool> m_switch;
	std::vector<int> m_value;
	std::vector<Map*> m_maps;
	Map* m_currentMap = nullptr;
	Object* m_player = nullptr;
	//RandomNumberGenerator* g_RNG = nullptr;
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;

	FullDungeonMatrix* m_fullMetrix = nullptr;
	int m_selectedMetrix=0;

public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void BeginFrame(){}
	void EndFrame(){}
	Game(){}
	~Game();
	void DebugRender();
	void RenderAttractMode();
	void RenderImGui();

	void ImGuiGlobalSetting();
	void ImGuiLocalSetting();
	void ImGuiGameplaySetting();

	void EnterAttractMode();
	void ExitAttractMode();
	void HandleKeyPress();
	void LoadSound();

	void ChangeMap(Map* map, IntVec2 startPos, bool retainDirection=true);
private:
	bool m_IsDebugging=false;
	
};