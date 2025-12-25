#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>
class HeatMaps {
public:
	HeatMaps(IntVec2 const& dimensions);
	float GetHeat(IntVec2 position) const;
	void SetHeat(IntVec2 position, float newHeat);
	void AddHeat(IntVec2 position,float heatToAdd);
private:
	IntVec2 m_dimensions;
	std::vector<float> m_values;

};
