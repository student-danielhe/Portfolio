#pragma once
#include "ChessPlayer.hpp"
#include "ChessPieceDefinition.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"
enum class Position {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	COUNT
};




class ChessBoard;
class ChessPiece {
public:
    
    Position m_position;
	Position m_prevPosition;
	ChessPieceDefinition m_def;
	bool m_captured = false;
	ChessBoard* m_board = nullptr;
	Faction m_faction = Faction::WHITE; 
	EulerAngles m_orientation;
	ChessPlayer* m_player=nullptr;
	Rgba8 m_color;

	int m_lastMovedTurn=0;
	int m_totalMove=0;
	float m_lastMoveSecond=0.f;

	bool m_bishopBouncing = false;
	Position m_bishopBouncePosition;

public:

	ChessPiece(Position pos, ChessPieceDefinition def, Faction faction, ChessBoard* board);
	void Render();
	bool Move(Position pos, Type promoteTo = Type::COUNT, bool teleport = false);
	bool CheckValidMove(Position pos);
	bool CheckPawnValidMove(IntVec2 from, IntVec2 to);
	bool CheckRookValidMove(IntVec2 from, IntVec2 to);
	bool CheckKnightValidMove(IntVec2 from, IntVec2 to);
	bool CheckBishopValidMove(IntVec2 from, IntVec2 to);
	bool CheckQueenValidMove(IntVec2 from, IntVec2 to);
	bool CheckKingValidMove(IntVec2 from, IntVec2 to);
	void Captured();

	static Position ParsePositionFromText(std::string str);
	static IntVec2 GetIntVec2FromPosition(Position pos);
	static Position GetPositionFromIntVec2(IntVec2 pos);

	Mat44 GetModelToWorld();
	Vec3 CalculatePosition();

	RaycastResult3D RayCastVSMe(Vec3 startPos, Vec3 fwdNormal, float maxDist);
};