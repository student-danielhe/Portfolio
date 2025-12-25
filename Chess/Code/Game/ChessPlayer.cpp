#include "ChessPlayer.hpp"
ChessPlayer::ChessPlayer(Faction faction) : m_faction(faction)
{
    m_upgrades.resize((int)Upgrade::COUNT);
}

void ChessPlayer::Toggle(Upgrade upgrade)
{
    m_upgrades[(int)upgrade]=!m_upgrades[(int)upgrade];
}

Upgrade ChessPlayer::GetUpgradeFromStr(std::string name)
{
    if (name.compare("PAWN") == 0) {
	    return Upgrade::PAWN_BANDIT;
    }
	if (name.compare("ROOK") == 0) {
	    return Upgrade::ROOK_UNSTOPPABLE_FORCE;
	}
	if (name.compare("KNIGHT") == 0) {
	    return Upgrade::KNIGHT_PRINCESSKNIGHT;
	}
	if (name.compare("BISHOP") == 0) {
	    return Upgrade::BISHOP_BOUNCY;
	}
	if (name.compare("QUEEN") == 0) {
	    return Upgrade::QUEEN_POLYGAMY;
	}
	if (name.compare("KING") == 0) {
	    return Upgrade::KING_OF_THE_HILL;
	}
	return Upgrade::COUNT;
}

