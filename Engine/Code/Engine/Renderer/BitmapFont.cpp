#include "BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include"Engine/Core/EngineCommon.hpp"
BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture)
	:m_fontFilePathNameWithNoExtension(fontFilePathNameWithNoExtension)
	, m_fontGlyphsSpriteSheet(SpriteSheet(fontTexture, IntVec2(16, 16)))
{
	
}


Texture& BitmapFont::GetTexture() {
	return m_fontGlyphsSpriteSheet.GetTexture();
}
void     BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspectScale) {
	Vec2 currentMins = textMins;
	float cellWidth = cellHeight * cellAspectScale;
	for (char c : text) {
		//find position to draw
		Vec2 BR(currentMins.x + cellWidth, currentMins.y);
		Vec2 TR(currentMins.x + cellWidth, currentMins.y + cellHeight);
		Vec2 TL(currentMins.x, currentMins.y + cellHeight);
		Vec2 BL(currentMins.x, currentMins.y);
		//find sprite
		int charIndex = c;
		AABB2 UV;
		m_fontGlyphsSpriteSheet.GetSpriteUVs(UV.m_mins, UV.m_maxs, charIndex);

		AddVertsForRect(vertexArray,  BR, TR, TL,BL, tint,UV);
		currentMins.x += cellWidth;
	}
}


void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, std::string const& text, AABB2 const& box, float cellHeight, Rgba8 const& tint,
	float cellAspectScale, Vec2 const& alignment, TextBoxMode mode, int maxGlyphsToDraw) {
	float maxWidth=0;
	float height;
	std::vector<std::string> paragraph;
	//------------------------------------------
	std::string currentLine;
	for (int i = 0; i < text.length(); i++) {
		//potential bug: some one print paragraph ends with /
		if ((text.c_str()[i]== '/')&&(text.c_str()[i+1]=='n')) {
			//create next line
			paragraph.push_back(currentLine);
			if (currentLine.length() > maxWidth) {
				maxWidth = static_cast<float>(currentLine.length());
			}
			currentLine.clear();
			i++;
		}
		else {
			if (i < maxGlyphsToDraw) {
				currentLine.push_back(text.c_str()[i]);
			}
			else {
				currentLine.push_back(' ');
			}
			
		}
	}
	paragraph.push_back(currentLine);
	if (currentLine.length() > maxWidth) {
		maxWidth = static_cast<float>(currentLine.length());
	}
	height = static_cast<float>(paragraph.size());
	//paragraph now hold all lines to print

	//check text box size
	if (mode == SHRINK_TO_FIT) {
		if (cellHeight * height > box.GetDimensions().y) {
			cellHeight = box.GetDimensions().y / height;
		}if (cellHeight * cellAspectScale * maxWidth > box.GetDimensions().x) {
			cellHeight = box.GetDimensions().x / (maxWidth * cellAspectScale);
		}
	}

	//cell height is now shrunk to be able to fit in the box
	float cellWidth = cellHeight * cellAspectScale;
	for (float line = 0.f; line < paragraph.size();line++) {
		std::string stringToDraw = paragraph[static_cast<const unsigned int>(line)];
		float minsX = box.m_mins.x + alignment.x * box.m_maxs.x-alignment.x*stringToDraw.length()*cellWidth;
		float minsY = box.m_mins.y + alignment.y * box.m_maxs.y  -(line-height*alignment.y) * cellHeight;
		AddVertsForText2D(vertexArray, Vec2(minsX, minsY), cellHeight, stringToDraw, tint, cellAspectScale);
	}


}
float    BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspectScale) {
	return text.size() * cellHeight * cellAspectScale;
}
float    BitmapFont::GetGlyphAspect(int glyphUnicode) const {
	UNUSED(glyphUnicode);
	return m_fontDefaultAspect;
}

void BitmapFont::AddVertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& verts, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspectScale, Vec2 const& alignment, int maxGlyphsToDraw) {
	int length = maxGlyphsToDraw;
	if (text.size() < maxGlyphsToDraw) {
		length = (int)text.size();
	}
	Vec2 currentMins = Vec2(0.f-(1.f-alignment.x)*length, 0.f-(1.f-alignment.y)*cellHeight);
	float cellWidth = cellHeight * cellAspectScale;
	int i = 0;
	for (char c : text) {
		//find position to draw
		Vec2 BR(currentMins.x + cellWidth, currentMins.y);
		Vec2 TR(currentMins.x + cellWidth, currentMins.y + cellHeight);
		Vec2 TL(currentMins.x, currentMins.y + cellHeight);
		Vec2 BL(currentMins.x, currentMins.y);
		//find sprite
		int charIndex = c;
		AABB2 UV;
		m_fontGlyphsSpriteSheet.GetSpriteUVs(UV.m_mins, UV.m_maxs, charIndex);

		AddVertsForRect(verts, BR, TR, TL, BL, tint, UV);
		currentMins.x += cellWidth;
		i++;
		if (i >= maxGlyphsToDraw) {
			break;
		}
	}
	Mat44 transform(
		Vec3(0, -1, 0),
		Vec3(0, 0, 1),
		Vec3(1, 0, 0),
		Vec3(0, 0, 0)
	);
	TransformVertexArray3D(verts, transform);
}
