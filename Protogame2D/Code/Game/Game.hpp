#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
//#include "Engine/Input/InputSystem.hpp"
class Game {
public:
	
	RandomNumberGenerator* g_RNG = nullptr;
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;
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
	void EnterAttractMode();
	void ExitAttractMode();
	void HandleKeyPress();
	void loadSound();
private:
	bool m_IsDebugging=false;
	
};