#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <vector>
#include "Engine/Core/XmlUtils.hpp"

enum class TileType {
	WALL,
	FLOOR,
	GRASS,
	SAND,
	COUNT
};

class TileDefinition {
public:
	SpriteSheet* m_spriteSheet;
	int m_spriteIndex;
	bool m_solid = false;
	TileType m_type = TileType::WALL;
	Rgba8 m_color=Rgba8::BLACK;

	void Initialize(XmlElement* element);

	static std::vector<TileDefinition*> g_tileDefinitions;
	static SpriteSheet* g_dungeonA4; 
	static SpriteSheet* g_terrain8X8;
	static void InitializeTileDefinition();

	static TileType ParseTypeFromStr(std::string str);
	static SpriteSheet* ParseFromStr(std::string str);
};