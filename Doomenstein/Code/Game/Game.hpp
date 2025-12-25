#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Player.hpp"
#include "Engine/Core/Clock.hpp"
#include "Map.hpp"
#include "Engine/Core/Timer.hpp"
//#include "Engine/Input/InputSystem.hpp"
enum class GameState {
	NONE=-1,
	ATTRACT,
	PLAYING,
	GAMEOVER,
	VICTORY,
	COUNT
};
class Game {
public:

	std::string m_defaultMapName = "TestMap";
	Map* m_currentLoadedMap = nullptr;

	GameState m_currentState = GameState::ATTRACT;
	GameState m_nextState = GameState::PLAYING;
	SoundPlaybackID m_menu = 0;
	SoundPlaybackID m_gameOver = 0;
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void BeginFrame();
	void EndFrame();
	Game(){}
	~Game();
	void RenderAttractMode();
	void RenderGameOver();
	void RenderVictory();
	void EnterState(GameState state);
	void ExitState(GameState state);
	void HandleKeyPress();


	MapDefinition* FindDefinition();

private:

private:
	bool m_IsDebugging=false;
	SpriteSheet* m_font = nullptr;
	bool m_player1 = false;
	bool m_gold = true;

};