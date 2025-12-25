#include"Engine/Renderer/SpriteDefinition.hpp"
#include"Engine/Renderer/SpriteSheet.hpp"
SpriteDefinition::SpriteDefinition(SpriteSheet const& spriteSheet, int spriteIndex, Vec2 UV_min, Vec2 UV_max)
	: m_spriteSheet(spriteSheet)
	, m_index(spriteIndex)
	, m_UV(AABB2(Vec2(UV_min.x+ 1.f/262144.f,UV_min.y+ 1.f/65536.f), Vec2(UV_max.x- 1.f/262144.f,UV_max.y- 1.f/65536.f)))
{

}
void SpriteDefinition::GetUV(Vec2& outMins,Vec2& outMaxs){
	outMins = m_UV.m_mins;
	outMaxs = m_UV.m_maxs;
}
AABB2 SpriteDefinition::GetUV() const{
	return m_UV;
}
SpriteSheet const& SpriteDefinition::GetSpriteSheet()const {
	return m_spriteSheet;
}
Texture& SpriteDefinition::GetTexture()const {
	return m_spriteSheet.GetTexture();
}
float SpriteDefinition::GetAspect()const {
	Vec2 dimension = m_UV.GetDimensions();
	return dimension.x / dimension.y;
}
