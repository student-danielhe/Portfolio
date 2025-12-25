#pragma once
#include "ChessBoard.hpp"
#include "GameCommon.hpp"
#include "ChessPlayer.hpp"
#include "Game.hpp"

enum class GameState {
	PLAYING,
	WHITE_WON,
	BLACK_WON
};

class ChessMatch {
public:
	
	Game* m_owner;
	ChessBoard* m_board;
	ChessPlayer* m_whitePlayer;
	ChessPlayer* m_blackPlayer;
	bool m_whiteTurn = true;
	GameState m_gameState = GameState::PLAYING;
	bool m_freeCamera = false;
	int m_turns = 1;
	ChessMatch(Game* owner);
	~ChessMatch();
	void Update(float deltaSeconds);
	void GameOver(Faction looser);
	void HandleKeyPressed();
};