#pragma once
#include "Engine/Core/TileHeatMap.hpp"
#include "ActorHandle.hpp"
class Map;
class Actor;
class Controller {
public:
	ActorHandle* m_ownerHandle = nullptr;
	Map* m_map = nullptr;
	Controller(ActorHandle* handle, Map* map);
	void Possess(ActorHandle* newActor);
	Actor* GetActor()const;
	void MoveToTile(IntVec2 pos);
	IntVec2 GetMyTile();
	Vec3 GetTargetPath(ActorHandle* target);
	TileHeatMap CreateHeatMap(IntVec2 targetCoords);
	std::vector<IntVec2> SpreadHeat(TileHeatMap& map, std::vector<IntVec2> previousTiles, int currentHeat);
	bool IsSpreadable(TileHeatMap& map, IntVec2 pos);
};