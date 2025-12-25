#pragma once
#include"Engine/Math/IntVec2.hpp"
enum TileType {
	TILE_TYPE_GRASS,
	TILE_TYPE_WALL,
	TILE_TYPE_END,
	TILE_TYPE_STONE_WALL,
	TILE_TYPE_STONE_FLOOR,
	NUM_TILETYPE
};
class Tile {
    public:
		IntVec2 m_tileCoords;
		TileType m_tileType=TILE_TYPE_GRASS;
		//explicit Tile(IntVec2 tileCoord, TileType tileType) :m_tileCoords(tileCoord), m_tileType(tileType) {};
};