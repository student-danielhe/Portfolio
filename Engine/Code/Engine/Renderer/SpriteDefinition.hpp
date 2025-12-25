#pragma once

#include"Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
class Texture;
class SpriteSheet;
class SpriteDefinition {
public:
	explicit SpriteDefinition(SpriteSheet const& spriteSheet, int spriteIndex, Vec2 UV_min, Vec2 UV_max);
	void GetUV(Vec2& outMins, Vec2& outMaxs);
	AABB2 GetUV()const;
	SpriteSheet const& GetSpriteSheet()const;
	Texture& GetTexture()const;
	float GetAspect()const;
protected:
	AABB2 m_UV;
	SpriteSheet const& m_spriteSheet;
	int m_index=-1;
};