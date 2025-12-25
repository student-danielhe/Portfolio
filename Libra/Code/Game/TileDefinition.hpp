#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Tile.hpp"
#include "GameCommon.hpp"
#include <vector>
class TileDefinition {
public:
	TileType m_type;
	bool m_IsSolid;
	AABB2 m_SpriteSheetUV;
	Rgba8 m_TintColor;
	static std::vector<TileDefinition> s_definitions;
	static void SetTileDef(TileType tileType, bool isSolid, IntVec2 SpriteCoords, Rgba8 color = Rgba8(255, 255, 255));
	static void InitiateTileType();
};



