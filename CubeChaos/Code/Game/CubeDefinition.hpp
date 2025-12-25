#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <vector>
#include "Engine/Core/XmlUtils.hpp"

enum class CubeType {
	STONE,
	DWARF_WARRIOR,
	EARTH,
	LEADER,
	EXCAVATE,
	TINYCASTLE,
	KNIGHT,
	GIANTFLY,
	MAGIC_MISSILE,
	MAGIC_MISSILE_MAGE,
	WATER,
	ROYAL_GUARD,
	SUNFLOWER,
	PLANT_STEM,
	BUNNY,
	RAIN_CLOUD,
	SMALL_WARRIOR_SLIME,
	MEDIUM_WARRIOR_SLIME,
	LARGE_WARRIOR_SLIME,
	COUNT
};

enum class AIPlacement {
    AIWARRIOR,       //random halfway towards enemy DONE
	AIDEFENSE,       //random halfway near ally DONE
	AIBACKLINE,      //As back as possible    DONE
	AIFRONTLINE,     //As forward as possible DONE
	AINORTHEMPTY,    //north must be empty    DONE
	AISTACKING,      //Must near another ally
	AIENEMYSOUTH,    //Must be an enemy south of it DONE
	AIALLYSOUTH,     //Must be an ally south of it N/A
	AIPROJECTILE,    //Must align with an enemy
	AIFLYING,        //can be placed in air DONE
	COUNT
};

class CubeDefinition {
public:
//Basics------------------------------------------------------------
	int m_currentHealth = 10;
	int m_maxHealth = 10;
	int m_manaCost = 10;
	SpriteSheet* m_spriteSheet;
	int m_spriteIndex;
	bool m_blockLight = false;
	std::string m_name = "";

	//Abilities------------------------------------------------
	Vec2 m_melee;
	float m_charging = 0.f;
	bool m_leader = false;
	bool m_burrowed = false;
	bool m_climbing = false;
	bool m_flying = false;
	bool m_liquid = false;
	int m_retaliate = 0;
	int m_projectile = 0;
	int m_regen = 0;
	int m_growth = 0;
	int m_temporary = 0;
	//-----------------------------------------------------------
	std::vector<bool> m_AIPlacement;

	std::string m_text="";

	static SpriteSheet* g_general1; //8x8
	static SpriteSheet* g_general2; //42x42
	static SpriteSheet* g_token1;   //10x10
	static SpriteSheet* g_token2;   //16x16
	static SpriteSheet* g_token3;   //8x8

	void Initialize(XmlElement* element);
	void InitializeVisual(XmlElement const& element);
	void InitializeAbility(XmlElement* element);
	void InitializeAI(XmlElement* element);

	static std::vector<CubeDefinition*> g_cubeDefinitions;

	static void InitializeCubeDefinition();
	static CubeType ParseTypeFromStr(std::string str);
	static AIPlacement ParseAIFromStr(std::string str);
};