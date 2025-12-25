#include "Controller.hpp"
#include "Map.hpp"
#include "Actor.hpp"
Controller::Controller(ActorHandle* handle, Map* map)
:m_ownerHandle(handle)
,m_map(map){}

void   Controller::Possess(ActorHandle* newActor) {
	if (GetActor() != nullptr) {
		GetActor()->OnUnpossessed();
	}
	if (m_map->GetActorByHandle(*newActor) != nullptr) {
		m_map->GetActorByHandle(*newActor)->OnPossessed(this);
	}
}
Actor* Controller::GetActor()const {
	if (m_ownerHandle == nullptr) {
		return nullptr;
	}
	return m_map->GetActorByHandle(*m_ownerHandle);
}

void Controller::MoveToTile(IntVec2 pos)
{

}

IntVec2 Controller::GetMyTile()
{
	if (m_ownerHandle) {
	    Actor* owner = m_map->GetActorByHandle(*m_ownerHandle);
		if (owner) {
		    return m_map->PositionToCoords(owner->m_bottomPosition);
		}
    }
	return IntVec2(-1,-1);
}

Vec3 Controller::GetTargetPath(ActorHandle* target)
{
    IntVec2 targetTile(-1,-1);
	if (target) {
		Actor* targetActor = m_map->GetActorByHandle(*target);
		if (targetActor) {
			targetTile = targetActor->m_controller->GetMyTile();
		}
	}
	if (targetTile.x == -1) {
		return Vec3();
	}
	IntVec2 myTile = GetMyTile();
	if (targetTile == myTile) {
		return Vec3();
	}
    TileHeatMap map = CreateHeatMap(targetTile);
	
	int heat = (int)map.GetHeat(myTile);

	if (map.GetHeat(IntVec2(myTile.x-1, myTile.y)) == heat - 1) {
		return Vec3(-1.f, 0.f, 0.f);
	}
	if (map.GetHeat(IntVec2(myTile.x+1, myTile.y)) == heat - 1) {
		return Vec3(1.f, 0.f, 0.f);
	}
	if (map.GetHeat(IntVec2(myTile.x, myTile.y-1)) == heat - 1) {
		return Vec3(0.f, -1.f, 0.f);
	}
	if (map.GetHeat(IntVec2(myTile.x, myTile.y+1)) == heat - 1) {
		return Vec3(0.f, 1.f, 0.f);
	}
	return Vec3();
}

TileHeatMap Controller::CreateHeatMap(IntVec2 targetCoords)
{
    TileHeatMap heatMap(IntVec2(50,50),-1.f);
	heatMap.SetAll(-1);
	heatMap.SetHeat(targetCoords, 0);
	std::vector<IntVec2> tileToSpread;
	tileToSpread.push_back(targetCoords);
	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(heatMap, tileToSpread, currentHeat);
		currentHeat++;
	}
	return heatMap;
}

std::vector<IntVec2> Controller::SpreadHeat(TileHeatMap& heatMap, std::vector<IntVec2> previousTiles, int currentHeat)
{
	std::vector<IntVec2> result;
	for (IntVec2 tile : previousTiles) {
		int x = tile.x;
		int y = tile.y;
		//check all adjacent tile, spread heat if possible, then store all tile heated
		if (IsSpreadable(heatMap, IntVec2(x - 1, y))) {
			result.push_back(IntVec2(x - 1, y));
			heatMap.SetHeat(IntVec2(x - 1, y), currentHeat + 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x + 1, y))) {
			result.push_back(IntVec2(x + 1, y));
			heatMap.SetHeat(IntVec2(x + 1, y), currentHeat + 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x, y - 1))) {
			result.push_back(IntVec2(x, y - 1));
			heatMap.SetHeat(IntVec2(x, y - 1), currentHeat + 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x, y + 1))) {
			result.push_back(IntVec2(x, y + 1));
			heatMap.SetHeat(IntVec2(x, y + 1), currentHeat + 1.f);
		}

	}
	return result;
}

bool Controller::IsSpreadable(TileHeatMap& map,IntVec2 tile)
{
	//check in bound
	if (tile.x < 0 || tile.x >= 49) {
		return false;
	}
	if (tile.y < 0 || tile.y >= 49) {
		return false;
	}
	//check is solid

	if (TileDefinition::GetTileDef(m_map->m_tiles[tile.x + tile.y * 50].m_tileType)->m_isSolid) {
		return false;
	}
	//if heat not yet set, return true
	if (map.GetHeat(tile) == -1.f) {
		//heatMap.SetHeat(tile, currentHeat + 1);
		return true;
	}
	else {
		return false;
	}
}
