#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
//#include "Engine/Input/InputSystem.hpp"
class PlayerShip;
class Bullet;
class Asteroid;
class Beetle;
class Wasp;
class Debris;
class Game {
public:
	
	PlayerShip* g_player = nullptr;
	RandomNumberGenerator* g_RNG = nullptr;
	Bullet** m_bulletList =nullptr;
	Asteroid** m_asteroidList = nullptr;
	Beetle** m_beetleList = nullptr;
	Wasp** m_waspList = nullptr;
	Debris** m_debrisList = nullptr;
	int m_wave = 1;
	int m_live = 0;
	bool m_attractMode = true;
	SoundID m_BGM;
	SoundPlaybackID m_BGMPlayBack;
	SoundID m_Victory;
	SoundPlaybackID m_VictoryPlayBack;
	SoundID m_Defeat;
	SoundPlaybackID m_DefeatPlayBack;
	SoundID m_Death;
	SoundID m_EnemyDeath;
	SoundID m_WaveStart;
	SoundID m_Respawn;
	SoundID m_Start;
	SoundID m_Quit;
	SoundID m_Shoot;
	SoundID m_Hurt1;
	SoundID m_Hurt2;
	SoundID m_Hurt3;
	float m_volume=1.0f;
	float m_storedVolume = 1.0f;
	bool m_mute = false;
	float m_trauma = 0;
	Camera* g_WorldCamera = nullptr;
	Camera* g_ScreenCamera = nullptr;
	int m_beetleRemain = 0;
	int m_waspRemain = 0;
	float m_attractScale = 1;
	bool m_shrink = true;
	float m_asteroidSpawnTimer = 300;
	float m_asteroidTimerRemain = 300;
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void BeginFrame(){}
	void EndFrame(){}
	Game(){}
	void SpawnBullet(Vec2 position, float directionDegree);
	void SpawnAsteroid();
	void SpawnBeetle();
	void SpawnWasp();
	void SpawnDebris(Vec2 position, Vec2 initialVerlocity,Rgba8 parentColor, int num);
	void UpdateEntity(float deltaSecond);
	void RemoveEntity();
	void CheckCollision();
	~Game();
	void DebugRender();
	Vec2 EnemySpawnLocation(float radius);
	void Cheat();
	void PlayerDeath();
	void RenderLives();
	bool WaveClear();
	void SpawnWave();
	void RenderAttractMode();
	bool IsOver();
	void EnterAttractMode();
	void ExitAttractMode();
	void HandleKeyPress();
	void RenderWorld();
	void RenderUI();
	void ApplyScreenShake();
	void Respawn();
	void AdjustSound();
	void renderHPBar(Vec2 const& position, float scale, float maxHP, float HP);
	void loadSound();
	void trySpawnEnemy();
	void PeriodicalAsteroid(float deltaseond);
private:
	bool m_IsDebugging=false;
	
};