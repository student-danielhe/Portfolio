#include "Engine/Core/HeatMaps.hpp"
HeatMaps::HeatMaps(IntVec2 const& dimensions)
	:m_dimensions(dimensions)
{
	m_values.clear();
}
float HeatMaps::GetHeat(IntVec2 position) const {
	return m_values[position.x + position.y * m_dimensions.x];
}
void  HeatMaps::SetHeat(IntVec2 position, float newHeat) {
	m_values[position.x + position.y * m_dimensions.x] = newHeat;
}
void  HeatMaps::AddHeat(IntVec2 position, float heatToAdd) {
	m_values[position.x + position.y * m_dimensions.x] += heatToAdd;
}