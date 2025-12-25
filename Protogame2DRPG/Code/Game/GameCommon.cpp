#include "GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
//#include "Engine/Math/RandomNumberGenerator.hpp"
void DebugDrawRing(Vec2 pos, float radius,float thickness, Rgba8 color) {
	constexpr int NUM_SIDES = 12;
	constexpr float DEG_PER_SIDE = 360.f / NUM_SIDES;
	Vec2 start(radius, 0);
	Vec2 end = start.GetRotatedDegrees(DEG_PER_SIDE);
	for (int i = 0; i < NUM_SIDES; i++) {
		DebugDrawLine(pos + start, pos + end, thickness, color);
		start.RotateDegrees(DEG_PER_SIDE);
		end.RotateDegrees(DEG_PER_SIDE);
	}

}
void DebugDrawLine(Vec2 start, Vec2 end, float thickness, Rgba8 color) {
	Vertex_PCU* arr = new Vertex_PCU[6];
	Vec2 uv (0, 0);
	Vec2 step = end - start;
	step.SetLength(thickness);
	Vec2 stepLeft = step.GetRotated90Degrees();
	Vec2 SL = start - step + stepLeft;
	Vec2 SR = start - step - stepLeft;
	Vec2 EL = end + step + stepLeft;
	Vec2 ER = end + step - stepLeft;
	arr[0].m_position = Vec3(SL.x, SL.y, 0);
	arr[1].m_position = Vec3(SR.x, SR.y, 0);
	arr[2].m_position = Vec3(ER.x, ER.y, 0);
	arr[3].m_position = Vec3(EL.x, EL.y, 0);
	arr[4].m_position = Vec3(ER.x, ER.y, 0);
	arr[5].m_position = Vec3(SL.x, SL.y, 0);
	for (int i = 0; i < 6; i++) {
		arr[i].m_color = color;
		arr[i].m_uvTexCoords = uv;
	}
	g_theRenderer->DrawVertexArray(6, arr);
}
int  GetConnectedControllerID() {
	for (int i = 0; i < 4; i++) {
		if (g_theInput->GetController(i).IsConnected()) {
			return i;
		}
	}
	return 0;
}

int Noise1DGetRandomIntInRange(int input, int minInclusive, int maxInclusive)
{
	float percentage = Get1dNoiseZeroToOne(input, GAME_SEED);
	int result = minInclusive + (int)roundf(percentage * (maxInclusive - minInclusive));

	if (result < minInclusive) {
		result = minInclusive;
	}

	if (result > maxInclusive) {
		result = maxInclusive;
	}

	return result;
}

int Noise2DGetRandomIntInRange(int inputX, int inputY, int minInclusive, int maxInclusive)
{
	float percentage = Get2dNoiseZeroToOne(inputX, inputY, GAME_SEED);
	int result = minInclusive + (int)roundf(percentage * (maxInclusive - minInclusive));
	
	if (result < minInclusive) {
		result = minInclusive;
	}

	if (result > maxInclusive) {
		result = maxInclusive;
	}

	return result;
}

int Noise4DGetRandomIntInRange(int inputX, int inputY, int inputZ, int inputT, int minInclusive, int maxInclusive)
{
	float percentage = Get4dNoiseZeroToOne(inputX, inputY, inputZ, inputT, GAME_SEED);
	int result = minInclusive + (int)roundf(percentage * (maxInclusive - minInclusive));

	if (result < minInclusive) {
		result = minInclusive;
	}

	if (result > maxInclusive) {
		result = maxInclusive;
	}

	return result;
}

void CellularAutomataGrid(std::vector<bool>& grids, IntVec2 dimension, std::vector<int> born, std::vector<int> survive, int maxIteration)
{
    std::vector<bool>& newGrids = grids;
	for (int iterations = 0; iterations < maxIteration; iterations++) {
		for (int i = 0; i < grids.size(); i++) {

			int liveNeighbor = GetNumLiveNeighbor(grids, dimension, i);

			bool bornCheck = false;
			for (int bornCondition : born) {
				if (liveNeighbor == bornCondition)
					bornCheck = true;
			}

			bool surviveCheck = false;
			for (int surviveCondition : survive) {
				if (liveNeighbor == surviveCondition)
					surviveCheck = true;
			}

			newGrids[i] = (grids[i] || bornCheck) && (surviveCheck);
		}
	}
	grids = newGrids;

}

int GetNumLiveNeighbor(std::vector<bool> grid, IntVec2 dimension, int index)
{
	int results=0;
	int x = (int)index % dimension.x;
	int y = (int)(index - x) / dimension.x;

	if (x - 1 >= 0) {
		results += grid[(x - 1) + (y)*dimension.x];

		if (y - 1 >= 0) {
			results += grid[(x - 1) + (y - 1) * dimension.x];
		}

		if (y + 1 < dimension.y) {
			results += grid[(x - 1) + (y + 1) * dimension.x];
		}
	}
	else {
		results++;
		if (y - 1 < 0) {
			results ++;
		}

		if (y + 1 >= dimension.y) {
			results ++;
		}
	}

	if (x + 1 < dimension.x) {

		results += grid[(x + 1) + (y)*dimension.x];
		if (y - 1 >= 0) {
			results += grid[(x + 1) + (y - 1) * dimension.x];
		}

		if (y + 1 < dimension.y) {
			results += grid[(x + 1) + (y + 1) * dimension.x];
		}
	}
	else {
		results++;
		if (y - 1 < 0) {
			results++;
		}

		if (y + 1 >= dimension.y) {
			results++;
		}
	}

	if (y - 1 >= 0) {
		results += grid[(x)+(y - 1) * dimension.x];
	}
	else {
		results++;
	}

	if (y + 1 < dimension.y) {
		results += grid[(x)+(y + 1) * dimension.x];
	}
	else {
		results++;
	}

	return results;
}

bool InBound(IntVec2 dimensions, IntVec2 pos)
{
	return pos.x >= 0 && pos.x < dimensions.x && pos.y >= 0 && pos.y < dimensions.y;
}

std::vector<IntVec2> SpreadHeat(TileHeatMap& heatMap, std::vector<IntVec2> previousTiles, int currentHeat, std::vector<bool> spreadAble)
{
	std::vector<IntVec2> result;
	for (IntVec2 tile : previousTiles) {
		int x = tile.x;
		int y = tile.y;
		//check all adjacent tile, spread heat if possible, then store all tile heated
		if (IsSpreadable(heatMap, IntVec2(x - 1, y), spreadAble)) {
			result.push_back(IntVec2(x - 1, y));
			heatMap.SetHeat(IntVec2(x - 1, y), currentHeat + 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x + 1, y), spreadAble)) {
			result.push_back(IntVec2(x + 1, y));
			heatMap.SetHeat(IntVec2(x + 1, y), currentHeat + 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x, y - 1), spreadAble)) {
			result.push_back(IntVec2(x, y - 1));
			heatMap.SetHeat(IntVec2(x, y - 1), currentHeat + 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x, y + 1), spreadAble)) {
			result.push_back(IntVec2(x, y + 1));
			heatMap.SetHeat(IntVec2(x, y + 1), currentHeat + 1.f);
		}

	}
	return result;
}

bool IsSpreadable(TileHeatMap& map, IntVec2 tile, std::vector<bool> spreadAble)
{
	//check is solid
	if ((tile.x<0||tile.y<0||tile.x>=map.m_dimensions.x||tile.y>=map.m_dimensions.y) ||
	    !spreadAble.empty()&& !spreadAble[tile.x + tile.y * map.m_dimensions.x]) {
		return false;
	}
	//if heat not yet set, return true
	if (map.GetHeat(tile) == -1.f) {
		return true;
	}
	else {
		return false;
	}
}

int GetRandomIndexFromWeightedList(std::vector<float> list, int inputX, int inputY, int inputZ, int inputT)
{
    float totalWeight = 0.f;
	for (int i = 0; i < list.size(); i++) {
	    totalWeight+=list[i];
	}

	float percentage = Get4dNoiseZeroToOne(inputX, inputY, inputZ, inputT, GAME_SEED);
	float result = percentage * totalWeight;

	if (result < 0.f) {
		result = 0.f;
	}

	if (result > totalWeight) {
		result = totalWeight;
	}

	float currentWeight = 0.f;

	for (int i = 0; i < list.size(); i++) {
		currentWeight += list[i];
		if (currentWeight >= result) {
			return i;
		}
	}

	return -1;
}
