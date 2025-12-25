#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>
class TileHeatMap {
public:
	TileHeatMap(IntVec2 const& dimensions, float initialValue=0.f);
	float GetHeat(IntVec2 position) const;
	void SetHeat(IntVec2 position, float newHeat);
	void AddHeat(IntVec2 position,float heatToAdd);
	void SetAll(float newHeat);
	void AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 totalBounds, FloatRange valueRange = FloatRange(0.f, 1.f), Rgba8 lowColor=Rgba8(0,0,0), Rgba8 highColor=Rgba8(255,255,255), float specialValue = -1.f, Rgba8 specialColor = Rgba8(255, 0, 255));
	IntVec2 m_dimensions;
	std::vector<float> m_values;

};
