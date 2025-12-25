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
	GUIDED_PATH_CELLULAR_AUTOMATA,
};

struct SpecificPoint {
	IntVec2 m_position;
	int m_mapIndex;
};

struct DungeonMetrics {
    IntVec2 m_dimensions = IntVec2(64,64);
    int m_roomPerMap = 8;
	IntVec2 m_minimumRoomSize = IntVec2(7,7);
	GenerationMethod m_method = GenerationMethod::BINARY_SPACE_PARTITION;
};

struct Room {
	std::vector<IntVec2> m_entraces;
	AABB2 m_constraints;
	int m_mapIndex;
};

struct Path {
	std::vector<IntVec2> m_path;
	IntVec2 m_start;
	IntVec2 m_end;
};

struct Block5X5 {
	IntVec2 m_center;
	std::vector<Block5X5*> m_adjBloacks;
	std::vector<Path*> m_paths;
};

struct AbstractMap {
	std::vector<Room*> m_rooms;
	IntVec2 m_dimensions;
	std::vector<Path*> m_paths;
	std::vector<Block5X5*> m_blocks;
};
struct RoomBST {
	RoomBST* childA = nullptr;
	RoomBST* childB = nullptr;
	Room*    room   = nullptr;
	
};



struct FullDungeonMatrix {
    MapConnectionType type;
	int levels;
};



class MapGenerator {
public:
    std::vector<Map*>CreateFullDungeon(FullDungeonMatrix matrix, std::vector<DungeonMetrics*> metrics);
    std::vector<DungeonMetrics*> m_dungeonMetrics;

	std::vector<AbstractMap*> m_maps;


	std::vector<Map*> GenerateMaps();

	Game* m_owner = nullptr;

	MapGenerator(std::vector<DungeonMetrics*> metrics);

	void CreateRooms();

	void CreateRandomPath(Room* from, Room* to);

	void CreateTunnelingPath(Room* from, Room* to, int maxWalkLen);

	void AddRandomEntrances(Room* room);

	void AddFixedEntrances(Room* room);

	//Helper
	Room* GetRoomAt(SpecificPoint location);

	bool RoomCuttable(Room* room);

	private:
	//BST
	AbstractMap* CreateRoomsWithBSP(int index);
	void CutRoom(RoomBST* original, int shrink = 1);
	void CreatePathWithBSP(RoomBST* root);
	std::vector<RoomBST*> GetAllChildren(RoomBST* root);
	Path* CreateTunnelingPath(IntVec2 from, IntVec2 to, int noiseIndex = 0, int maxWalkLen = 3);
	

	void SpawnStair(Map* mapA, Map* mapB, IntVec2 pos);

	IntVec2 GetRoomCenter(Room* room);

	IntVec2 GetNextStep(IntVec2 location, TileHeatMap map);

	bool DoesPathCross(Path* pathA, Path*pathB);

	IntVec2 GuidedPathGetNextPoint(IntVec2 start, int len, int noiseIndex, int retry);

	bool InBound(IntVec2 dimensions, IntVec2 pos);

	void GenerateMapWithGuidedPath(int noise, IntVec2 startPos, int amountStep, int stepLen);

	
};