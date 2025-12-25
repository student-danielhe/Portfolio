#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EventSystem.hpp"
//-------------------------------------
class App
{
public: 
	//InputSystem* g_input = nullptr;
	App(){}
	~App(){}
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleKeyPressed(unsigned char keyCode);
	bool HandleKeyReleased(unsigned char keyCode);
	static bool HandleQuitRequested(EventArgs& args);
	void RunMainloop();


private:
	void BeginFrame();
	void Update();
	void Render();
	void EndFrame();
	void HandleKeysPressed();

private:
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;
	bool m_waitingToPause = false;
	

	float m_now = 0.0f;
	float m_lastFrame=0.0f;
};