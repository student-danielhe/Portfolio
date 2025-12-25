#pragma once
#include "TileDefinition.hpp"

struct Tile {
	IntVec2 m_pos;
	TileType m_type;
	bool m_blockPath = false;
};