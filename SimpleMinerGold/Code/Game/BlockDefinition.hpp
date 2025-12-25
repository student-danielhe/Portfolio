#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"
#include <string>
#include "Engine/Core/XmlUtils.hpp"
enum BlockType {
    BLOCK_TYPE_AIR,
    BLOCK_TYPE_WATER,
	BLOCK_TYPE_SAND,    
    BLOCK_TYPE_SNOW,
    BLOCK_TYPE_ICE,
    BLOCK_TYPE_DIRT,
	BLOCK_TYPE_STONE,
	BLOCK_TYPE_COAL,
	BLOCK_TYPE_IRON,
	BLOCK_TYPE_GOLD,
	BLOCK_TYPE_DIAMOND,
    BLOCK_TYPE_OBSIDIAN,
    BLOCK_TYPE_LAVA,
    BLOCK_TYPE_GLOWSTONE,
    BLOCK_TYPE_COBBLESTONE,
    BLOCK_TYPE_CHISELEDBRICK,

    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_YELLOW_GRASS,
    BLOCK_TYPE_LIGHT_GRASS,
    BLOCK_TYPE_DARK_GRASS,

    BLOCK_TYPE_ACACIALOG,
    BLOCK_TYPE_ACACIAPLANKS,
    BLOCK_TYPE_ACACIALEAVES,

    BLOCK_TYPE_CACTUSLOG,

	BLOCK_TYPE_OAKLOG,
    BLOCK_TYPE_OAK_PLANKS,
	BLOCK_TYPE_OAKLEAVES,

    BLOCK_TYPE_BIRCHLOG,
    BLOCK_TYPE_BIRCHPLANKS,
    BLOCK_TYPE_BIRCHLEAVES,

    BLOCK_TYPE_JUNGLELOG,
    BLOCK_TYPE_JUNKGLE_PLANKS,
    BLOCK_TYPE_JUNGLELEAVES,

    BLOCK_TYPE_SPRUCELOG,
    BLOCK_TYPE_SPRUCEPLANKS,
    BLOCK_TYPE_SPRUCELEAVES,
    BLOCK_TYPE_SPRUCELEAVESSNOW,
    
    BLOCK_TYPE_COUNT
};
class BlockDefinition {
    public:
    uint8_t m_type=BLOCK_TYPE_COUNT;
    bool m_isVisible=true;
    bool m_isSolid = false;
    bool m_isOpaque = false;
    Vec2 m_topSpriteCoord;
    Vec2 m_bottomSpriteCoord;
    Vec2 m_sideSpriteCoord;

    int m_meltingPoint = 10000;
    uint8_t m_meltTo = m_type;

    int m_freezingPoint = -10000;
    uint8_t m_freezeTo = m_type;

    bool m_liquid = false;
    bool m_powder = false;

    static std::vector<BlockDefinition*> g_blockDefinitions;

    static void InitializeBlockDefinitions();
    static BlockType ParseFromString(std::string str);

    void Initialize(XmlElement* element);
};