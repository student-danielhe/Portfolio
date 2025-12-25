#pragma once
#include "SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB3.hpp"
enum TextBoxMode {
	SHRINK_TO_FIT,
	OVERRUN
};
class BitmapFont
{
	friend class Renderer; 

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);

public:
	Texture& GetTexture();

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8(255,255,255), float cellAspectScale = 1.f);
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, std::string const& text, AABB2 const& box, float cellHeight, Rgba8 const& tint = Rgba8(255,255,255),
		float cellAspectScale = 1.f, Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT, int maxGlyphsToDraw = INT_MAX);


	float GetTextWidth(float cellHeight, std::string const& text, float cellAspectScale = 1.f);
	void AddVertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& verts,
		float cellheight, std::string const& text, Rgba8 const& tint = Rgba8(255, 255, 255),
		float cellAspect = 1.f, Vec2 const& alignment = Vec2(.5f, .5f),
		int maxGlyphsToDraw = 999);
protected:
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return m_fontDefaultAspect

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
	float		m_fontDefaultAspect = 1.0f; // For basic (tier 1) fonts, set this to the aspect of the sprite sheet texture
};

