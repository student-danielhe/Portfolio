#pragma once
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <vector>
class VoxelHeatMap {
public:
	VoxelHeatMap(IntVec3 const& dimensions, int initialValue = 0);
	float GetHeat(IntVec3 position) const;
	void SetHeat(IntVec3 position, int newHeat);
	void AddHeat(IntVec3 position, int heatToAdd);
	void SetAll(int newHeat);
	bool InBound(IntVec3 pos);

	std::vector<IntVec3> SpreadDistField( std::vector<IntVec3> previousTiles, int currentHeat, std::vector<bool> spreadAble);

	bool DistFieldSpreadable( IntVec3 tile, std::vector<bool> spreadAble, int currentHeat);

	std::vector<IntVec3> SpreadInfluence( std::vector<IntVec3> previousTiles, int currentHeat, std::vector<bool> spreadAble);

	bool InfluenceSpreadable( IntVec3 tile, std::vector<bool> spreadAble, int currentHeat);

	IntVec3 m_dimensions;
	std::vector<int> m_values;
};