#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Player.hpp"
#include "Prop.hpp"
#include "Entity.hpp"
#include "Engine/Core/Clock.hpp"
//#include "Engine/Input/InputSystem.hpp"
class Game {
public:
	
	RandomNumberGenerator* g_RNG = nullptr;
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;
	Player* m_player = nullptr;
	
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
	void loadSound();
private:
	//helper
	void RenderCube();
	Prop MakeCube(float xLen, float yLen, float zLen, Rgba8 color=Rgba8(1,2,3));
	void MakeFrames();
	void MakeSphere();
	void MakeSphere2();
	void UpdateCubes(float deltaSeconds);
private:
	bool m_IsDebugging=false;
	Prop* m_cubeDepth = nullptr;
	Prop* m_cubeRotate = nullptr;
	std::vector<Prop> m_frame;
	Prop* m_sphere;
	Prop* m_sphere2;
	bool m_darkening = true;
	float m_cube2Color = 255;

	SpriteSheet* m_font = nullptr;
};