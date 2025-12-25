#pragma once
#include "Engine/Core/Image.hpp"
#include <string.h>
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer//SpriteSheet.hpp"
struct SpawnInfo {
	std::string m_actorName;
	Vec3 m_position;
	EulerAngles m_orientation;
	Vec3 m_velocity;
};
class MapDefinition {
public:
	std::string m_name;
	Image m_image;
	Shader* m_shader = nullptr;
	Texture* m_spriteSheetTexture = nullptr;
	IntVec2 m_spriteSheetCellCount;
	SpriteSheet* m_spiteSheet = nullptr;
	std::vector<SpawnInfo> m_spawnInfos;
	static std::vector<MapDefinition*> s_definitions;
	static void InitializeDefinition(const char* path);
	bool LoadFromXmlElement(const XmlElement& element);
	bool LoadSpawnInfoFromXmlElement(const XmlElement& element);
};