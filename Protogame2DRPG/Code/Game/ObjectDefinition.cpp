#include "ObjectDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <vector>
#include "Engine/Core/XmlUtils.hpp"

std::vector<ObjectDefinition*> ObjectDefinition::g_objDefinitions = std::vector<ObjectDefinition*>(0);
SpriteSheet* ObjectDefinition::g_actor1 = nullptr;
SpriteSheet* ObjectDefinition::g_monster1 = nullptr;
SpriteSheet* ObjectDefinition::g_dungeonB = nullptr;
SpriteSheet* ObjectDefinition::g_chest = nullptr;
SpriteSheet* ObjectDefinition::g_dungeonA5 = nullptr;

void ObjectDefinition::Initialize(XmlElement* element)
{
	std::string spriteSheetName = ParseXmlAttribute(*element, "sprite", "");
	m_spriteSheet=ParseFromStr(spriteSheetName);
	m_spriteIndex = ParseXmlAttribute(*element, "spriteIndex", 0);
	m_leftIndex = ParseXmlAttribute(*element, "leftIndex", m_spriteIndex);
	m_rightIndex = ParseXmlAttribute(*element, "rightIndex", m_spriteIndex);
	m_backIndex = ParseXmlAttribute(*element, "backIndex", m_spriteIndex);
	m_layer = ParseXmlAttribute(*element, "layer", 1);
}

void ObjectDefinition::InitializeObjectDefinition()
{
	Texture* actorTexture1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/Characters/Actor1.png");
	ObjectDefinition::g_actor1 = new SpriteSheet(*actorTexture1, IntVec2(12, 8)); 

	Texture* chestTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/Characters/!Chest.png");
	ObjectDefinition::g_chest = new SpriteSheet(*chestTexture, IntVec2(12, 8));

	Texture* stairTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/Tilesets/Dungeon_B.png");
	ObjectDefinition::g_dungeonB = new SpriteSheet(*stairTexture, IntVec2(16, 16));

	Texture* monsterTexture1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/Characters/Monster1.png");
	ObjectDefinition::g_monster1 = new SpriteSheet(*monsterTexture1, IntVec2(12, 8));

	Texture* shortCutTexture1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Graphics/TileSets/Dungeon_A5.png");
	ObjectDefinition::g_dungeonA5 = new SpriteSheet(*shortCutTexture1, IntVec2(8, 16));

	for (int i = 0; i < (int)ObjectType::COUNT; i++) {
		ObjectDefinition::g_objDefinitions.push_back(new ObjectDefinition);
	}

	XmlDocument objDoc;
	XmlResult result = objDoc.LoadFile("Data/Definition/ObjectDefinition.xml");
	UNUSED(result);
	XmlElement* rootElement = objDoc.RootElement();
	XmlElement* objDefinitionElement = rootElement->FirstChildElement();

	while (objDefinitionElement != nullptr) {
		std::string elementName = ParseXmlAttribute(*objDefinitionElement, "name", "");

		ObjectType type = ParseTypeFromStr(elementName);
		
		g_objDefinitions[(int)type]->m_type = type;
		g_objDefinitions[(int)type]->Initialize(objDefinitionElement);
		
		objDefinitionElement = objDefinitionElement->NextSiblingElement();
	}
}

ObjectType ObjectDefinition::ParseTypeFromStr(std::string str)
{
    if(str.compare("player")==0)
	    return ObjectType::PLAYER;
	
	if (str.compare("monster") == 0)
	    return ObjectType::MONSTER;
	
	if (str.compare("gate") == 0)
	    return ObjectType::GATE;
	
	if (str.compare("stair") == 0)
	    return ObjectType::STAIR;
	
	if (str.compare("chest") == 0)
	    return ObjectType::CHEST;

	if (str.compare("shortcut") == 0)
		return ObjectType::SHORTCUT;

	if(str.compare("boss")==0)
	    return ObjectType::BOSS;
    //Error case
	return ObjectType::COUNT;
}

SpriteSheet* ObjectDefinition::ParseFromStr(std::string str)
{
	if (str.compare("Actor1") == 0) {
		return g_actor1;
	}
	
	if (str.compare("Monster1") == 0) {
		return g_monster1;
	}

	if (str.compare("Dungeon_B") == 0) {
		return g_dungeonB;
	}

	if (str.compare("Chest") == 0) {
		return g_chest;
	}

	if (str.compare("Dungeon_A5") == 0) {
		return g_dungeonA5;
	}
	//error
	return g_actor1;
}
