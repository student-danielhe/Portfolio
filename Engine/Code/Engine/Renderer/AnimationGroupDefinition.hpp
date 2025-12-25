#pragma once
#include "SpriteAnimDefinition.hpp"
#include "Engine/Math/Vec3.hpp"
struct DirAnimation {
	Vec3* direction = nullptr;
	SpriteAnimDefinition* spriteDef = nullptr;
};

struct AnimationGroupDefinition {
	std::string m_name = "Walk";
	bool scaleBySpeed = false;
	std::vector<DirAnimation*> m_directions;
};