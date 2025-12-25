#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <vector>
#include "Engine/Core/XmlUtils.hpp"
enum {
	LAYER_BACKGROUND,
	LAYER_CHRACTER,
	LAYER_FOREGROUND,
};
enum class ObjectType {
	PLAYER,
	MONSTER,
	GATE,
	STAIR,
	CHEST,
	SHORTCUT,
	BOSS,
	COUNT
};
class ObjectDefinition {
public:
	SpriteSheet* m_spriteSheet;
	int m_spriteIndex;
	int m_leftIndex;
	int m_rightIndex;
	int m_backIndex;
	int m_layer=LAYER_BACKGROUND;
	ObjectType m_type;

	void Initialize(XmlElement* element);

	static std::vector<ObjectDefinition*> g_objDefinitions;
	static SpriteSheet* g_actor1;
	static SpriteSheet* g_dungeonB;
	static SpriteSheet* g_chest;
	static SpriteSheet* g_monster1;
	static SpriteSheet* g_dungeonA5;

	static void InitializeObjectDefinition();
	static ObjectType ParseTypeFromStr(std::string str);
    static SpriteSheet* ParseFromStr(std::string str);
};