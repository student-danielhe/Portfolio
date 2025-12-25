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
	m_image = Image(ParseXmlAttribute(element, "image", "").c_str());
	m_size = ParseXmlAttribute(element, "size", IntVec2(0,0));
	return true;
}

bool MapDefinition::LoadSpawnInfoFromXmlElement(const XmlElement& element) {
	SpawnInfo spawnInfo;
	spawnInfo.m_actorName = ParseXmlAttribute(element, "actor", "N/A");
	spawnInfo.m_position = ParseXmlAttribute(element, "position", IntVec2());
	m_spawnInfos.push_back(spawnInfo);
	return true;
}