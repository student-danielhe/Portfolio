#include "Engine/Renderer/SpriteSheet.hpp"
SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout)
	:m_texture(&texture)
	,m_dimnesions(simpleGridLayout)
{
	float x = 0.f;
	float y = static_cast<float>(simpleGridLayout.y-1.f);
	for (int i = 0; i < (simpleGridLayout.x * simpleGridLayout.y); i++) {
		m_sprites.push_back(SpriteDefinition(*this, 1, Vec2(x/simpleGridLayout.x,y/simpleGridLayout.y),Vec2((x+1)/simpleGridLayout.x,(y+1)/simpleGridLayout.y)));
		x++;
		if (x >= simpleGridLayout.x) {
			x = 0.f;
			y--;
		}

	}
}

Texture& SpriteSheet::GetTexture()const {
	return *m_texture;
}
int SpriteSheet::GetNumSprite() const {
	return static_cast<int>(m_sprites.size());
}
SpriteDefinition const& SpriteSheet::GetSpriteDef(int spriteIndex)const {
	return m_sprites[spriteIndex];
}
void SpriteSheet::GetSpriteUVs(Vec2& outMins, Vec2& outMaxs, int spriteIndex) {
	m_sprites[spriteIndex].GetUV(outMins, outMaxs);

}
AABB2 SpriteSheet::GetSpriteUVs(int spriteIndex) const {
	return m_sprites[spriteIndex].GetUV();
}
AABB2 SpriteSheet::GetSpriteUVs(IntVec2 coord) const {
	return m_sprites[coord.x+coord.y*m_dimnesions.x].GetUV();
}

AABB2 SpriteSheet::GetSpriteUVsTrimmed(int spriteIndex, int pixelTrmmed) const
{
	AABB2 UV = m_sprites[spriteIndex].GetUV();
	float trimX = (((float)pixelTrmmed)/(float)m_texture->GetDimensions().x);
	float trimY = (((float)pixelTrmmed) / (float)m_texture->GetDimensions().y);
	UV.m_mins+=Vec2(trimX, trimY);
	UV.m_maxs-=Vec2(trimX, trimY);
	return UV;
}
