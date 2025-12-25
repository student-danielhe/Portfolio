#pragma once
#include "GameCommon.hpp"
#include <vector>
#include "ChessPiece.hpp"
#include "Game.hpp"
#include "Prop.hpp"
#include "Engine/Core/DevConsole.hpp"
enum class GameState {
    PLAYING,
    WHITE_WON,
    BLACK_WON
};

class Board {
public:
    Faction m_currentTurn = Faction::WHITE;
    GameState m_gameState = GameState::PLAYING;
    std::vector<ChessPiece*> m_whitePieces;
    std::vector<ChessPiece*> m_blackPieces;
    Game* m_owner = nullptr;
    std::vector<Prop*> m_cells;
    void GameOver(Faction looser);
    void Render();
    void Update();
    bool TryMovePiece(Position piecePos, Position destination);
    Board(Game* owner);
    void InitializePieces();
    void InitializeProp();
    void DeleteCapturedPieces();
    std::string PrintBoardState();
    static bool ChessMove(EventArgs& args);
};