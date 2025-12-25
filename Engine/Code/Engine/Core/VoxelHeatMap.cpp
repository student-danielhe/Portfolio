#include "VoxelHeatMap.hpp"
VoxelHeatMap::VoxelHeatMap(IntVec3 const& dimensions, int initialValue /*= 0.f*/)
	:m_dimensions(dimensions)
{
	m_values.resize(dimensions.x * dimensions.y*dimensions.z);
	SetAll(initialValue);
}

float VoxelHeatMap::GetHeat(IntVec3 position) const {
	return m_values[position.x + position.y * m_dimensions.x + position.z * m_dimensions.x * m_dimensions.y];
}
void  VoxelHeatMap::SetHeat(IntVec3 position, int newHeat) {
	m_values[position.x + position.y * m_dimensions.x] = newHeat;
}
void  VoxelHeatMap::AddHeat(IntVec3 position, int heatToAdd) {
	m_values[position.x + position.y * m_dimensions.x + position.z * m_dimensions.x * m_dimensions.y] += heatToAdd;
}
void VoxelHeatMap::SetAll(int newHeat) {
	for (int i = 0; i < m_values.size(); i++) {
		m_values[i] = newHeat;
	}
}

bool VoxelHeatMap::InBound(IntVec3 tile)
{
	if (tile.x < 0 || tile.y < 0 || tile.z < 0 || tile.x >= m_dimensions.x || tile.y >= m_dimensions.y || tile.z >= m_dimensions.z) {
	    return false;
	}
	return true;
}

std::vector<IntVec3> VoxelHeatMap::SpreadDistField(std::vector<IntVec3> previousTiles, int currentHeat, std::vector<bool> spreadAble)
{
	std::vector<IntVec3> result;
	for (IntVec3 tile : previousTiles) {
		int x = tile.x;
		int y = tile.y;
		int z = tile.z;
		//check all adjacent tile, spread heat if possible, then store all tile heated
		if (DistFieldSpreadable(IntVec3(x - 1, y, z), spreadAble, currentHeat + 1)) {
			result.push_back(   IntVec3(x - 1, y, z));
			SetHeat(            IntVec3(x - 1, y, z), (float)(currentHeat + 1));
		}
		if (DistFieldSpreadable(IntVec3(x + 1, y, z), spreadAble, currentHeat + 1)) {
			result.push_back(   IntVec3(x + 1, y, z));
			SetHeat(            IntVec3(x + 1, y, z), (float)(currentHeat + 1));
		}

		if (DistFieldSpreadable(IntVec3(x, y - 1, z), spreadAble, currentHeat + 1)) {
			result.push_back(   IntVec3(x, y - 1, z));
			SetHeat(            IntVec3(x, y - 1, z), (float)(currentHeat + 1));
		}
		if (DistFieldSpreadable(IntVec3(x, y + 1, z), spreadAble, currentHeat + 1)) {
			result.push_back(   IntVec3(x, y + 1, z));
			SetHeat(            IntVec3(x, y + 1, z), (float)(currentHeat + 1));
		}

		if (DistFieldSpreadable(IntVec3(x, y, z - 1), spreadAble, currentHeat + 1)) {
			result.push_back(   IntVec3(x, y, z - 1));
			SetHeat(            IntVec3(x, y, z - 1), (float)(currentHeat + 1));
		}
		if (DistFieldSpreadable(IntVec3(x, y, z + 1), spreadAble, currentHeat + 1)) {
			result.push_back(   IntVec3(x, y, z + 1));
			SetHeat(            IntVec3(x, y, z + 1), (float)(currentHeat + 1));
		}
		

	}
	return result;
}

bool VoxelHeatMap::DistFieldSpreadable(IntVec3 tile, std::vector<bool> spreadAble, int currentHeat)
{
	//check is solid
	if ((!InBound(tile)) ||
		!spreadAble.empty() && !spreadAble[tile.x + tile.y * m_dimensions.x]) {
		return false;
	}
	//if heat not yet set, return true
	if (GetHeat(tile) == -1.f||GetHeat(tile)>currentHeat) {
		return true;
	}
	else {
		return false;
	}
}

std::vector<IntVec3> VoxelHeatMap::SpreadInfluence( std::vector<IntVec3> previousTiles, int currentHeat, std::vector<bool> spreadAble)
{
	std::vector<IntVec3> result;

	if (currentHeat <= 0) {
		return result;
	}

	for (IntVec3 tile : previousTiles) {
		int x = tile.x;
		int y = tile.y;
		int z = tile.z;
		//check all adjacent tile, spread heat if possible, then store all tile heated
		if (InfluenceSpreadable(IntVec3(x - 1, y, z), spreadAble, currentHeat - 1)) {
			result.push_back(IntVec3(x - 1, y, z));
			SetHeat(IntVec3(x - 1, y, z), (float)currentHeat - 1.f);
		}
		if (InfluenceSpreadable(IntVec3(x + 1, y, z), spreadAble, currentHeat - 1)) {
			result.push_back(IntVec3(x + 1, y, z));
			SetHeat(IntVec3(x + 1, y, z), (float)currentHeat - 1.f);
		}

		if (InfluenceSpreadable(IntVec3(x, y - 1, z), spreadAble, currentHeat - 1)) {
			result.push_back(IntVec3(x, y - 1, z));
			SetHeat(IntVec3(x, y - 1, z), (float)currentHeat - 1.f);
		}
		if (InfluenceSpreadable(IntVec3(x, y + 1, z), spreadAble, currentHeat - 1)) {
			result.push_back(IntVec3(x, y + 1, z));
			SetHeat(IntVec3(x, y + 1, z), (float)currentHeat - 1.f);
		}

		if (InfluenceSpreadable( IntVec3(x, y, z - 1), spreadAble, currentHeat - 1)) {
			result.push_back(IntVec3(x, y, z - 1));
			SetHeat(IntVec3(x, y, z - 1), (float)currentHeat - 1.f);
		}
		if (InfluenceSpreadable(IntVec3(x, y, z + 1), spreadAble, currentHeat - 1)) {
			result.push_back(IntVec3(x, y, z + 1));
			SetHeat(IntVec3(x, y, z + 1), (float)currentHeat - 1.f);
		}


	}
	return result;
}

bool VoxelHeatMap::InfluenceSpreadable( IntVec3 tile, std::vector<bool> spreadAble, int currrentHeat)
{
	if ((!InBound(tile)) ||
		!spreadAble.empty() && !spreadAble[tile.x + tile.y * m_dimensions.x]) {
		return false;
	}

	if (GetHeat(tile) < currrentHeat) {
		return true;
	}
	return false;
}
