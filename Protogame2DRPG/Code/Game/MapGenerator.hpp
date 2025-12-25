#pragma once
#include "Map.hpp"
class Game;

enum Direction {
	DIR_NORTH,
	DIR_SOUTH,
	DIR_EAST,
	DIR_WEST
};

enum class MapConnectionType {
	LINEAR,    //always go down
	ZIG_ZAG,   //switch between maps
	MOUNTAIN,  //witch between 2 maps, overall direction up
	HALLWAYS   //1 big outdoor section connected by disconnected indoor tunnels and rooms
};

enum class GenerationMethod {
	BINARY_SPACE_PARTITION,
	CELLULAR_AUTOMATA,
};

enum class ReplacementRule {
	NO_REPLACEMENT = 0,
	SQUARE,
	DRUNKARD
};

//--------------------------------------------------------------------------------------------------------
struct DungeonMetrics {
	IntVec2 m_dimensions = IntVec2(80, 80);
	int m_roomPerMap = 10;
	int m_pathLen = 7;
	IntVec2 m_minimumRoomSize = IntVec2(3, 3);
	int m_openness = 45;
	GenerationMethod m_method = GenerationMethod::CELLULAR_AUTOMATA;
	int m_enemyAmount = 5;
	int m_lootAmount = 5;

	ReplacementRule m_replacementRule = ReplacementRule::NO_REPLACEMENT;
	int m_replaceAmount = 0;
};

struct FullDungeonMatrix {
	MapConnectionType type;
	std::vector<DungeonMetrics*> m_dungeonMatrix;
};
//--------------------------------------------------------------------------------------------------------


struct SpecificPoint {
	IntVec2 m_position;
	int m_mapIndex;
};

struct PathSegment {
	IntVec2 m_begin;
	IntVec2 m_end;
};

struct Path {
	std::vector<PathSegment*> m_path;
	int m_mapIndex;
};

struct FullPath {
	std::vector<Path*> m_mainPath;
	std::vector<Path*> m_splitPath;
};
//______________________________________________________________________________________________________



struct Room {
	AABB2 m_constraints;
	int m_mapIndex;
	std::vector<Room*> m_northRoom;
	std::vector<Room*> m_southRoom;
	std::vector<Room*> m_eastRoom;
	std::vector<Room*> m_westRoom;

	bool m_visited = false;
	bool m_isMainRoom = false;
	ReplacementRule m_replaced = ReplacementRule::NO_REPLACEMENT;
};

struct AbstractMap {
	std::vector<Room*> m_rooms;
	IntVec2 m_dimensions;
	Path* m_mainPath;
	Path* m_splitPath;
	int m_index;
};







class MapGenerator {
public:
    std::vector<Map*>CreateFullDungeon(FullDungeonMatrix matrix);
    std::vector<DungeonMetrics*> m_dungeonMetrics;

	std::vector<AbstractMap*> m_maps;




	Game* m_owner = nullptr;

	MapGenerator();



	private:
	//BST----------------------------------------------------------------------------------------------
	AbstractMap* CreateWithBSP(int index);

	bool IsRoomAlighed(Room* roomA, Room* roomB);

	void CutRoom(std::vector<Room*>& rooms, int index, int edge);
	
	bool RoomCuttable(Room* room);
	
	//Path---------------------------------------------------------------------------------------------
	PathSegment* CreatePathSegment(Room* from, Room* to);

	std::vector<PathSegment*> CreatePathSegmentForCave(Room* from, Room* to);
	
	std::vector<Room*>LookAheadPossibleRooms(Room* from);

	IntVec2 GenerateTunnelingPathForBSPMap(AbstractMap* map, IntVec2 startPos, int pathLen=10, int noiseVariance = 0);

	void GenerateSplitPathForBPS(AbstractMap* map);

	void GenerateSplitPathForCave(AbstractMap* map);
	
	//Room----------------------------------------------------------------------------------------------
	int GetNearestRoomIndex(std::vector<Room*> rooms, IntVec2 pos);

	std::vector<Room*> GetAdjRooms(Room* room);
	
	IntVec2 GetRoomCenter(Room* room);

	IntVec2 GetMapBegin(AbstractMap* map);
	
	void RemoveUnalignedNeighbor(Room* room);

	void RemoveAdjacency(Room* roomA, Room* roomB);

	void ClearRoomAdjacency(Room* room);

	//Gameplay Object------------------------------------------------------------------------------------
	void SpawnStair(Map* mapA, Map* mapB, IntVec2 pos);

	void SpawnEnemies(AbstractMap* absMap, Map* map, int spawnNum = 5);

	void SpawnLoot(AbstractMap* absMap, Map* map, int spawnNum = 5);

	//Abstract to Real Map----------------------------------------------

	std::vector<Map*> AbstractMapToRealMaps();

	Map* BSPMapToRealMap(AbstractMap* map, int noiseIndex);

	Map* CelluarMapToRealMap(AbstractMap* map, int noiseIndex);

	void CreateRoomForRealmap(Room* room, Map* map, int noiseIndex);

	void CreatePathForRealMap(Path* path, Map* map, TileType type);

	void AddGrass(Map* map, TileType floor, TileType grass, int noiseIndex);

	//heatmap---------------------------------------------------------------
	bool GetIsBorder(TileHeatMap* map, IntVec2 pos);

	TileHeatMap* CreatePathHeatMap(AbstractMap* map, std::vector<bool> spreadable);

	TileHeatMap* CreateMainPathHeatMap(AbstractMap* map, Map* realMap);

	TileHeatMap* CreateSplitPathHeatMap(AbstractMap* map, Map* realMap);

	TileHeatMap* CreateTreasureHeatMap( Map* realMap);

	TileHeatMap* CreateEnemyHeatMap( Map* realMap);

	TileHeatMap* CreateOpenTerrainHeatMap( Map* realMap);

	//----------------------------------------------------------------------
	float GetEnemySpawnChanceAtPos(TileHeatMap* mainPathMap, TileHeatMap* splitPathMap, TileHeatMap* enemyMap, TileHeatMap* lootMap, TileHeatMap* openessMap, Map* map, IntVec2 pos);

	float GetLootSpawnChanceAtPos(TileHeatMap* mainPathMap, TileHeatMap* splitPathMap, TileHeatMap* lootMap, TileHeatMap* openessMap, Map* map, IntVec2 pos);

	//---------------------------------------------------------------------
	

};