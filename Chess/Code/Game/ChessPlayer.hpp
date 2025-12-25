#pragma once
#include<vector>
#include<string>
class ChessPiece;
enum class Faction {
	WHITE,
	BLACK
};
enum class Upgrade {
	PAWN_BANDIT,
	ROOK_UNSTOPPABLE_FORCE,
	KNIGHT_PRINCESSKNIGHT,
	BISHOP_BOUNCY,
	QUEEN_POLYGAMY,
	KING_OF_THE_HILL,
	COUNT
};
class ChessPlayer {
public:
    std::string m_name = "Player1";
    Faction m_faction;
	std::vector<bool> m_upgrades;
	ChessPlayer(Faction faction);
	void Toggle(Upgrade upgrade);
	Upgrade GetUpgradeFromStr(std::string name);
};