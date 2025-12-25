#include "MapDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
std::vector<MapDefinition*> MapDefinition::s_definitions = std::vector<MapDefinition*>(0);
void MapDefinition::InitializeDefinition(const char* path) {
	XmlDocument doc;
	XmlResult result = doc.LoadFile(path);
	UNUSED(result);
	XmlElement* rootElement = doc.RootElement();
	XmlElement* mapDefinitionElement = rootElement->FirstChildElement();

	while (mapDefinitionElement != nullptr) {
		std::string elementName = mapDefinitionElement->Name();
		MapDefinition* mapDefintion = new MapDefinition();
		mapDefintion->LoadFromXmlElement(*mapDefinitionElement);
		XmlElement* spawnInfoElement = mapDefinitionElement->FirstChildElement("SpawnInfos");
		spawnInfoElement = spawnInfoElement->FirstChildElement("SpawnInfo");
		while (spawnInfoElement != nullptr) {
			mapDefintion->LoadSpawnInfoFromXmlElement(*spawnInfoElement);
			spawnInfoElement = spawnInfoElement->NextSiblingElement();
		}
		MapDefinition::s_definitions.push_back(mapDefintion);

		mapDefinitionElement = mapDefinitionElement->NextSiblingElement();
	}
}
bool MapDefinition::LoadFromXmlElement(const XmlElement& element) {
	m_name = ParseXmlAttribute(element, "name", m_name);
	m_spriteSheetCellCount = ParseXmlAttribute(element, "spriteSheetCellCount", IntVec2(0, 0));
	m_image = Image(ParseXmlAttribute(element, "image", "").c_str());
	std::string filename = ParseXmlAttribute(element, "shader", "").c_str();
	filename.append(".hlsl");
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	m_shader= g_theRenderer->CreateShader("Diffuse", buffer.str().c_str(),VertexType::VERTEX_PCUTBN);
	m_spriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile(ParseXmlAttribute(element, "spriteSheetTexture", "").c_str());
	m_spiteSheet = new SpriteSheet(*m_spriteSheetTexture, m_spriteSheetCellCount);
	return true;
}

bool MapDefinition::LoadSpawnInfoFromXmlElement(const XmlElement& element) {
	SpawnInfo spawnInfo;
	spawnInfo.m_actorName = ParseXmlAttribute(element, "actor", "N/A");
	spawnInfo.m_orientation = ParseXmlAttribute(element, "orientation", EulerAngles());
	spawnInfo.m_position = ParseXmlAttribute(element, "position", Vec3());
	m_spawnInfos.push_back(spawnInfo);
	return true;
}