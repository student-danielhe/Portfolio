#include "TileDefinition.hpp"
#include"Engine/Renderer/SpriteSheet.hpp"
std::vector<TileDefinition> TileDefinition::s_definitions = std::vector<TileDefinition>(NUM_TILETYPE);
void TileDefinition::SetTileDef(TileType tileType, bool isSolid, IntVec2 SpriteCoords, Rgba8 color) {
	TileDefinition::s_definitions[tileType].m_IsSolid = isSolid;
	TileDefinition::s_definitions[tileType].m_TintColor = color;
	TileDefinition::s_definitions[tileType].m_type = tileType;
	int spriteIndex = SpriteCoords.x + SpriteCoords.y * g_gameConfigBlackboard.GetValue("TERRAINSHEET_LAYOUT_X", 8);
	TileDefinition::s_definitions[tileType].m_SpriteSheetUV = g_terrainSheet->GetSpriteUVs(spriteIndex);
	
}
void TileDefinition::InitiateTileType() {
	TileDefinition::s_definitions.resize(NUM_TILETYPE);
	IntVec2 grassUV(0, 0);
	SetTileDef(TILE_TYPE_GRASS, false, grassUV);
	IntVec2 wallUV(1, 4);
	SetTileDef(TILE_TYPE_WALL, true, wallUV);
	IntVec2 endUV(1, 7);
	SetTileDef(TILE_TYPE_END, false, endUV);
	IntVec2 StoneWallUV(5,6);
	SetTileDef(TILE_TYPE_STONE_WALL, true, StoneWallUV);
	IntVec2 StoneFloowUV(3,6);
	SetTileDef(TILE_TYPE_STONE_FLOOR, false, StoneFloowUV);
}