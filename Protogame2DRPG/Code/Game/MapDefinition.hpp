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
	IntVec2 m_position;
};
class MapDefinition {
public:
	std::string m_name;
	Image m_image;
    IntVec2 m_size;
	SpriteSheet* m_spiteSheet = nullptr;
	std::vector<SpawnInfo> m_spawnInfos;
	static std::vector<MapDefinition*> s_definitions;
	static void InitializeDefinition(const char* path);
	bool LoadFromXmlElement(const XmlElement& element);
	bool LoadSpawnInfoFromXmlElement(const XmlElement& element);
};