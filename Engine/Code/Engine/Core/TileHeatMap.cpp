#include "Engine/Core/TileHeatMap.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Rgba8.hpp"
TileHeatMap::TileHeatMap(IntVec2 const& dimensions, float initialValue)
	:m_dimensions(dimensions)
{
	m_values.resize(dimensions.x * dimensions.y);
	SetAll(initialValue);
}
float TileHeatMap::GetHeat(IntVec2 position) const {
	return m_values[position.x + position.y * m_dimensions.x];
}
void  TileHeatMap::SetHeat(IntVec2 position, float newHeat) {
	m_values[position.x + position.y * m_dimensions.x] = newHeat;
}
void  TileHeatMap::AddHeat(IntVec2 position, float heatToAdd) {
	m_values[position.x + position.y * m_dimensions.x] += heatToAdd;
}
void TileHeatMap::SetAll(float newHeat) {
	for (int i = 0; i < m_values.size();i++) {
		m_values[i] = newHeat;
	}
}
void TileHeatMap::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 totalBounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor,  float specialValue, Rgba8 specialColor) {
	for (int y = 0; y < m_dimensions.y; y++) {
		for (int x = 0; x < m_dimensions.x; x++) {
			float value = GetHeat(IntVec2(x, y));
			Rgba8 color;
			if (value == specialValue) {
				color = specialColor;
			}
			else {
				float fraction = GetFractionWithinRange(value, valueRange.m_min, valueRange.m_max);
				color = Rgba8::InterpolateColor(lowColor, highColor, fraction);
				if (value == specialValue) {
					color = specialColor;
				}
			}
			
			float gridX = (totalBounds.m_maxs.x - totalBounds.m_mins.x) / m_dimensions.x;
			float gridY = (totalBounds.m_maxs.y - totalBounds.m_mins.y) / m_dimensions.y;
			Vec2 BL = Vec2(gridX * x, gridY * y);
			Vec2 BR = Vec2(BL.x + gridX, BL.y);
			Vec2 TR = Vec2(BL.x + gridX, BL.y + gridY);
			Vec2 TL = Vec2(BL.x, BL.y + gridY);
			AddVertsForRect(verts, BR, TR, TL, BL,color,Vec2(0,0));
		}
	}
}