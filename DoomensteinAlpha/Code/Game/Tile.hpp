#pragma once
#include "Engine/Math/AABB3.hpp"

enum class TileType {
	UNKNOWN = -1,
	STONEFLOOR,
	WOODFLOOR,
	BRICKWALL,
	WOODWALL,
	OPENGRASS,
	OPENDIRT,
	COUNT,
};
class Tile {
public:
	AABB3 m_bounds;
	TileType m_tileType= TileType::UNKNOWN;
	//explicit Tile(IntVec2 tileCoord, TileType tileType) :m_tileCoords(tileCoord), m_tileType(tileType) {};
};