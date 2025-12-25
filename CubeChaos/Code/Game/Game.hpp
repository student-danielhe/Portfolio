#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Map.hpp"
//#include "Engine/Input/InputSystem.hpp"
enum class AttractState {
	ATTRACT,
	WIN,
	LOOSE
};
class Game {
public:
	static Map* g_map;

	bool m_attractMode = true;
	
	static Clock* g_gameClock;

	SoundID m_MainSong=0;
	SoundPlaybackID m_MainPlayback=0;
	static SoundID m_CubePlacement;
	static SoundID m_Damage;
	static SoundID m_Heal;
	static SoundID m_Melee;
	static SoundID m_Shoot;
	static SoundID m_Explosion;
	static SoundID m_Tick;
	static SoundID m_Tock;

	AttractState m_state = AttractState::ATTRACT;

	Texture* m_victoryTexture = nullptr;
	Texture* m_looseTexture = nullptr;

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
	void LoadSound();
private:

private:
	bool m_IsDebugging=false;
	SpriteSheet* m_font = nullptr;
};