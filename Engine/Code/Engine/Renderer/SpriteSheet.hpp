#pragma once
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <vector>

class SpriteSheet {
public:
	explicit SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout);

	Texture& GetTexture()const;
	int GetNumSprite() const;
	SpriteDefinition const& GetSpriteDef(int spriteIndex)const;
	void GetSpriteUVs(Vec2& outMins, Vec2& outMaxs, int spriteIndex);
	AABB2 GetSpriteUVs(int spriteIndex) const;
	AABB2 GetSpriteUVs(IntVec2 spriteCoord) const;
	AABB2 GetSpriteUVsTrimmed(int spriteIndex, int pixelTrmmed = 1) const;
protected:
	Texture* m_texture;
	std::vector<SpriteDefinition> m_sprites;
	IntVec2 m_dimnesions;
};