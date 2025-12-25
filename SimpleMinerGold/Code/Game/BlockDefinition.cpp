#include "BlockDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::vector<BlockDefinition*> BlockDefinition::g_blockDefinitions = std::vector<BlockDefinition*>(0);

void BlockDefinition::InitializeBlockDefinitions()
{
	for (int i = 0; i < BLOCK_TYPE_COUNT; i++) {
		BlockDefinition::g_blockDefinitions.push_back(new BlockDefinition);
	}

	XmlDocument tileDoc;
	XmlResult result = tileDoc.LoadFile("Data/Definitions/BlockSpriteSheet_BlockDefinitions.xml");
	UNUSED(result);
	XmlElement* rootElement = tileDoc.RootElement();
	XmlElement* blockDefinitionElement = rootElement->FirstChildElement();

	while (blockDefinitionElement != nullptr) {
		std::string elementName = ParseXmlAttribute(*blockDefinitionElement, "name", "");
		BlockType type = ParseFromString(elementName);

		g_blockDefinitions[type]->m_type = (uint8_t)type;
		g_blockDefinitions[type]->Initialize(blockDefinitionElement);

		blockDefinitionElement = blockDefinitionElement->NextSiblingElement();
	}
}

BlockType BlockDefinition::ParseFromString(std::string str)
{
    if (str.compare("Air") == 0) 
        return BLOCK_TYPE_AIR;
    
    if (str.compare("Grass") == 0) 
        return BLOCK_TYPE_GRASS;

	if (str.compare("Dirt") == 0)
		return BLOCK_TYPE_DIRT;

	if (str.compare("Stone") == 0)
		return BLOCK_TYPE_STONE;

	if (str.compare("Coal") == 0)
		return BLOCK_TYPE_COAL;

	if (str.compare("Iron") == 0)
		return BLOCK_TYPE_IRON;

	if (str.compare("Gold") == 0)
		return BLOCK_TYPE_GOLD;

	if (str.compare("Diamond") == 0)
		return BLOCK_TYPE_DIAMOND;

	if (str.compare("Water") == 0)
		return BLOCK_TYPE_WATER;

	if (str.compare("Glowstone") == 0)
		return BLOCK_TYPE_GLOWSTONE;

	if (str.compare("Cobblestone") == 0)
		return BLOCK_TYPE_COBBLESTONE;

	if (str.compare("ChiseledBrick") == 0)
		return BLOCK_TYPE_CHISELEDBRICK;

	if (str.compare("Sand") == 0)
		return BLOCK_TYPE_SAND;

	if (str.compare("Snow") == 0)
		return BLOCK_TYPE_SNOW;

	if (str.compare("Ice") == 0)
		return BLOCK_TYPE_ICE;

	if (str.compare("AcaciaLog") == 0)
		return BLOCK_TYPE_ACACIALOG;

	if (str.compare("AcaciaLeaves") == 0)
		return BLOCK_TYPE_ACACIALEAVES;

	if (str.compare("BirchLog") == 0)
		return BLOCK_TYPE_BIRCHLOG;

	if (str.compare("BirchLeaves") == 0)
		return BLOCK_TYPE_BIRCHLEAVES;

	if (str.compare("JungleLog") == 0)
		return BLOCK_TYPE_JUNGLELOG;

	if (str.compare("JungleLeaves") == 0)
		return BLOCK_TYPE_JUNGLELEAVES;

	if (str.compare("OakLog") == 0)
		return BLOCK_TYPE_OAKLOG;

	if (str.compare("OakLeaves") == 0)
		return BLOCK_TYPE_OAKLEAVES;

	if (str.compare("SpruceLog") == 0)
		return BLOCK_TYPE_SPRUCELOG;

	if (str.compare("SpruceLeaves") == 0)
		return BLOCK_TYPE_SPRUCELEAVES;
	
	if(str.compare("Obsidian") == 0)
	    return BLOCK_TYPE_OBSIDIAN;

	if(str.compare("Lava") == 0)
	    return BLOCK_TYPE_LAVA;

	if (str.compare("AcaciaPlanks") == 0)
		return BLOCK_TYPE_ACACIAPLANKS;

	if (str.compare("CactusLog") == 0)
		return BLOCK_TYPE_CACTUSLOG;

	if (str.compare("OakPlanks") == 0)
		return BLOCK_TYPE_OAK_PLANKS;

	if (str.compare("BirchPlanks") == 0)
		return BLOCK_TYPE_BIRCHPLANKS;

	if (str.compare("JunglePlanks") == 0)
		return BLOCK_TYPE_JUNKGLE_PLANKS; 
		
	if (str.compare("SprucePlanks") == 0)
		return BLOCK_TYPE_SPRUCEPLANKS;

	if (str.compare("SpruceLeavesSnow") == 0)
		return BLOCK_TYPE_SPRUCELEAVESSNOW;

	if (str.compare("GrassLight") == 0)
		return BLOCK_TYPE_LIGHT_GRASS;

	if (str.compare("GrassYellow") == 0)
		return BLOCK_TYPE_YELLOW_GRASS;

	if (str.compare("GrassDark") == 0)
		return BLOCK_TYPE_DARK_GRASS;

	return BLOCK_TYPE_COUNT;
}

void BlockDefinition::Initialize(XmlElement* element)
{
    m_isVisible = ParseXmlAttribute(*element, "isVisible", true);
	m_isSolid = ParseXmlAttribute(*element, "isSolid", false);
	m_isOpaque = ParseXmlAttribute(*element, "isOpaque", false);
	
	Vec2 orig = Vec2(0,0);
	m_topSpriteCoord = ParseXmlAttribute(*element, "topSpriteCoords", orig);
	m_bottomSpriteCoord = ParseXmlAttribute(*element, "bottomSpriteCoords", orig);
	m_sideSpriteCoord = ParseXmlAttribute(*element, "sideSpriteCoords", orig);

	m_freezingPoint = ParseXmlAttribute(*element, "freezeAt", -10000);
	m_meltingPoint = ParseXmlAttribute(*element, "meltAt", 10000);
	m_freezeTo = (uint8_t)ParseFromString(ParseXmlAttribute(*element, "freezeTo", "Air"));
	m_meltTo = (uint8_t)ParseFromString(ParseXmlAttribute(*element, "meltTo", "Air"));

	if (m_type == BLOCK_TYPE_WATER||m_type == BLOCK_TYPE_LAVA) {
		m_liquid = true;
	}

	if (m_type == BLOCK_TYPE_SNOW || m_type == BLOCK_TYPE_SAND) {
		m_powder = true;
	}
}
