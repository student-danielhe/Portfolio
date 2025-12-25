#include "CubeDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
std::vector<CubeDefinition*> CubeDefinition::g_cubeDefinitions = std::vector<CubeDefinition*>(0);

SpriteSheet* CubeDefinition::g_general1 = nullptr; //8x8
SpriteSheet* CubeDefinition::g_general2 = nullptr; //42x42
SpriteSheet* CubeDefinition::g_token1   = nullptr;   //10x10
SpriteSheet* CubeDefinition::g_token2   = nullptr;   //16x16
SpriteSheet* CubeDefinition::g_token3   = nullptr;   //8x8

void CubeDefinition::Initialize(XmlElement* element)
{
    m_name = ParseXmlAttribute(*element, "name", "");
	m_manaCost = ParseXmlAttribute(*element, "mana", 0);
	m_currentHealth = ParseXmlAttribute(*element, "hp", 0);
	m_maxHealth = ParseXmlAttribute(*element, "maxhp", 0);

	XmlElement* VisualElement = element->FirstChildElement("Visual");

	if (VisualElement) {
		InitializeVisual(*VisualElement);
	}

	XmlElement* AbilityElement = element->FirstChildElement("Abilities");

	if (AbilityElement) {
	    InitializeAbility(AbilityElement);
	}

	XmlElement* AIElement = element->FirstChildElement("AI");
	
	m_AIPlacement.resize((int)AIPlacement::COUNT);
	for (bool ai : m_AIPlacement) {
		ai=false;
	}
	
	if (AIElement) {
	    InitializeAI(AIElement);
	}

	XmlElement* TextElement = element->FirstChildElement("Text");
	if (TextElement) {
	    m_text = ParseXmlAttribute(*TextElement, "text", "");
	}
}

void CubeDefinition::InitializeVisual(XmlElement const& element)
{
    std::string spriteSheetName = "";
	spriteSheetName = ParseXmlAttribute(element, "sprite", "error");
	if (spriteSheetName.compare("General1")==0) {
		m_spriteSheet = CubeDefinition::g_general1;
	}
	else if (spriteSheetName.compare("General2") == 0) {
		m_spriteSheet = CubeDefinition::g_general2;
	}
	else if (spriteSheetName.compare("Token1") == 0) {
		m_spriteSheet = CubeDefinition::g_token1;
	}
	else if (spriteSheetName.compare("Token2") == 0) {
		m_spriteSheet = CubeDefinition::g_token2;
	}
	else if (spriteSheetName.compare("Token3") == 0) {
		m_spriteSheet = CubeDefinition::g_token3;
	}
	else {
	//Error
		m_spriteSheet = CubeDefinition::g_token3;
		m_spriteIndex = 63;
		return;
	}

	m_spriteIndex = ParseXmlAttribute(element, "spriteIndex", 0);
	m_blockLight = ParseXmlAttribute(element, "blockLight", false);
}

void CubeDefinition::InitializeAbility(XmlElement* element)
{
    m_melee = ParseXmlAttribute(*element, "melee", Vec2(0.f, 0.f));
	m_charging = ParseXmlAttribute(*element, "charging", 0.f);
	m_burrowed = ParseXmlAttribute(*element, "burrowed", false);
	m_leader = ParseXmlAttribute(*element, "leader", false);

	m_climbing = ParseXmlAttribute(*element, "climbing", false);
	m_flying = ParseXmlAttribute(*element, "flying", false);
	m_liquid = ParseXmlAttribute(*element, "liquid", false);
	m_retaliate = ParseXmlAttribute(*element, "retaliate", 0);
	m_projectile = ParseXmlAttribute(*element, "projectile", 0);
	m_regen = ParseXmlAttribute(*element, "regen", 0);
	m_growth = ParseXmlAttribute(*element, "growth", 0);
	m_temporary = ParseXmlAttribute(*element, "temporary", 0);
}

void CubeDefinition::InitializeAI(XmlElement* element)
{
    std::string AIString ="";
	AIString = ParseXmlAttribute(*element, "AiPlacement", "");
	std::stringstream commandArg(AIString);
	std::string temp;
	while (std::getline(commandArg, temp, '&')) {
		m_AIPlacement[(int)ParseAIFromStr(temp)] = true;
	}
}

void CubeDefinition::InitializeCubeDefinition()
{
    Texture* generalTexture1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Sprites/GeneralCubes.c.png");
	Texture* generalTexture2 = g_theRenderer->CreateOrGetTextureFromFile("Data/Sprites/GeneralCubes2.c.png");
	Texture* tokenTexture1   = g_theRenderer->CreateOrGetTextureFromFile("Data/Sprites/TokenCubes.c.png");
	Texture* tokenTexture2   = g_theRenderer->CreateOrGetTextureFromFile("Data/Sprites/TokenCubes2.c.png");
	Texture* tokenTexture3   = g_theRenderer->CreateOrGetTextureFromFile("Data/Sprites/TokenCubes3.c.png");

	CubeDefinition::g_general1 = new SpriteSheet(*generalTexture1, IntVec2(8,8)); //8x8
	CubeDefinition::g_general2 = new SpriteSheet(*generalTexture2, IntVec2(42,42)); //42x42
	CubeDefinition::g_token1 = new SpriteSheet(*tokenTexture1, IntVec2(10,10));   //10x10
	CubeDefinition::g_token2 = new SpriteSheet(*tokenTexture2, IntVec2(16,16));   //16x16
	CubeDefinition::g_token3 = new SpriteSheet(*tokenTexture3, IntVec2(8,8));   //8x8
	
	

	for (int i = 0; i < (int)CubeType::COUNT; i++) {
		CubeDefinition::g_cubeDefinitions.push_back(new CubeDefinition);
	}

	XmlDocument cubeDoc;
	XmlResult result = cubeDoc.LoadFile("Data/Definitions/CubeDefinition.xml");
	UNUSED(result);
	XmlElement* rootElement = cubeDoc.RootElement();
	XmlElement* cubeDefinitionElement = rootElement->FirstChildElement();
	
	while (cubeDefinitionElement != nullptr) {
		std::string elementName = ParseXmlAttribute(*cubeDefinitionElement,"name", "");
		CubeType type = ParseTypeFromStr(elementName);
		g_cubeDefinitions[(int)type]->Initialize(cubeDefinitionElement);
		cubeDefinitionElement = cubeDefinitionElement->NextSiblingElement();
	}
}

CubeType CubeDefinition::ParseTypeFromStr(std::string str)
{
    if (str.compare("Stone") == 0) 
        return CubeType::STONE;

	if (str.compare("Dwarf Warrior") == 0)
		return CubeType::DWARF_WARRIOR;

	if (str.compare("Earth") == 0)
		return CubeType::EARTH;

	if (str.compare("Leader") == 0)
		return CubeType::LEADER;

	if (str.compare("Excavate") == 0)
		return CubeType::EXCAVATE;

	if (str.compare("Small Warrior Slime") == 0)
		return CubeType::SMALL_WARRIOR_SLIME;

	if (str.compare("Medium Warrior Slime") == 0)
		return CubeType::MEDIUM_WARRIOR_SLIME;

	if (str.compare("Large Warrior Slime") == 0)
		return CubeType::LARGE_WARRIOR_SLIME;

	if (str.compare("Bunny") == 0)
		return CubeType::BUNNY;

	if (str.compare("Tiny Castle") == 0)
		return CubeType::TINYCASTLE;

	if (str.compare("Knight") == 0)
		return CubeType::KNIGHT;

	if (str.compare("Water") == 0)
		return CubeType::WATER;

	if (str.compare("Rain Cloud") == 0)
		return CubeType::RAIN_CLOUD;

	if (str.compare("Magic Missile") == 0)
		return CubeType::MAGIC_MISSILE;

	if (str.compare("Magic Missile Mage") == 0)
		return CubeType::MAGIC_MISSILE_MAGE;

	if (str.compare("Plant Stem") == 0)
		return CubeType::PLANT_STEM;

	if (str.compare("Sunflower") == 0)
		return CubeType::SUNFLOWER;

	if (str.compare("Giant Fly") == 0)
		return CubeType::GIANTFLY;

	if (str.compare("Royal Guard") == 0)
		return CubeType::ROYAL_GUARD;

    return CubeType::COUNT;
}

AIPlacement CubeDefinition::ParseAIFromStr(std::string str)
{

	if (str.compare("AiWarrior") == 0) 
		return AIPlacement::AIWARRIOR;
    
	if (str.compare("AiDefense") == 0)
		return AIPlacement::AIDEFENSE;

	if (str.compare("AiBackline") == 0)
		return AIPlacement::AIBACKLINE;

	if (str.compare("AiFrontline") == 0)
		return AIPlacement::AIFRONTLINE;

	if (str.compare("AiNorthEmpty") == 0)
		return AIPlacement::AINORTHEMPTY;
	
	if (str.compare("AiStacking") == 0)
		return AIPlacement::AISTACKING;

	if (str.compare("AiEnemySouth") == 0)
		return AIPlacement::AIENEMYSOUTH;

	if (str.compare("AiProjectile") == 0)
		return AIPlacement::AIPROJECTILE;

	if (str.compare("AiFlying") == 0)
		return AIPlacement::AIFLYING;
    return AIPlacement::COUNT;
}
