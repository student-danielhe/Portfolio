#include "MapGenerator.hpp"
#include "StairInteraction.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
std::vector<IntVec2> g_directions{IntVec2(0,1), IntVec2(0,-1), IntVec2(1,0), IntVec2(-1,0)};
std::vector<Map*> MapGenerator::GenerateMaps()
{
    std::vector<Map*> result;
    int rand=0;
    for (AbstractMap* map : m_maps) {
        rand++;
        Map* newMap = new Map(map->m_dimensions, m_owner);
        result.push_back(newMap);

        //Fill with walls
        for (int y = 0; y < newMap->m_dimensions.y; y++) {
            for (int x = 0; x < newMap->m_dimensions.x; x++) {
				Tile newTile;
				newTile.m_pos = IntVec2(x, y);
				newTile.m_type = TileType::WALL;

                newMap->m_tiles.push_back(newTile);
            }
        }

        for (Room* room : map->m_rooms) {
        //square room
        
        //Automata
            std::vector<bool> grid;
            int yRange= (int)room->m_constraints.m_maxs.y - (int)(room->m_constraints.m_mins.y + 1);
            int xRange = (int)room->m_constraints.m_maxs.x - (int)(room->m_constraints.m_mins.x + 1);
            for (int y = 0; y < yRange; y++) {
				for (int x = 0; x < xRange; x++) {
                    //newMap->m_tiles[x + y* newMap->m_dimensions.x].m_type = TileType::FLOOR;
                    grid.push_back(Noise2DGetRandomIntInRange(x+rand, y+rand, 0, 1));
				}
            }

            std::vector<int> born{6, 7, 8};
            std::vector<int> survive{3, 4, 5, 6, 7, 8};
            CellularAutomataGrid(grid, IntVec2(xRange,yRange),born, survive);
			
            for (int y = (int)room->m_constraints.m_mins.y + 1; y < (int)room->m_constraints.m_maxs.y; y++) {
				for (int x = (int)room->m_constraints.m_mins.x + 1; x < (int)room->m_constraints.m_maxs.x; x++) {
					//newMap->m_tiles[x + y* newMap->m_dimensions.x].m_type = TileType::FLOOR;
                    int gridIndex = (y - ((int)room->m_constraints.m_mins.y + 1)) * (xRange) + (x - ((int)room->m_constraints.m_mins.x + 1));
                    if (grid[gridIndex]) {
                        newMap->m_tiles[x + y* newMap->m_dimensions.x].m_type = TileType::GRASS;
                    }
                    else {
                        newMap->m_tiles[x + y* newMap->m_dimensions.x].m_type = TileType::FLOOR;
                    }
				}
			}

        }

		for (Path* path : map->m_paths) {
			IntVec2 point = path->m_path[0];

			int positionIndex = point.x + point.y * newMap->m_dimensions.x;
			newMap->m_tiles[positionIndex].m_type = TileType::SAND;

			for (int i = 1; i < (int)path->m_path.size(); i++) {
				point = path->m_path[i];

				int posIdx = point.x + point.y * newMap->m_dimensions.x;
				newMap->m_tiles[posIdx].m_type = TileType::SAND;

			}
		}

    }

    

    return result;

}

MapGenerator::MapGenerator(std::vector<DungeonMetrics*> metrics):
m_dungeonMetrics(metrics)
{
    for (int i = 0; i < metrics.size(); i++) {
        
        AbstractMap* newMap = new AbstractMap();
        newMap->m_dimensions = m_dungeonMetrics[i]->m_dimensions;

        m_maps.push_back(newMap);

    }
}

void MapGenerator::CreateRooms()
{
    
    //Test
    /*Room* room1 = new Room();
    room1->m_constraints = AABB2(Vec2(2.f, 2.f), Vec2(10.f,10.f));
    room1->m_mapIndex = 0;


    Room* room2 =  new Room();
	room2->m_constraints = AABB2(Vec2(15.f, 12.f), Vec2(25.f, 18.f));
	room2->m_mapIndex = 0;
	
    AddRandomEntrances(room1);
    AddRandomEntrances(room2);

    m_maps[0]->m_rooms.push_back(room1);
    m_maps[0]->m_rooms.push_back(room2);
    CreatePath(room2, room1);*/
    m_maps[0] = CreateRoomsWithBSP(0);

}

void MapGenerator::CreateRandomPath(Room* from, Room* to)
{
    IntVec2 begin(0,0);

    IntVec2 end(0, 0);
    
    Path* newPath = new Path();

    int shortest = 9999;

    for (IntVec2 startCoord : from->m_entraces) {
        for (IntVec2 endCoord : to->m_entraces) {
            int diff = abs(startCoord.x- endCoord.x) + abs(startCoord.y - endCoord.y);

            if (diff < shortest) {
                shortest = diff;
                begin = startCoord;
                end = endCoord;
            }
        }
    }

    newPath->m_start = begin;
    newPath->m_end = end;

    //int levelDiff = begin->m_mapIndex-end->m_mapIndex;
    IntVec2 currentPos = begin;
    newPath->m_path.push_back(currentPos);
    while (currentPos!=end) {
        std::vector<IntVec2> possibleMovement;
        if (currentPos.x > end.x) {
            possibleMovement.push_back(IntVec2(-1, 0));
        }

		if (currentPos.x < end.x) {
			possibleMovement.push_back(IntVec2(1, 0));
		}

		if (currentPos.y > end.y) {
			possibleMovement.push_back(IntVec2(0, -1));
		}

		if (currentPos.y < end.y) {
			possibleMovement.push_back(IntVec2(0, 1));
		}

        currentPos = currentPos + possibleMovement[Noise2DGetRandomIntInRange(currentPos.x+from->m_mapIndex, currentPos.y+from->m_mapIndex, 0, (int)possibleMovement.size()-1)];
		IntVec2 nextPoint = currentPos;
		newPath->m_path.push_back(nextPoint);
        
    }

    m_maps[from->m_mapIndex]->m_paths.push_back(newPath);
}

void MapGenerator::CreateTunnelingPath(Room* from, Room* to, int maxWalkLen)
{
     
    std::vector<IntVec2> directions;
    directions.resize(4);
    directions[DIR_NORTH] = IntVec2( 0,  1);
    directions[DIR_SOUTH] = IntVec2( 0,  -1);
    directions[DIR_EAST]  = IntVec2( 1,  0);
    directions[DIR_WEST]  = IntVec2( -1, 0);

	IntVec2 begin(0,0);
    IntVec2 startStep;

	IntVec2 end(0,0);
    IntVec2 endStep;

	

	int shortest = 9999;

	for (int startDir = DIR_NORTH; startDir < 4; startDir++) {
        
        IntVec2 startCoord = from->m_entraces[startDir];
		
        for (int endDir = DIR_NORTH; endDir < 4; endDir++) {
            
            IntVec2 endCoord = to->m_entraces[endDir];
			int diff = abs(startCoord.x - endCoord.x) + abs(startCoord.y - endCoord.y);

			if (diff < shortest) {
				
                shortest = diff;
				
                begin = startCoord;
                startStep = startCoord + directions[startDir];
				
                end = endCoord;
                endStep = endCoord + directions[endDir];
			}
		}
	}
    Path* newPath = CreateTunnelingPath(startStep,endStep, from->m_mapIndex, maxWalkLen);
	newPath->m_start = begin;
	newPath->m_end = end;


	
    newPath->m_path.push_back(newPath->m_start);
	
    newPath->m_path.push_back(newPath->m_end);

	m_maps[from->m_mapIndex]->m_paths.push_back(newPath);

}

Path* MapGenerator::CreateTunnelingPath(IntVec2 from, IntVec2 to, int noiseIndex /*= 0*/, int maxWalkLen /*= 3*/)
{
    Path* newPath = new Path();
    newPath->m_start = from;
    newPath->m_end = to;
	IntVec2 currentPos = from;
	IntVec2 startPoint = currentPos;
	while (currentPos != to) {

		std::vector<IntVec2> possibleMovement;
		std::vector<int> maxSteps;
		if (currentPos.x > to.x) {
			possibleMovement.push_back(IntVec2(-1, 0));
			maxSteps.push_back(currentPos.x - to.x);
		}

		if (currentPos.x < to.x) {
			possibleMovement.push_back(IntVec2(1, 0));
			maxSteps.push_back(to.x - currentPos.x);
		}

		if (currentPos.y > to.y) {
			possibleMovement.push_back(IntVec2(0, -1));
			maxSteps.push_back(currentPos.y - to.y);
		}

		if (currentPos.y < to.y) {
			possibleMovement.push_back(IntVec2(0, 1));
			maxSteps.push_back(to.y - currentPos.y);
		}

		int finalDirection = Noise2DGetRandomIntInRange(currentPos.x + noiseIndex, currentPos.y + noiseIndex, 0, (int)possibleMovement.size() - 1);//[g_RNG->RollRandomIntLessThan(possibleMovement.size())];
		int finalLen = Noise2DGetRandomIntInRange(currentPos.x + 1 + noiseIndex, currentPos.y + 1 + noiseIndex, 1, maxSteps[finalDirection]);
		if (finalLen > maxWalkLen) {
			finalLen = maxWalkLen;
		}

		for (int i = 0; i <= finalLen; i++) {
			IntVec2 point = currentPos + IntVec2(possibleMovement[finalDirection].x * i, possibleMovement[finalDirection].y * i);

			newPath->m_path.push_back(point);
		}

		currentPos = currentPos + IntVec2(possibleMovement[finalDirection].x * finalLen, possibleMovement[finalDirection].y * finalLen);

	}

    return newPath;
}

void MapGenerator::AddRandomEntrances(Room* room)
{
    //Up
    IntVec2 UpEntrance(Noise2DGetRandomIntInRange((int)room->m_constraints.m_mins.x +room->m_mapIndex, (int)room->m_constraints.m_mins.y+room->m_mapIndex, (int)room->m_constraints.m_mins.x + 1, (int)room->m_constraints.m_maxs.x - 1), (int)room->m_constraints.m_maxs.y);

    //Down
    IntVec2 DownEntrance(Noise2DGetRandomIntInRange((int)room->m_constraints.m_mins.x+room->m_mapIndex, (int)room->m_constraints.m_mins.y+room->m_mapIndex, (int)room->m_constraints.m_mins.x + 1, (int)room->m_constraints.m_maxs.x - 1), (int)room->m_constraints.m_mins.y);

    //Left
    IntVec2 LeftEntrance((int)room->m_constraints.m_mins.x, Noise2DGetRandomIntInRange((int)room->m_constraints.m_mins.x+room->m_mapIndex, (int)room->m_constraints.m_mins.y+room->m_mapIndex, (int)room->m_constraints.m_mins.y + 1, (int)room->m_constraints.m_maxs.y - 1));

    //Right
    IntVec2 RightEntrance((int)room->m_constraints.m_maxs.x, Noise2DGetRandomIntInRange((int)room->m_constraints.m_mins.x+room->m_mapIndex, (int)room->m_constraints.m_mins.y+room->m_mapIndex, (int)room->m_constraints.m_mins.y + 1, (int)room->m_constraints.m_maxs.y - 1));

	room->m_entraces.resize(4);
	room->m_entraces[DIR_NORTH] = UpEntrance;
	room->m_entraces[DIR_SOUTH] = DownEntrance;
	room->m_entraces[DIR_EAST] = RightEntrance;
	room->m_entraces[DIR_WEST] = LeftEntrance;
}

void MapGenerator::AddFixedEntrances(Room* room)
{
	//Up
	IntVec2 UpEntrance((int)(room->m_constraints.m_mins.x + room->m_constraints.m_maxs.x)/2, (int)room->m_constraints.m_maxs.y);

	//Down
	IntVec2 DownEntrance((int)(room->m_constraints.m_mins.x + room->m_constraints.m_maxs.x)/2, (int)room->m_constraints.m_mins.y);

	//Left
	IntVec2 LeftEntrance((int)room->m_constraints.m_mins.x, (int)(room->m_constraints.m_mins.y + room->m_constraints.m_maxs.y)/2);

	//Right
	IntVec2 RightEntrance((int)room->m_constraints.m_maxs.x, (int)(room->m_constraints.m_mins.y + room->m_constraints.m_maxs.y)/2);
    
    room->m_entraces.resize(4);
    room->m_entraces[DIR_NORTH] = UpEntrance;
    room->m_entraces[DIR_SOUTH] = DownEntrance;
    room->m_entraces[DIR_EAST] = RightEntrance;
    room->m_entraces[DIR_WEST] = LeftEntrance;


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

AbstractMap* MapGenerator::CreateRoomsWithBSP(int index)
{
    AbstractMap* map = new AbstractMap();
    m_maps[index] = map;
    map->m_dimensions = m_dungeonMetrics[index]->m_dimensions;
    RoomBST* root = new RoomBST();
    Room* mainRoom = new Room();
    mainRoom->m_constraints = AABB2(Vec2(2.f,2.f), Vec2((float)map->m_dimensions.x - 3.f, (float)map->m_dimensions.y - 3.f));
    mainRoom->m_mapIndex = index;
    root->room = mainRoom;
    for (int i = 0; i<m_dungeonMetrics[index]->m_roomPerMap; i++)
    {
       // CutRoom();
       std::vector<RoomBST*> rooms = GetAllChildren(root);

       RoomBST* rand = rooms[Noise2DGetRandomIntInRange(i+m_dungeonMetrics[index]->m_dimensions.x, index, 0, (int)rooms.size() - 1)];
       int tries = 0;
       while (!RoomCuttable(rand->room)) {
           rand = rooms[Noise2DGetRandomIntInRange(i+index, tries+index, 0, (int)rooms.size() - 1)];
           tries++;
       }
       CutRoom(rand,2);
    }

    std::vector<RoomBST*> rooms = GetAllChildren(root);
    std::vector<Room*> mapRoom;
    for (RoomBST* roomBST : rooms) {
        AddFixedEntrances(roomBST->room);
        mapRoom.push_back(roomBST->room);
    }
    map->m_rooms = mapRoom;

    CreatePathWithBSP(root);
    
    return map;
}

void MapGenerator::CutRoom(RoomBST* original, int shrink)
{
    bool horitonzalCut = false;
	
    

    AABB2 roomAConstraint = original->room->m_constraints;
    AABB2 roomBConstraint = original->room->m_constraints;

    if (roomAConstraint.m_maxs.x - roomAConstraint.m_mins.x > roomAConstraint.m_maxs.y - roomAConstraint.m_mins.y) {
        horitonzalCut = true;
    }
    if (horitonzalCut) {
        int cutPoint=(int)(roomAConstraint.m_mins.x/2.f) + (int)(roomAConstraint.m_maxs.x/2.f) - 1;
        roomAConstraint.m_maxs.x = (float)(cutPoint - shrink);
        roomBConstraint.m_mins.x = (float)(cutPoint + shrink);
    }
    else {
        int cutPoint=(int)(roomAConstraint.m_mins.y/2.f) + (int)(roomAConstraint.m_maxs.y/2.f) - 1;
		roomAConstraint.m_maxs.y = (float)(cutPoint - shrink);
		roomBConstraint.m_mins.y = (float)(cutPoint + shrink);
    }

    RoomBST* roomBSTA = new RoomBST();
	Room* roomA = new Room();
    roomA->m_mapIndex = original->room->m_mapIndex;
	roomA->m_constraints = roomAConstraint;
	roomBSTA->room = roomA;

	RoomBST* roomBSTB = new RoomBST();
	Room* roomB = new Room();
	roomB->m_mapIndex = original->room->m_mapIndex;
	roomB->m_constraints = roomBConstraint;
	roomBSTB->room = roomB;

    original->room= nullptr;
    original->childA = roomBSTA;
    original->childB = roomBSTB;
}

void MapGenerator::CreatePathWithBSP(RoomBST* root)
{
    if (root->childA->childA) {
        CreatePathWithBSP(root->childA);
    }
    if (root->childB->childA) {
        CreatePathWithBSP(root->childB);
    }
    std::vector<RoomBST*> AChildren = GetAllChildren(root->childA);
    std::vector<RoomBST*> BChildren = GetAllChildren(root->childB);
    int index = AChildren[0]->room->m_mapIndex;
    RoomBST* RandomAChildren = AChildren[Noise2DGetRandomIntInRange((int)AChildren.size(), index, 0, (int)AChildren.size() - 1)];
    RoomBST* RandomBChildren = BChildren[Noise2DGetRandomIntInRange((int)BChildren.size(), index, 0, (int)BChildren.size() - 1)];

    CreateTunnelingPath(RandomAChildren->room, RandomBChildren->room, 3);
}



std::vector<RoomBST*> MapGenerator::GetAllChildren(RoomBST* root)
{
    if (root->childA&&root->childB) {
        std::vector<RoomBST*> A = GetAllChildren(root->childA);
        std::vector<RoomBST*> B = GetAllChildren(root->childB);
        std::vector<RoomBST*> AB;
		AB.reserve(A.size() + B.size()); // preallocate memory
		AB.insert(AB.end(), A.begin(), A.end());
		AB.insert(AB.end(), B.begin(), B.end());
        return AB;
    }
    else {
        std::vector<RoomBST*> result;
        result.push_back(root);
        return result;
    }
}

std::vector<Map*> MapGenerator::CreateFullDungeon(FullDungeonMatrix matrix, std::vector<DungeonMetrics*> metrics)
{
    std::vector<Map*> realMaps;
    if (matrix.type == MapConnectionType::LINEAR) {
        
        for (int i = 0; i < metrics.size(); i++) {
            if (metrics[i]->m_method == GenerationMethod::BINARY_SPACE_PARTITION) {
                CreateRoomsWithBSP(i);
                
            }
			else if (metrics[i]->m_method == GenerationMethod::GUIDED_PATH_CELLULAR_AUTOMATA) {
				GenerateMapWithGuidedPath(i, IntVec2(5,5), 20, 20);

			}

        }
        
        realMaps = GenerateMaps();
        
        IntVec2 begin = GetRoomCenter(m_maps[0]->m_rooms[0]);
        Object* player =  realMaps[0]->SpawnObject(ObjectType::PLAYER, begin);
        realMaps[0]->OnEnter(player, player->m_position);

        TileHeatMap heatMap(IntVec2(0,0), 0.f);
        
        for (int i = 0; i < m_maps.size() - 1; i++) {
            
            IntVec2 stairPosition = realMaps[i]->GetFurthestEmptyPos(begin, heatMap);

            Map* next = realMaps[i+1];

            while (next->IsBlocked(stairPosition)) {
                stairPosition = GetNextStep(stairPosition, heatMap);
            }
            SpawnStair(realMaps[i], next, stairPosition);
            begin = stairPosition;
        }

        IntVec2 bossPosition = realMaps[m_maps.size()-1]->GetFurthestEmptyPos(begin, heatMap);
        realMaps[m_maps.size()-1]->SpawnObject(ObjectType::BOSS, bossPosition);
        return realMaps;
    }
    return realMaps;
}

void MapGenerator::SpawnStair(Map* mapA, Map* mapB, IntVec2 pos)
{
		StairInteraction* stairDown = new StairInteraction(mapB, pos);
		mapA->SpawnObject(ObjectType::STAIR, pos, stairDown);

		StairInteraction* stairUp = new StairInteraction(mapA, pos);
		mapB->SpawnObject(ObjectType::STAIR, pos, stairUp);


}

IntVec2 MapGenerator::GetRoomCenter(Room* room)
{
    return IntVec2((int)room->m_constraints.GetCenter().x, (int)room->m_constraints.GetCenter().y);
}

IntVec2 MapGenerator::GetNextStep(IntVec2 location, TileHeatMap map)
{
    int currentHeat = (int)map.GetHeat(location);
    std::vector<IntVec2> possibleMovment;
    for (IntVec2 dir : g_directions) {
        if (map.GetHeat(dir + location) < currentHeat) {
            possibleMovment.push_back(location+dir);
        }
    }

    if (possibleMovment.size() > 0) {
        int index = Noise2DGetRandomIntInRange(location.x, location.y, 0, (int)possibleMovment.size()-1);
        return possibleMovment[index];
    }
    else {
        return location;
    }
}

bool MapGenerator::DoesPathCross(Path* pathA, Path* pathB)
{
    //Ray cast for early quite
    Vec2 forward = Vec2(pathA->m_end.x-pathA->m_start.x, pathA->m_end.y-pathA->m_start.y);
    int len = forward.GetLength();
    forward.Normalize();
    RaycastResult2D raycast = RaycastVsLineSegment2D(pathA->m_start,forward, len, pathB->m_start, pathB->m_end);
    if (raycast.m_didImpact) {
        return true;
    }

    //Check one by one, ignore start and ends
    for (IntVec2 posA : pathA->m_path) {
        if (posA != pathA->m_start && posA != pathA->m_end) {
            for (IntVec2 posB : pathB->m_path) {
                if (posB != pathB->m_start && posB != pathB->m_end) {
                    if (posA == posB) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

IntVec2 MapGenerator::GuidedPathGetNextPoint(IntVec2 start, int len, int mapIndex, int retryNoise)
{
    AbstractMap* map = m_maps[mapIndex];
    IntVec2 dimensions = map->m_dimensions;
    
    int xOffset = start.x+dimensions.x;
    for (int i = 0; i<10; i++) {
		if (start.x + xOffset < 0 || start.x + xOffset >= dimensions.x) {
			xOffset = Noise2DGetRandomIntInRange(start.x + mapIndex+i + retryNoise, start.y + mapIndex+i +retryNoise, 0, 2*len)-len;
		}

		int yOffset = len - abs(xOffset);
        if (Noise2DGetRandomIntInRange(start.x + mapIndex + i + retryNoise, start.y + mapIndex + i + retryNoise, 0, 1)) {
            yOffset*=-1;
        }
        

        if (InBound(dimensions, IntVec2(xOffset, yOffset) + start)) {
            Path* next = CreateTunnelingPath(start, IntVec2(xOffset, yOffset) + start);

            bool crossed = false;
            for (Path* path : map->m_paths) {
                if (DoesPathCross(path, next)) {
                    crossed=true;
                    break;
                }
            }
            if (!crossed) {
                map->m_paths.push_back(next);
                return IntVec2(xOffset, yOffset) + start;
            }
        }
    }
    
    return IntVec2(-1, -1);

}

bool MapGenerator::InBound(IntVec2 dimensions, IntVec2 pos)
{
    return pos.x>=0&&pos.x<dimensions.x&&pos.y>=0&&pos.y<dimensions.y;
}

void MapGenerator::GenerateMapWithGuidedPath(int mapIndex, IntVec2 startPos, int amountStep, int stepLen)
{
    AbstractMap* map = new AbstractMap();
    map->m_dimensions = m_dungeonMetrics[mapIndex]->m_dimensions;
    m_maps[mapIndex] = map;
    IntVec2 currentPos = startPos;
    int fails = 0;
    for (int i = 0; i < amountStep; i++) {
        currentPos = GuidedPathGetNextPoint(currentPos,stepLen,mapIndex, fails);
        if (currentPos == IntVec2(-1, -1)) {
            currentPos = startPos;
            i=0;
            map->m_paths.clear();
            fails++;
        }
    }
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
