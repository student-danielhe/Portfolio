#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Tile.hpp"
#include "GameCommon.hpp"
#include <vector>
#include "Engine/Core/XmlUtils.hpp"
class TileDefinition {
public:
	TileType m_type=TileType::UNKNOWN;
	bool m_isSolid=false;
	IntVec2 m_wallCoord;
	IntVec2 m_floorCoord;
	IntVec2 m_cellingCoord;
	Rgba8 m_mapDefColor;
	static std::vector<TileDefinition*> s_definitions;
	void SetTileDef(const XmlElement& element);
	static void InitiateTileType(const char* path);
	static TileDefinition* GetTileDef(TileType type);
};
