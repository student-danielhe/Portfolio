#include "MapGenerator.hpp"
#include "StairInteraction.hpp"
#include "MonsterInteraction.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
std::vector<IntVec2> g_directions{IntVec2(0,1), IntVec2(0,-1), IntVec2(1,0), IntVec2(-1,0)};
std::vector<Map*> MapGenerator::AbstractMapToRealMaps()
{
    std::vector<Map*> result;

    for (int i = 0; i < (int)m_maps.size(); i++) {

		if (m_dungeonMetrics[i]->m_method == GenerationMethod::BINARY_SPACE_PARTITION) {
		    result.push_back(BSPMapToRealMap(m_maps[i], i));
		}
		else {
			result.push_back(CelluarMapToRealMap(m_maps[i], i));
		}
        AddGrass(result[i], TileType::FLOOR, TileType::GRASS, i);
    }

	for (int i = 0; i < (int)m_maps.size()-1; i++) {
	    IntVec2 pos = m_maps[i]->m_mainPath->m_path[m_maps[i]->m_mainPath->m_path.size()-1]->m_end;
	    SpawnStair(result[i], result[i+1], pos);
	}
    


    return result;

}



Map* MapGenerator::BSPMapToRealMap(AbstractMap* map, int noiseIndex)
{
	Map* newMap = new Map(map->m_dimensions, m_owner);

	

	//Fill with walls
	for (int y = 0; y < newMap->m_dimensions.y; y++) {
		for (int x = 0; x < newMap->m_dimensions.x; x++) {
			Tile newTile;
			newTile.m_pos = IntVec2(x, y);
			newTile.m_type = TileType::WALL;

			newMap->m_tiles.push_back(newTile);
		}
	}
	CreatePathForRealMap(map->m_mainPath, newMap, TileType::SAND);
	CreatePathForRealMap(map->m_splitPath, newMap, TileType::SAND);

	for (Room* room : map->m_rooms) {
		CreateRoomForRealmap(room, newMap, noiseIndex);

	}
	
	


	return newMap;
}

Map* MapGenerator::CelluarMapToRealMap(AbstractMap* map, int noiseIndex)
{
    bool debug = false;
	Map* newMap = new Map(map->m_dimensions, m_owner);
	std::vector<bool> spreadable;
	TileHeatMap* pathHeatMap = CreatePathHeatMap(map, spreadable);
	TileHeatMap* borderMap = new TileHeatMap(map->m_dimensions, -1.f);

	std::vector<IntVec2> tileToSpread;

	for (int y = 0; y < newMap->m_dimensions.y; y++) {
		for (int x = 0; x < newMap->m_dimensions.x; x++) {

			if (GetIsBorder(pathHeatMap, IntVec2(x, y))) {
			    tileToSpread.push_back(IntVec2(x,y));
				borderMap->SetHeat(IntVec2(x,y), 0.f);
			}

		}
	}


	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*borderMap, tileToSpread, currentHeat, spreadable);
		currentHeat++;
	}





	std::vector<bool> grid;
	for (int y = 0; y < newMap->m_dimensions.y; y++) {
		for (int x = 0; x < newMap->m_dimensions.x; x++) {
			if (pathHeatMap->GetHeat(IntVec2(x, y)) < 2.f&&pathHeatMap->GetHeat(IntVec2(x, y))!=-1.f) {
				int noise = Noise4DGetRandomIntInRange(x, y, noiseIndex, 0, 0, 5);
				int heat = pathHeatMap->GetHeat(IntVec2(x, y));

				grid.push_back(debug||noise>=heat);
			}
			else if (borderMap->GetHeat(IntVec2(x, y)) < 2.f&&borderMap->GetHeat(IntVec2(x, y))!=-1.f) {
				int noise = Noise4DGetRandomIntInRange(x, y, noiseIndex, 0, 0, 10);
				int heat = pathHeatMap->GetHeat(IntVec2(x, y));
				grid.push_back(false);
			}
			else {
			    bool chance = Noise4DGetRandomIntInRange(x, y, noiseIndex, 0, 0, 100)<m_dungeonMetrics[map->m_index]->m_openness;
				grid.push_back(debug||chance);
			}
			

		}
	}

	for (int y = 0; y < newMap->m_dimensions.y; y++) {
		grid[y * map->m_dimensions.x] = false;
		grid[y * map->m_dimensions.x + map->m_dimensions.x - 1] = false;
	}
	for (int x = 0; x < newMap->m_dimensions.x; x++) {
		grid[x] = false;
		grid[x + (map->m_dimensions.y - 1) * map->m_dimensions.x] = false;
	}
	
	
    std::vector<Room*> possibleRooms;
	ReplacementRule rule = m_dungeonMetrics[noiseIndex]->m_replacementRule;
	if (rule != ReplacementRule::NO_REPLACEMENT) {

		for (Room * room: map->m_rooms) {
		    possibleRooms.push_back(room);
		}
	}

	for (int i = 0; i < m_dungeonMetrics[noiseIndex]->m_replaceAmount; i++) {

		if (possibleRooms.size() != 0) {
		    int idx = Noise4DGetRandomIntInRange(noiseIndex, i, map->m_dimensions.x, map->m_rooms.size(), 0, possibleRooms.size() - 1);
			Room* room  = possibleRooms[idx];
			for (int y = (int)room->m_constraints.m_mins.y; y <= (int)room->m_constraints.m_maxs.y; y++) {
				for (int x = (int)room->m_constraints.m_mins.x; x <= (int)room->m_constraints.m_maxs.x; x++) {
						grid[x + y * map->m_dimensions.x]=false;
				}
			}

			room->m_replaced = rule;


			possibleRooms.erase(possibleRooms.begin()+idx);
		}
		
	}

	

	std::vector<int> born{ 6, 7, 8 };
	std::vector<int> survive{3, 4, 5, 6, 7, 8 };
	
	if(!debug)
	    CellularAutomataGrid(grid, map->m_dimensions, born, survive);
	
	

	pathHeatMap = CreatePathHeatMap(map, grid);

	for (int y = 0; y < newMap->m_dimensions.y; y++) {
		for (int x = 0; x < newMap->m_dimensions.x; x++) {
			Tile newTile;
			newTile.m_pos = IntVec2(x, y);

			if (pathHeatMap->GetHeat(IntVec2(x, y)) == -1) {
			    newTile.m_type = TileType::WALL;
			}
			else {
				if (grid[x + y * map->m_dimensions.x]) {
					newTile.m_type = TileType::FLOOR;
				}
				else {
					newTile.m_type = TileType::WALL;
				}
			}
			newMap->m_tiles.push_back(newTile);
		}
	}

	for (Room* room : map->m_rooms) {
		if (room->m_replaced != ReplacementRule::NO_REPLACEMENT) {
			CreateRoomForRealmap(room, newMap, noiseIndex);
		}
	    	
	}
	
	CreatePathForRealMap(map->m_mainPath, newMap, TileType::FLOOR);

	CreatePathForRealMap(map->m_splitPath, newMap, TileType::FLOOR);
	
	
	


	return newMap;
	
}

MapGenerator::MapGenerator()
{
    
}

bool MapGenerator::RoomCuttable(Room* room)
{
    if (abs(room->m_constraints.m_maxs.x - room->m_constraints.m_mins.x) < 10.f) {
        return false;
    }

	if (abs(room->m_constraints.m_maxs.y - room->m_constraints.m_mins.y) < 10.f) {
		return false;
	}
    return true;
}


TileHeatMap* MapGenerator::CreatePathHeatMap(AbstractMap* map, std::vector<bool> spreadable)
{
	TileHeatMap* heatMap = new TileHeatMap(map->m_dimensions, -1.f);
	heatMap->SetAll(-1);

    std::vector<IntVec2> tileToSpread;
	
	for (PathSegment* seg : map->m_mainPath->m_path) {

		IntVec2 currentPos = seg->m_begin;

        heatMap->SetHeat(currentPos, 0.f);
		tileToSpread.push_back(currentPos);

		while (currentPos != seg->m_end) {

			if (currentPos.x < seg->m_end.x) {
				currentPos.x++;
			}

			if (currentPos.x > seg->m_end.x) {
				currentPos.x--;
			}

			if (currentPos.y < seg->m_end.y) {
				currentPos.y++;
			}

			if (currentPos.y > seg->m_end.y) {
				currentPos.y--;
			}

			heatMap->SetHeat(currentPos, 0.f);
			tileToSpread.push_back(currentPos);

		}

	}


	for (PathSegment* seg: map->m_splitPath->m_path) {

		IntVec2 currentPos = seg->m_begin;

		heatMap->SetHeat(currentPos, 0.f);
		tileToSpread.push_back(currentPos);

		while (currentPos != seg->m_end) {

			if (currentPos.x < seg->m_end.x) {
				currentPos.x++;
			}

			if (currentPos.x > seg->m_end.x) {
				currentPos.x--;
			}

			if (currentPos.y < seg->m_end.y) {
				currentPos.y++;
			}

			if (currentPos.y > seg->m_end.y) {
				currentPos.y--;
			}

			heatMap->SetHeat(currentPos, 0.f);
			tileToSpread.push_back(currentPos);

		}
	}
	

	

	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*heatMap, tileToSpread, currentHeat,spreadable);
		currentHeat++;
	}
	return heatMap;
}

TileHeatMap* MapGenerator::CreateMainPathHeatMap(AbstractMap* map, Map* realMap)
{
    std::vector<bool> spreadable;
	spreadable.resize(map->m_dimensions.x * map->m_dimensions.y);
	for (int y = 0; y < realMap->m_dimensions.y; y++) {
		for (int x = 0; x < realMap->m_dimensions.x; x++) {
			spreadable[x+ y*map->m_dimensions.x] = !realMap->IsBlocked(IntVec2(x,y));
		}
	}


	TileHeatMap* heatMap = new TileHeatMap(map->m_dimensions, -1.f);
	heatMap->SetAll(-1);

	std::vector<IntVec2> tileToSpread;

	for (PathSegment* seg : map->m_mainPath->m_path) {

		IntVec2 currentPos = seg->m_begin;

		heatMap->SetHeat(currentPos, 0.f);
		tileToSpread.push_back(currentPos);

		while (currentPos != seg->m_end) {

			if (currentPos.x < seg->m_end.x) {
				currentPos.x++;
			}

			if (currentPos.x > seg->m_end.x) {
				currentPos.x--;
			}

			if (currentPos.y < seg->m_end.y) {
				currentPos.y++;
			}

			if (currentPos.y > seg->m_end.y) {
				currentPos.y--;
			}

			heatMap->SetHeat(currentPos, 0.f);
			tileToSpread.push_back(currentPos);

		}

	}

	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*heatMap, tileToSpread, currentHeat, spreadable);
		currentHeat++;
	}
	return heatMap;

}

TileHeatMap* MapGenerator::CreateSplitPathHeatMap(AbstractMap* map, Map* realMap)
{
	std::vector<bool> spreadable;
	spreadable.resize(map->m_dimensions.x * map->m_dimensions.y);
	for (int y = 0; y < realMap->m_dimensions.y; y++) {
		for (int x = 0; x < realMap->m_dimensions.x; x++) {
			spreadable[x + y * map->m_dimensions.x] = !realMap->IsBlocked(IntVec2(x, y));
		}
	}


	TileHeatMap* heatMap = new TileHeatMap(map->m_dimensions, -1.f);
	heatMap->SetAll(-1);

	std::vector<IntVec2> tileToSpread;

	for (PathSegment* seg : map->m_splitPath->m_path) {

		IntVec2 currentPos = seg->m_begin;

		heatMap->SetHeat(currentPos, 0.f);
		tileToSpread.push_back(currentPos);

		while (currentPos != seg->m_end) {

			if (currentPos.x < seg->m_end.x) {
				currentPos.x++;
			}

			if (currentPos.x > seg->m_end.x) {
				currentPos.x--;
			}

			if (currentPos.y < seg->m_end.y) {
				currentPos.y++;
			}

			if (currentPos.y > seg->m_end.y) {
				currentPos.y--;
			}

			heatMap->SetHeat(currentPos, 0.f);
			tileToSpread.push_back(currentPos);

		}

	}

	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*heatMap, tileToSpread, currentHeat, spreadable);
		currentHeat++;
	}
	return heatMap;
}

TileHeatMap* MapGenerator::CreateTreasureHeatMap(Map* map)
{
	std::vector<bool> spreadable;
	spreadable.resize(map->m_dimensions.x * map->m_dimensions.y);
	for (int y = 0; y < map->m_dimensions.y; y++) {
		for (int x = 0; x < map->m_dimensions.x; x++) {
			spreadable[x + y * map->m_dimensions.x] = !map->IsBlocked(IntVec2(x, y));
		}
	}


	TileHeatMap* heatMap = new TileHeatMap(map->m_dimensions, -1.f);
	heatMap->SetAll(-1);

	std::vector<IntVec2> tileToSpread;

	for (Object* obj:map->m_chObjects) {

		if (obj&&obj->m_def->m_type == ObjectType::CHEST) {
			IntVec2 currentPos = obj->m_position;
			heatMap->SetHeat(currentPos, 0.f);
			tileToSpread.push_back(currentPos);
		}
		
	}

		

	

	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*heatMap, tileToSpread, currentHeat, spreadable);
		currentHeat++;
	}
	return heatMap;
}

TileHeatMap* MapGenerator::CreateEnemyHeatMap(Map* map)
{
	std::vector<bool> spreadable;

	spreadable.resize(map->m_dimensions.x * map->m_dimensions.y);
	for (int y = 0; y < map->m_dimensions.y; y++) {
		for (int x = 0; x < map->m_dimensions.x; x++) {
			spreadable[x + y * map->m_dimensions.x] = !map->IsBlocked(IntVec2(x, y));
		}
	}


	TileHeatMap* heatMap = new TileHeatMap(map->m_dimensions, -1.f);
	heatMap->SetAll(-1);

	std::vector<IntVec2> tileToSpread;

	for (Object* obj : map->m_chObjects) {

		if (obj&&obj->m_def->m_type == ObjectType::MONSTER) {
			IntVec2 currentPos = obj->m_position;
			heatMap->SetHeat(currentPos, 0.f);
			tileToSpread.push_back(currentPos);
		}

	}





	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*heatMap, tileToSpread, currentHeat, spreadable);
		currentHeat++;
	}
	return heatMap;
}

TileHeatMap* MapGenerator::CreateOpenTerrainHeatMap(Map* realMap)
{
	std::vector<bool> spreadable;

	TileHeatMap* heatMap = new TileHeatMap(realMap->m_dimensions, -1.f);
	heatMap->SetAll(-1);

	std::vector<IntVec2> tileToSpread;

	for (int y = 0; y < realMap->m_dimensions.y; y++) {
		for (int x = 0; x < realMap->m_dimensions.x; x++) {
			if (realMap->m_tiles[x+y*realMap->m_dimensions.x].m_type == TileType::WALL) {
				IntVec2 currentPos = IntVec2(x,y);
				heatMap->SetHeat(currentPos, 0.f);
				tileToSpread.push_back(currentPos);
			}
		}
	}

	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(*heatMap, tileToSpread, currentHeat, spreadable);
		currentHeat++;
	}

	return heatMap;
}


float MapGenerator::GetEnemySpawnChanceAtPos(TileHeatMap* mainPathMap, TileHeatMap* splitPathMap, TileHeatMap* enemyMap, TileHeatMap* lootMap, TileHeatMap* openessMap, Map* map, IntVec2 pos)
{

	if (map->IsBlocked(pos)) {
		return 0.f;
	}

    float result = 0.f;

	float mainVacinity = (5.f-mainPathMap->GetHeat(pos))/5.f;
	float splitVacinity = (5.f-splitPathMap->GetHeat(pos))/5.f;

	if (mainVacinity < 0.f) {
		mainVacinity = 0.f;
	}
	if (splitVacinity < 0.f) {
		splitVacinity = 0.f;
	}

    float pathVal = mainVacinity * ENEMYSPAWN_MAINPATH_BIAS;
	float splitVal = splitVacinity * ENEMYSPAWN_SPLITPATH_BIAS;
	if (pathVal < splitVal) {
		pathVal = splitVal;
	}
	result +=pathVal;

	if (enemyMap->GetHeat(pos) != -1.f) {
		float enemyVal = enemyMap->GetHeat(pos)/map->m_dimensions.x* -ENEMYSPAWN_ENEMY_BIAS;

		result += enemyVal;

		if (enemyMap->GetHeat(pos) < (float)ENEMY_APART) {
			return 0.f;
		}
	}

	
	if (lootMap->GetHeat(pos) != -1.f) {
	    float val = (4.f - lootMap->GetHeat(pos))/4.f;
		if (val < 0.f) {
			val = 0.f;
		}
		float lootVal =  val * ENEMYSPAWN_LOOT_BIAS;
		result += pathVal;
	}
	

	float openVal = openessMap->GetHeat(pos)/map->m_dimensions.x*ENEMYSPAWN_OPENESS_BIAS;

	if (openessMap->GetHeat(pos) <= 1) {
		return 0.f;
	}

	result += openVal;

	if (result < 0.f) {
		return 0.f;
	}

	return result;
}

float MapGenerator::GetLootSpawnChanceAtPos(TileHeatMap* mainPathMap, TileHeatMap* splitPathMap, TileHeatMap* lootMap, TileHeatMap* openessMap, Map* map, IntVec2 pos)
{
	if (map->IsBlocked(pos)) {
		return 0.f;
	}

	float result = 0.f;

	float mainVacinity = (10.f - mainPathMap->GetHeat(pos)) / 10.f;
	float splitVacinity = (5.f - splitPathMap->GetHeat(pos)) / 5.f;

	if (mainVacinity < 0.f) {
		mainVacinity = 0.f;
	}
	if (splitVacinity < 0.f) {
		splitVacinity = 0.f;
	}

	float pathVal = mainVacinity * -LOOTSPAWN_MAINPATH_BIAS;
	float splitVal = splitVacinity * LOOTSPAWN_SPLITPATH_BIAS;

	result += pathVal;
	result += splitVal;


	if (lootMap->GetHeat(pos) != -1) {
		if (lootMap->GetHeat(pos) <= (float)LOOT_APART) {
			return 0.f;
		}


		float lootVal = lootMap->GetHeat(pos)/map->m_dimensions.x * -LOOTSPAWN_LOOT_BIAS;
		result += pathVal;
	}
	
	


	float openVal =  openessMap->GetHeat(pos)/map->m_dimensions.x * LOOTSPAWN_OPENESS_BIAS;

	if (openessMap->GetHeat(pos) <= 1) {
		return 0.f;
	}

	result += openVal;

	if (result < 0.f) {
		return 0.f;
	}

	return result;
}

void MapGenerator::SpawnEnemies(AbstractMap* absMap, Map* map, int spawnNum /*= 5*/)
{
    
	for (int spawned = 0; spawned < spawnNum; spawned++) {
	    bool sucess = false;
		TileHeatMap* mainMap = CreateMainPathHeatMap(absMap,map);
		TileHeatMap* splitMap = CreateSplitPathHeatMap(absMap, map);
		TileHeatMap* enemyMap = CreateEnemyHeatMap(map);
		TileHeatMap* lootMap = CreateTreasureHeatMap(map);
		TileHeatMap* openMap = CreateOpenTerrainHeatMap(map);
		
		std::vector<float> weightList;
		for (int y = 0; y < absMap->m_dimensions.y; y++) {
			for (int x = 0; x < absMap->m_dimensions.x; x++) {
                weightList.push_back(GetEnemySpawnChanceAtPos(mainMap, splitMap, enemyMap, lootMap, openMap, map, IntVec2(x,y)));
			}
		}
		 int index = GetRandomIndexFromWeightedList(weightList, absMap->m_dimensions.x, absMap->m_dimensions.y, spawned, absMap->m_index);

		 int x = index%absMap->m_dimensions.x;
		 int y = (index-x)/absMap->m_dimensions.x;

		 MonsterInteraction* interaction = new MonsterInteraction();

		 interaction->m_monsterIndex = spawned;

		 map->SpawnObject(ObjectType::MONSTER, IntVec2(x,y), interaction);
	}
}

void MapGenerator::SpawnLoot(AbstractMap* absMap, Map* map, int spawnNum /*= 5*/)
{
	for (int spawned = 0; spawned < spawnNum; spawned++) {
		bool sucess = false;
		TileHeatMap* mainMap = CreateMainPathHeatMap(absMap, map);
		TileHeatMap* splitMap = CreateSplitPathHeatMap(absMap, map);
		TileHeatMap* lootMap = CreateTreasureHeatMap(map);
		TileHeatMap* openMap = CreateOpenTerrainHeatMap(map);
		
		std::vector<float> weightList;
		for (int y = 0; y < absMap->m_dimensions.y; y++) {
			for (int x = 0; x < absMap->m_dimensions.x; x++) {
				weightList.push_back(GetLootSpawnChanceAtPos(mainMap, splitMap, lootMap, openMap, map, IntVec2(x, y)));
			}
		}
		int index = GetRandomIndexFromWeightedList(weightList, absMap->m_dimensions.x, absMap->m_dimensions.y, spawned, absMap->m_index);

		int x = index % absMap->m_dimensions.x;
		int y = (index - x) / absMap->m_dimensions.x;

		map->SpawnObject(ObjectType::CHEST, IntVec2(x, y));
	}
}

AbstractMap* MapGenerator::CreateWithBSP(int index)
{
    
    AbstractMap* map = m_maps[index];
    Room* mainRoom = new Room();
    mainRoom->m_constraints = AABB2(Vec2(2.f,2.f), Vec2((float)map->m_dimensions.x - 3.f, (float)map->m_dimensions.y - 3.f));
    mainRoom->m_mapIndex = index;

    std::vector<Room*> rooms;
    rooms.push_back(mainRoom);

    for (int i = 0; i<m_dungeonMetrics[index]->m_roomPerMap; i++)
    {
       // CutRoom();
       int rand = Noise4DGetRandomIntInRange(m_dungeonMetrics[index]->m_dimensions.x, m_dungeonMetrics[index]->m_dimensions.y, i, index, 0, (int)rooms.size() - 1);
       
       int tries = 0;
       while (!RoomCuttable(rooms[rand])) {
           rand = Noise4DGetRandomIntInRange(m_dungeonMetrics[index]->m_dimensions.x, m_dungeonMetrics[index]->m_dimensions.y, i*tries, index, 0, (int)rooms.size() - 1);
           tries++;
       }

       CutRoom(rooms, rand,2);
    }

    map->m_rooms = rooms;

    
    return map;
}

void MapGenerator::CutRoom(std::vector<Room*>& rooms, int index, int edge)
{
    Room* roomA = new Room();
    Room* roomB = new Room();

    roomA->m_eastRoom = rooms[index]->m_eastRoom;
    roomA->m_westRoom = rooms[index]->m_westRoom;
    roomA->m_southRoom = rooms[index]->m_southRoom;
	roomA->m_northRoom = rooms[index]->m_northRoom;

	roomB->m_eastRoom = rooms[index]->m_eastRoom;
	roomB->m_westRoom = rooms[index]->m_westRoom;
	roomB->m_southRoom = rooms[index]->m_southRoom;
	roomB->m_northRoom = rooms[index]->m_northRoom;

    AABB2 roomAConstraint = rooms[index]->m_constraints;
    AABB2 roomBConstraint = rooms[index]->m_constraints;

    ClearRoomAdjacency(rooms[index]);
    rooms.erase(rooms.begin() + index);

    bool verticalCut = (roomAConstraint.m_maxs.x - roomAConstraint.m_mins.x > roomAConstraint.m_maxs.y - roomAConstraint.m_mins.y);

    if (verticalCut) {
        int cutPoint=(int)(roomAConstraint.m_mins.x/2.f) + (int)(roomAConstraint.m_maxs.x/2.f) - 1;
        roomAConstraint.m_maxs.x = (float)(cutPoint - edge);
        roomBConstraint.m_mins.x = (float)(cutPoint + edge);


		roomA->m_eastRoom.clear();
		roomB->m_westRoom.clear();

        roomA->m_eastRoom.push_back(roomB);
        roomB->m_westRoom.push_back(roomA);
        
        for (Room* room : roomA->m_northRoom) {
            room->m_southRoom.push_back(roomA);
        }

		for (Room* room : roomA->m_southRoom) {
		    room->m_northRoom.push_back(roomA);
		}

		for (Room* room : roomA->m_westRoom) {
            room->m_eastRoom.push_back(roomA);
		}
		
		for (Room* room : roomB->m_northRoom) {
			room->m_southRoom.push_back(roomB);
		}

		for (Room* room : roomB->m_southRoom) {
			room->m_northRoom.push_back(roomB);
		}

		for (Room* room : roomB->m_eastRoom) {
			room->m_westRoom.push_back(roomB);
		}
		
    }
    else {
        int cutPoint=(int)(roomAConstraint.m_mins.y/2.f) + (int)(roomAConstraint.m_maxs.y/2.f) - 1;
		roomAConstraint.m_maxs.y = (float)(cutPoint - edge);
		roomBConstraint.m_mins.y = (float)(cutPoint + edge);

		roomA->m_northRoom.clear();
		roomB->m_southRoom.clear();

        roomA->m_northRoom.push_back(roomB);
        roomB->m_southRoom.push_back(roomA);

		for (Room* room : roomA->m_eastRoom) {
			room->m_westRoom.push_back(roomA);
		}

		for (Room* room : roomA->m_southRoom) {
			room->m_northRoom.push_back(roomA);
		}

		for (Room* room : roomA->m_westRoom) {
			room->m_eastRoom.push_back(roomA);
		}

		for (Room* room : roomB->m_northRoom) {
			room->m_southRoom.push_back(roomB);
		}

		for (Room* room : roomB->m_eastRoom) {
			room->m_westRoom.push_back(roomB);
		}

		for (Room* room : roomB->m_westRoom) {
			room->m_eastRoom.push_back(roomB);
		}
    }

    roomA->m_constraints = roomAConstraint;
    roomB->m_constraints = roomBConstraint;

	RemoveUnalignedNeighbor(roomA);
	RemoveUnalignedNeighbor(roomB);

	rooms.push_back(roomA);
	rooms.push_back(roomB);
}

std::vector<Map*> MapGenerator::CreateFullDungeon(FullDungeonMatrix matrix)
{
    m_dungeonMetrics = matrix.m_dungeonMatrix;

    m_maps.clear();
	m_maps.resize(matrix.m_dungeonMatrix.size());

	for (int i = 0; i < matrix.m_dungeonMatrix.size(); i++) {
		/*if (m_dungeonMetrics[i]->m_roomPerMap < m_dungeonMetrics[i]->m_pathLen) {
			ERROR_AND_DIE("Invalid Path Length");
		}*/

		AbstractMap* map = new AbstractMap();
		m_maps[i] = map;
		map->m_dimensions = IntVec2(m_dungeonMetrics[i]->m_dimensions.x, m_dungeonMetrics[i]->m_dimensions.y);
		map->m_index = i;
		map->m_mainPath = new Path();
	}

	m_dungeonMetrics = matrix.m_dungeonMatrix;
	//Create Rooms
	for (int i = 0; i < matrix.m_dungeonMatrix.size(); i++) {
		
		m_maps[i] = CreateWithBSP(i);
	     
	}
	//-------------------------------
	
	IntVec2 center = GetRoomCenter(m_maps[0]->m_rooms[0]);
	for (int i = 0; i < (int)m_maps.size(); i++) {
	    


		int reTry = 0;
		IntVec2 result(-1,-1);

		while (result == IntVec2(-1, -1)) {
			
			result = GenerateTunnelingPathForBSPMap(m_maps[i], center, m_dungeonMetrics[i]->m_pathLen,reTry);

		    
			reTry++;
		}
		
		for (Room* room : m_maps[i]->m_rooms) {
			if (room->m_visited) {
				room->m_isMainRoom = true;
			}
		}

		center = result;

		if (matrix.m_dungeonMatrix[i]->m_method == GenerationMethod::BINARY_SPACE_PARTITION) {
			GenerateSplitPathForBPS(m_maps[i]);
		}
		else if (matrix.m_dungeonMatrix[i]->m_method == GenerationMethod::CELLULAR_AUTOMATA) {
			GenerateSplitPathForCave(m_maps[i]);
		}
	}
	//-----------------------------

	std::vector<Map*> realMaps;

	realMaps = AbstractMapToRealMaps();

	for (int i = 0; i < realMaps.size(); i++) {
	    SpawnLoot(m_maps[i], realMaps[i], m_dungeonMetrics[i]->m_lootAmount);
		SpawnEnemies(m_maps[i], realMaps[i], m_dungeonMetrics[i]->m_enemyAmount);
	}

	//______________________________________

	Object* player = realMaps[0]->SpawnObject(ObjectType::PLAYER, GetMapBegin(m_maps[0]));
	realMaps[0]->OnEnter(player, GetMapBegin(m_maps[0]));

	return realMaps;
}

void MapGenerator::SpawnStair(Map* mapA, Map* mapB, IntVec2 pos)
{
		StairInteraction* stairDown = new StairInteraction(mapB, pos);
		mapA->SpawnObject(ObjectType::STAIR, pos, stairDown);

		StairInteraction* stairUp = new StairInteraction(mapA, pos);
		mapB->SpawnObject(ObjectType::STAIR, pos, stairUp);


}

void MapGenerator::CreateRoomForRealmap(Room* room, Map* map, int noiseIndex)
{

	if (room->m_replaced == ReplacementRule::SQUARE) {
		for (int y = (int)room->m_constraints.m_mins.y; y <= (int)room->m_constraints.m_maxs.y; y++) {
			for (int x = (int)room->m_constraints.m_mins.x; x <= (int)room->m_constraints.m_maxs.x; x++) {

				if (y == (int)room->m_constraints.m_mins.y || y == (int)room->m_constraints.m_maxs.y
					|| x == (int)room->m_constraints.m_mins.x || x == (int)room->m_constraints.m_maxs.x) {
					map->m_tiles[x + y * map->m_dimensions.x].m_type = TileType::WALL;
				}
				else {
					map->m_tiles[x + y * map->m_dimensions.x].m_type = TileType::FLOOR;
				}

			}
		}
	}

	if (room->m_replaced == ReplacementRule::DRUNKARD) {
		int length = (room->m_constraints.m_maxs.y - room->m_constraints.m_mins.y)*(room->m_constraints.m_maxs.x - room->m_constraints.m_mins.x);
		length = (int)floorf(length*1.5f);
		IntVec2 pos = GetRoomCenter(room);
		for (int i = 0; i < length; i++) {
		    int index = pos.x + pos.y * map->m_dimensions.x;
		    map->m_tiles[index].m_type = TileType::FLOOR;
			std::vector<IntVec2> possiblePos;

			if (pos.x + 1 < room->m_constraints.m_maxs.x) {
				possiblePos.push_back(IntVec2(pos.x + 1, pos.y));
			}

			if (pos.x - 1 > room->m_constraints.m_mins.x) {
				possiblePos.push_back(IntVec2(pos.x - 1, pos.y));
			}

			if (pos.y + 1 < room->m_constraints.m_maxs.y) {
				possiblePos.push_back(IntVec2(pos.x, pos.y + 1));
			}

			if (pos.y - 1 < room->m_constraints.m_mins.y) {
				possiblePos.push_back(IntVec2(pos.x, pos.y - 1));
			}

			int idx = Noise4DGetRandomIntInRange(i, pos.x, pos.y, noiseIndex, 0, possiblePos.size()-1);

			pos = possiblePos[idx];
		}
	}
	
	
}

void MapGenerator::CreatePathForRealMap(Path* path, Map* map, TileType type)
{
	for (PathSegment* seg : path->m_path) {
		
		IntVec2 currentPos = seg->m_begin;
		map->m_tiles[currentPos.x + currentPos.y*map->m_dimensions.x].m_type = type;
		
		while (currentPos != seg->m_end) {

			if (currentPos.x < seg->m_end.x) {
				currentPos.x++;
			}

			if (currentPos.x > seg->m_end.x) {
				currentPos.x--;
			}

			if (currentPos.y < seg->m_end.y) {
				currentPos.y++;
			}

			if (currentPos.y > seg->m_end.y) {
				currentPos.y--;
			}

			map->m_tiles[currentPos.x + currentPos.y*map->m_dimensions.x].m_type = type;

		}


	}
}

bool MapGenerator::GetIsBorder(TileHeatMap* map, IntVec2 pos)
{
    std::vector<int> adj;

	if (pos.x - 1 >= 0) {

	    adj.push_back(map->GetHeat(IntVec2(pos.x-1, pos.y)));
		if (pos.y - 1 >= 0) {
		    adj.push_back(map->GetHeat(IntVec2(pos.x-1, pos.y-1)));
		}
		if (pos.y + 1 < map->m_dimensions.y) {
		    adj.push_back(map->GetHeat(IntVec2(pos.x-1, pos.y+1)));
		}
	}

	if (pos.x + 1 < map->m_dimensions.x) {
	    adj.push_back(map->GetHeat(IntVec2(pos.x+1, pos.y)));
	    if (pos.y - 1 >= 0) {
		    adj.push_back(map->GetHeat(IntVec2(pos.x+1, pos.y-1)));
		}
		if (pos.y + 1 < map->m_dimensions.y) {
		    adj.push_back(map->GetHeat(IntVec2(pos.x+1, pos.y+1)));
		}
	}

	if (pos.y - 1 >= 0) {
		adj.push_back(map->GetHeat(IntVec2(pos.x, pos.y - 1)));
	}
	if (pos.y + 1 < map->m_dimensions.y) {
		adj.push_back(map->GetHeat(IntVec2(pos.x, pos.y + 1)));
	}

	for (int adjHeat : adj) {
		
		if ((adjHeat == map->GetHeat(pos) - 1)) {
			int same = 0;
			int same2 = 0;
			for (int otherHeat : adj) {
				if (otherHeat == adjHeat) {
					same++;
				}
				if (otherHeat == adjHeat + 1) {
				    same2 ++;
				}
			}
			if (same >= 4) {
				return true;
			}
			if (same2 >= 5) {
				return true;
			}
		}
		
	}
	return false;
}

void MapGenerator::AddGrass(Map* map, TileType floor, TileType grass, int noiseIndex)
{
	std::vector<bool> grid;
	for (int y = 0; y < map->m_dimensions.y; y++) {
		for (int x = 0; x < map->m_dimensions.x; x++) {
			//newMap->m_tiles[x + y* newMap->m_dimensions.x].m_type = TileType::FLOOR;
			grid.push_back(Noise4DGetRandomIntInRange(x, y, noiseIndex, 0, 0, 1));
		}
	}

	std::vector<int> born{ 6, 7, 8 };
	std::vector<int> survive{ 3, 4, 5, 6, 7, 8 };
	CellularAutomataGrid(grid, IntVec2(map->m_dimensions.x, map->m_dimensions.y), born, survive);

	for (int y = 0; y < map->m_dimensions.y; y++) {
		for (int x = 0; x < map->m_dimensions.x; x++) {
			if (map->m_tiles[x + y * map->m_dimensions.x].m_type == floor&& grid[x + y* map->m_dimensions.x]) {
			    map->m_tiles[x + y * map->m_dimensions.x].m_type = grass;
			}
		}
	}
}

IntVec2 MapGenerator::GetRoomCenter(Room* room)
{
    return IntVec2((int)room->m_constraints.GetCenter().x, (int)room->m_constraints.GetCenter().y);
}

IntVec2 MapGenerator::GetMapBegin(AbstractMap* map)
{
    return GetRoomCenter(map->m_rooms[GetNearestRoomIndex(map->m_rooms, map->m_mainPath->m_path[0]->m_begin)]);
}

void MapGenerator::RemoveUnalignedNeighbor(Room* room)
{
    std::vector<Room*> adjRooms = GetAdjRooms(room);

	for (Room* adjRoom : adjRooms) {
		if (!IsRoomAlighed(room, adjRoom)) {
		    RemoveAdjacency(room, adjRoom);
		}
	}
}

void MapGenerator::RemoveAdjacency(Room* roomA, Room* roomB)
{

	for (int i = 0; i < (int)roomA->m_eastRoom.size(); i++) {
		if (roomA->m_eastRoom[i] == roomB) {
			roomA->m_eastRoom.erase(roomA->m_eastRoom.begin() + i);

			for (int j = 0; j < (int)roomB->m_westRoom.size(); j++) {
				if (roomB->m_westRoom[j] == roomA) {
					roomB->m_westRoom.erase(roomB->m_westRoom.begin() + j);
					break;
				}
			}
			return;
		}
	}

	for (int i = 0; i < (int)roomA->m_eastRoom.size(); i++) {
		if (roomA->m_eastRoom[i] == roomB) {
			roomA->m_eastRoom.erase(roomA->m_eastRoom.begin() + i);

			for (int j = 0; j < (int)roomB->m_westRoom.size(); j++) {
				if (roomB->m_westRoom[j] == roomA) {
					roomB->m_westRoom.erase(roomB->m_westRoom.begin() + j);
					break;
				}
			}
			return;
		}
	}

	for (int i = 0; i < (int)roomA->m_northRoom.size(); i++) {
		if (roomA->m_northRoom[i] == roomB) {
			roomA->m_northRoom.erase(roomA->m_northRoom.begin() + i);

			for (int j = 0; j < (int)roomB->m_southRoom.size(); j++) {
				if (roomB->m_southRoom[j] == roomA) {
					roomB->m_southRoom.erase(roomB->m_southRoom.begin() + j);
					break;
				}
			}
			return;
		}
	}

	for (int i = 0; i < (int)roomA->m_southRoom.size(); i++) {
		if (roomA->m_southRoom[i] == roomB) {
			roomA->m_southRoom.erase(roomA->m_southRoom.begin() + i);

			for (int j = 0; j < (int)roomB->m_northRoom.size(); j++) {
				if (roomB->m_northRoom[j] == roomA) {
					roomB->m_northRoom.erase(roomB->m_northRoom.begin() + j);
					break;
				}
			}
			return;
		}
	}
}

//Create a straight path connecting 2 adjacent rooms
PathSegment* MapGenerator::CreatePathSegment(Room* from, Room* to)
{    
    int sharedMax = 0;
    int sharedMin = 0;
    if (from->m_constraints.m_mins.y > to->m_constraints.m_maxs.y || from->m_constraints.m_maxs.y< to->m_constraints.m_mins.y) {
        
        sharedMax = (int)from->m_constraints.m_maxs.x;
        if ((int)to->m_constraints.m_maxs.x < sharedMax) {
            sharedMax = (int)to->m_constraints.m_maxs.x;
        }

		sharedMin = (int)from->m_constraints.m_mins.x;
		if ((int)to->m_constraints.m_mins.x > sharedMin) {
			sharedMin = (int)to->m_constraints.m_mins.x;
		}

    }
    else {
		sharedMax = (int)from->m_constraints.m_maxs.y;
		if ((int)to->m_constraints.m_maxs.y < sharedMax) {
			sharedMax = (int)to->m_constraints.m_maxs.y;
		}

		sharedMin = (int)from->m_constraints.m_mins.y;
		if ((int)to->m_constraints.m_mins.y > sharedMin) {
			sharedMin = (int)to->m_constraints.m_mins.y;
		}
    }

    int pathPos = sharedMin + (sharedMax - sharedMin)/2;

    PathSegment* result = new PathSegment();

    if (from->m_constraints.m_maxs.x < to->m_constraints.m_mins.x ) {
        result->m_begin = IntVec2((int)from->m_constraints.m_maxs.x, pathPos);
        result->m_end = IntVec2((int)to->m_constraints.m_mins.x, pathPos);
    }

	if (from->m_constraints.m_mins.x > to->m_constraints.m_maxs.x) {
		result->m_begin = IntVec2((int)from->m_constraints.m_mins.x, pathPos);
		result->m_end = IntVec2((int)to->m_constraints.m_maxs.x, pathPos);
	}
    
	if (from->m_constraints.m_maxs.y < to->m_constraints.m_mins.y) {
		result->m_begin = IntVec2(pathPos, (int)from->m_constraints.m_maxs.y);
		result->m_end = IntVec2(pathPos, (int)to->m_constraints.m_mins.y);
	}

	if (from->m_constraints.m_mins.y > to->m_constraints.m_maxs.y ) {
		result->m_begin = IntVec2(pathPos, (int)from->m_constraints.m_mins.y);
		result->m_end = IntVec2(pathPos, (int)to->m_constraints.m_maxs.y);
	}

    return result;
}

std::vector<PathSegment*> MapGenerator::CreatePathSegmentForCave(Room* from, Room* to)
{
    
	int sharedMax = 0;
	int sharedMin = 0;
	if (from->m_constraints.m_mins.y > to->m_constraints.m_maxs.y || from->m_constraints.m_maxs.y < to->m_constraints.m_mins.y) {

		sharedMax = (int)from->m_constraints.m_maxs.x;
		if ((int)to->m_constraints.m_maxs.x < sharedMax) {
			sharedMax = (int)to->m_constraints.m_maxs.x;
		}

		sharedMin = (int)from->m_constraints.m_mins.x;
		if ((int)to->m_constraints.m_mins.x > sharedMin) {
			sharedMin = (int)to->m_constraints.m_mins.x;
		}

	}
	else {
		sharedMax = (int)from->m_constraints.m_maxs.y;
		if ((int)to->m_constraints.m_maxs.y < sharedMax) {
			sharedMax = (int)to->m_constraints.m_maxs.y;
		}

		sharedMin = (int)from->m_constraints.m_mins.y;
		if ((int)to->m_constraints.m_mins.y > sharedMin) {
			sharedMin = (int)to->m_constraints.m_mins.y;
		}
	}

	int pathPos = sharedMin + (sharedMax - sharedMin) / 2;
	
	std::vector<PathSegment*> result;

	PathSegment* seg1 = new PathSegment();
	PathSegment* seg2 = new PathSegment();
	PathSegment* seg3 = new PathSegment();

	IntVec2 fromCenter = GetRoomCenter(from);
    IntVec2 toCenter = GetRoomCenter(to);

	seg1->m_begin = fromCenter;

	seg3->m_end = toCenter;


	if (from->m_constraints.m_maxs.x < to->m_constraints.m_mins.x) {
		seg2->m_begin = IntVec2(fromCenter.x, pathPos);
		seg2->m_end = IntVec2((int)toCenter.x, pathPos);
	}

	if (from->m_constraints.m_mins.x > to->m_constraints.m_maxs.x) {
		seg2->m_begin = IntVec2((int)fromCenter.x, pathPos);
		seg2->m_end = IntVec2((int)toCenter.x, pathPos);
	}

	if (from->m_constraints.m_maxs.y < to->m_constraints.m_mins.y) {
		seg2->m_begin = IntVec2(pathPos, (int)fromCenter.y);
		seg2->m_end = IntVec2(pathPos, (int)toCenter.y);
	}

	if (from->m_constraints.m_mins.y > to->m_constraints.m_maxs.y) {
		seg2->m_begin = IntVec2(pathPos, (int)fromCenter.y);
		seg2->m_end = IntVec2(pathPos, (int)toCenter.y);
	}

	seg1->m_end = seg2->m_begin;
	seg3->m_begin = seg2->m_end;

	result.push_back(seg1);
	result.push_back(seg2);
	result.push_back(seg3);

	return result;
}

void MapGenerator::ClearRoomAdjacency(Room* room)
{
    

    for (Room* adjRoom : room->m_eastRoom) {
        for (int i = 0; i<(int)adjRoom->m_westRoom.size(); i++) {
            if (adjRoom->m_westRoom[i] == room) {
                adjRoom->m_westRoom.erase(adjRoom->m_westRoom.begin()+i);
                break;
            }
        }
    }
    room->m_eastRoom.clear();

	for (Room* adjRoom : room->m_westRoom) {
		for (int i = 0; i < (int)adjRoom->m_eastRoom.size(); i++) {
			if (adjRoom->m_eastRoom[i] == room) {
				adjRoom->m_eastRoom.erase(adjRoom->m_eastRoom.begin() + i);
				break;
			}
		}
	}
	room->m_westRoom.clear();


	for (Room* adjRoom : room->m_northRoom) {
		for (int i = 0; i < (int)adjRoom->m_southRoom.size(); i++) {
			if (adjRoom->m_southRoom[i] == room) {
				adjRoom->m_southRoom.erase(adjRoom->m_southRoom.begin() + i);
				break;
			}
		}
	}
	room->m_northRoom.clear();

	for (Room* adjRoom : room->m_southRoom) {
		for (int i = 0; i < (int)adjRoom->m_northRoom.size(); i++) {
			if (adjRoom->m_northRoom[i] == room) {
				adjRoom->m_northRoom.erase(adjRoom->m_northRoom.begin() + i);
				break;
			}
		}
	}
	room->m_southRoom.clear();
}

bool MapGenerator::IsRoomAlighed(Room* roomA, Room* roomB)
{
    return !((roomA->m_constraints.m_maxs.x < roomB->m_constraints.m_mins.x || roomA->m_constraints.m_mins.x > roomB->m_constraints.m_maxs.x)
           &&(roomA->m_constraints.m_maxs.y < roomB->m_constraints.m_mins.y || roomA->m_constraints.m_mins.y > roomB->m_constraints.m_maxs.y));
}


std::vector<Room*> MapGenerator::LookAheadPossibleRooms(Room* from)
{
    std::vector<Room*> result;

	for (Room* adjRoom : from->m_eastRoom) {
	    
		for (Room* aheadRoom : adjRoom->m_eastRoom) {
			if (!aheadRoom->m_visited) {
			    result.push_back(adjRoom);
			}
		}
	}
	return result;
}

IntVec2 MapGenerator::GenerateTunnelingPathForBSPMap(AbstractMap* map, IntVec2 startPos, int pathLen, int noiseVariance)
{

    map->m_mainPath->m_path.clear();

	for (Room* room : map->m_rooms) {
		room->m_visited = false;
	}

    Room* currentRoom = map->m_rooms[GetNearestRoomIndex(map->m_rooms, startPos)];

	IntVec2 endPos = GetRoomCenter(currentRoom);

	PathSegment* seg1 = new PathSegment();

	seg1->m_begin = startPos;
	seg1->m_end = IntVec2(startPos.x, endPos.y);

	PathSegment* seg2 = new PathSegment();

	seg2->m_begin = IntVec2(startPos.x, endPos.y);

	seg2->m_end = endPos;

	map->m_mainPath->m_path.push_back(seg1);
	map->m_mainPath->m_path.push_back(seg2);

	currentRoom->m_visited = true;

	std::vector<Room*> possibleRooms;

	for (Room* adjRoom : GetAdjRooms(currentRoom)) {
		if (!adjRoom->m_visited) {
			for (Room* ahead : GetAdjRooms(adjRoom)) {
				if (!ahead->m_visited)
					possibleRooms.push_back(adjRoom);
			}
		}
		
	}

	for (int i = 0; i < pathLen; i++) {
		if (possibleRooms.empty()) {
		    //Failed to generate
			return IntVec2(-1,-1);
		}

		int noise = Noise4DGetRandomIntInRange(GetRoomCenter(currentRoom).x, GetRoomCenter(currentRoom).y, map->m_index, noiseVariance, 0, (int)possibleRooms.size()-1);

		Room* nextRoom = possibleRooms[noise];
		std::vector<PathSegment*> segs = CreatePathSegmentForCave(currentRoom,nextRoom);

		for(PathSegment* seg: segs)
		    map->m_mainPath->m_path.push_back(seg);

		currentRoom = nextRoom;
		currentRoom->m_visited = true;

		possibleRooms.clear();
		for (Room* adjRoom : GetAdjRooms(currentRoom)) {
			if (!adjRoom->m_visited) {
				for (Room* ahead : GetAdjRooms(adjRoom)) {
					if (!ahead->m_visited)
						possibleRooms.push_back(adjRoom);
				}
			}
		}
	}

	return GetRoomCenter(currentRoom);
	
}

void MapGenerator::GenerateSplitPathForBPS(AbstractMap* map)
{
    Path* splitPath = new Path();
	map->m_splitPath = splitPath;
	splitPath->m_mapIndex = map->m_index;
    bool modified = true;

	while (modified) {
	    
	    modified = false;
		for (Room* room : map->m_rooms) {
			
			if (room->m_visited) {
				
				for (Room* adjRoom : GetAdjRooms(room)) {
					
					if (!adjRoom->m_visited) {
						map->m_splitPath->m_path.push_back(CreatePathSegment(room, adjRoom));
						adjRoom->m_visited = true;
						modified = true;
					}

				}

			}

		}

	}
	
}

void MapGenerator::GenerateSplitPathForCave(AbstractMap* map)
{
	Path* splitPath = new Path();
	map->m_splitPath = splitPath;
	splitPath->m_mapIndex = map->m_index;
	bool modified = true;

	while (modified) {

		modified = false;
		for (Room* room : map->m_rooms) {

			if (room->m_visited) {

				for (Room* adjRoom : GetAdjRooms(room)) {

					if (!adjRoom->m_visited) {
					    std::vector<PathSegment*> segs = CreatePathSegmentForCave(room,adjRoom);
						for (PathSegment* seg : segs) {
						    map->m_splitPath->m_path.push_back(seg);
						}
						adjRoom->m_visited = true;
						modified = true;
					}

				}

			}

		}

	}
}

int MapGenerator::GetNearestRoomIndex(std::vector<Room*> rooms, IntVec2 pos)
{
    int len = INT_MAX;
	int result = 0;
	for (int i = 0; i < (int)rooms.size(); i++) {
	    AABB2 constraints = rooms[i]->m_constraints;

		if (constraints.IsPointInside(pos)) {
			return i;
		}

		IntVec2 center = IntVec2((int)roundf(constraints.GetCenter().x), (int)roundf(constraints.GetCenter().y));

		int roomDist = abs(center.x - pos.x) + abs(center.y - pos.y);
		if (roomDist < len) {
		    len = roomDist;
			result = i;
		}

	}

	return result;
}

std::vector<Room*> MapGenerator::GetAdjRooms(Room* room)
{
    std::vector<Room*> result;

	for (Room* aheadRoom : room->m_eastRoom) {
		result.push_back(aheadRoom);
	}

	for (Room* aheadRoom : room->m_westRoom) {
		result.push_back(aheadRoom);
	}

	for (Room* aheadRoom : room->m_northRoom) {
		result.push_back(aheadRoom);
	}

	for (Room* aheadRoom : room->m_southRoom) {
		result.push_back(aheadRoom);
	}

	return result;
}

/*Look Ahead Tunneling:
  Create Abstract Map Input: size, start, path len
  Initialize blocks
  starting block = get block from start pos;
  while less than path len
  get all valid block near current block, count their valid neighbor -1
  roll random chance, the block with more valid out are more likely to be chosen
  create path to the new place, update current block

  if no valid out, restart
*/
