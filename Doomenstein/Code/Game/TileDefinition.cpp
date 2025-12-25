#include "TileDefinition.hpp"
#include"Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"
std::vector<TileDefinition*> TileDefinition::s_definitions = std::vector<TileDefinition*>(0);
void TileDefinition::SetTileDef(const XmlElement& element) {
	std::string name = ParseXmlAttribute(element, "name", "UNKOWN");
	m_isSolid = ParseXmlAttribute(element, "isSolid", false);
	m_mapDefColor = ParseXmlAttribute(element, "mapImagePixelColor", Rgba8(255, 0, 255));
	m_wallCoord = ParseXmlAttribute(element, "wallSpriteCoords", IntVec2(-1, -1));
	m_cellingCoord = ParseXmlAttribute(element, "ceilingSpriteCoords", IntVec2(-1, -1));
	m_floorCoord = ParseXmlAttribute(element, "floorSpriteCoords", IntVec2(-1, -1));
	if (name.compare("StoneFloor")==0) {
		m_type = TileType::STONEFLOOR;
	}
	else if (name.compare("WoodFloor")==0) {
		m_type = TileType::WOODFLOOR;
	}
	else if (name.compare("BrickWall")==0) {
		m_type = TileType::BRICKWALL;
	}
	else if (name.compare("WoodWall")==0) {
		m_type = TileType::WOODWALL;
	}
	else if (name.compare("OpenGrass")==0) {
		m_type = TileType::OPENGRASS;
	}
	else if (name.compare("OpenDirt")==0) {
		m_type = TileType::OPENDIRT;
	}
}
void TileDefinition::InitiateTileType(const char* path) {
	XmlDocument doc;
	XmlResult result = doc.LoadFile(path);
	UNUSED(result);
	XmlElement* rootElement = doc.RootElement();
	XmlElement* mapDefinitionElement = rootElement->FirstChildElement();

	while (mapDefinitionElement != nullptr) {
		std::string elementName = mapDefinitionElement->Name();
		TileDefinition* tileDefintion = new TileDefinition();
		tileDefintion->SetTileDef(*mapDefinitionElement);
		TileDefinition::s_definitions.push_back(tileDefintion);
		mapDefinitionElement = mapDefinitionElement->NextSiblingElement();
	}
}

TileDefinition* TileDefinition::GetTileDef(TileType type) {
	for (TileDefinition* def : TileDefinition::s_definitions) {
		if (def->m_type == type) {
			return def;
		}
	}
	return nullptr;
}