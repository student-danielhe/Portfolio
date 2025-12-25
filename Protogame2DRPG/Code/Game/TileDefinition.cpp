#include "TileDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::vector<TileDefinition*> TileDefinition::g_tileDefinitions = std::vector<TileDefinition*>(0);
SpriteSheet* TileDefinition::g_dungeonA4 = nullptr;
SpriteSheet* TileDefinition::g_terrain8X8 = nullptr;
void TileDefinition::Initialize(XmlElement* element)
{
	std::string name = ParseXmlAttribute(*element, "name", "");
	m_type = ParseTypeFromStr(name);
	std::string spriteSheetName = ParseXmlAttribute(*element, "sprite", "");
	m_spriteSheet = ParseFromStr(spriteSheetName);
	m_spriteIndex = ParseXmlAttribute(*element, "spriteIndex", 0);
	m_color = ParseXmlAttribute(*element, "color", Rgba8::BLACK);
	m_solid = ParseXmlAttribute(*element, "solid", false);
}

void TileDefinition::InitializeTileDefinition()
{
	Texture* tileTexture1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/Tilesets/Dungeon_A4.png");
	TileDefinition::g_dungeonA4 = new SpriteSheet(*tileTexture1, IntVec2(16, 16));
	
	Texture* tileTexture2 = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/Tilesets/Terrain_8x8.png");
	TileDefinition::g_terrain8X8 = new SpriteSheet(*tileTexture2, IntVec2(8, 8));
	
	for (int i = 0; i < (int)TileType::COUNT; i++) {
		TileDefinition::g_tileDefinitions.push_back(new TileDefinition);
	}

	XmlDocument tileDoc;
	XmlResult result = tileDoc.LoadFile("Data/Definition/TileDefinition.xml");
	UNUSED(result);
	XmlElement* rootElement = tileDoc.RootElement();
	XmlElement* tileDefinitionElement = rootElement->FirstChildElement();

	while (tileDefinitionElement != nullptr) {
		std::string elementName = ParseXmlAttribute(*tileDefinitionElement, "name", "");
		TileType type = ParseTypeFromStr(elementName);
		g_tileDefinitions[(int)type]->Initialize(tileDefinitionElement);
		tileDefinitionElement = tileDefinitionElement->NextSiblingElement();
	}
}

TileType TileDefinition::ParseTypeFromStr(std::string str)
{
	if (str.compare("wall") == 0)
		return TileType::WALL;

	if (str.compare("floor") == 0)
		return TileType::FLOOR;
	
	if (str.compare("grass") == 0)
		return TileType::GRASS;
	
	if (str.compare("sand") == 0)
		return TileType::SAND;
	//Error case
	return TileType::COUNT;
}

SpriteSheet* TileDefinition::ParseFromStr(std::string str)
{
	if (str.compare("DungeonA4") == 0) {
		return g_dungeonA4;
	}

	if (str.compare("Terrain8X8") == 0) {
		return g_terrain8X8;
	}
	//error
	return g_dungeonA4;
}