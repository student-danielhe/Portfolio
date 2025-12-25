#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Player.hpp"
#include "Block.hpp"
#include "Engine/Core/Clock.hpp"
//#include "Engine/Input/InputSystem.hpp"
class Map;
class Game {
public:
	
	Rgba8 m_skyColor= Rgba8::WHITE;
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;
	Player* m_player = nullptr;
	Map* m_currentMap = nullptr;
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void BeginFrame();
	void EndFrame();
	Game(){}
	~Game();
	void RenderAttractMode();
	void EnterAttractMode();
	void ExitAttractMode();
	void HandleKeyPress();
	void LoadSound();
    
	void BindWorldConstant();

	std::string CameraState();
	std::string PlayerState();
private:
	
private:
	bool m_IsDebugging=false;
};