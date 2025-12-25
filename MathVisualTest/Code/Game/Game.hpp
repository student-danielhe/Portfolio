#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "GameCommon.hpp"
//#include "Engine/Input/InputSystem.hpp"
enum GameMode {
	GAME_MODE_NEAREST_POINT,
	GAME_MODE_RAYCAST_VS_DISCS,
	GAME_MODE_RAYCAST_VS_LINE,
	GAME_MODE_RAYCAST_VS_AABB2,
	GAME_MODE_3D,
	GAME_MODE_2D_CURVE,
	GAME_MDOE_PACHINKO,
	NUM_GAME_MODE,
};

class Game {
public:
	
	RandomNumberGenerator* g_RNG = nullptr;
	Camera* g_ScreenCamera = nullptr;
	std::string m_extraText = "";
	float m_textSize = 5.f;
public:
	virtual void StartUp();
	virtual void Update(float deltaSecond);
	virtual void Render();
	Game(){}
	~Game();
	virtual void HandleKeyPress();

private:
	
	
};