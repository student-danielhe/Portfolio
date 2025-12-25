#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
//#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "TileDefinition.hpp"
#include "Player.hpp"
class Map;
class Game {
public:
	
	
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;
	SoundID m_BGM=0;
	SoundPlaybackID m_BGMPlayBack=0;
	SoundID m_InGame=0;
	SoundPlaybackID m_InGamelayBack=0;
	SoundID m_Explosion = 0;
	SoundID m_Respawn = 0;
	SoundID m_Shoot = 0;
	SoundID m_Button = 0;
	SoundID m_Vicotry = 0;
	SoundID m_defeat = 0;
	Map* m_currentMap = nullptr;
	Player* m_player=nullptr;
	std::vector<Map*> m_mapList;
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void BeginFrame();
	void EndFrame();
	Game(){}
	~Game();
	void DebugRender();
	void RenderAttractMode();
	void EnterAttractMode();
	void ExitAttractMode();
	void HandleKeyPress();
	void LoadSoundAndTexture();
	void RenderScene()const;
	void MapTransition(Map* targetMap);
	bool playerReachedGoal();
	bool m_noclip = false;
	bool m_debugLine = false;
	bool m_debugCamera = false;
	bool m_victory_Scene = false;
	bool m_renderHeaMap = false;
private:
	bool m_IsDebugging=false;
	float m_deathCountDown = 3.f;
	bool m_deathScene = false;
	
	
	
};