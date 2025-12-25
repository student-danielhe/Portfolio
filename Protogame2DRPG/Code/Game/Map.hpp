#pragma once
#include "GameCommon.hpp"
#include "MapDefinition.hpp"
#include <vector>
#include "Engine/Core/Timer.hpp"
#include "Tile.hpp"
#include "Object.hpp"
#include "Engine/Core/TileHeatMap.hpp"
class Game;
class Map {
public:

	Game* m_owner = nullptr;
	Timer* m_PhysicsTimer = nullptr;
	IntVec2 m_dimensions;
	float m_tileSize = 1.f;
	MapDefinition* m_definition;
	Object* m_player = nullptr;
	Timer* m_movementTimer = nullptr;
	TileHeatMap* m_heatMap = nullptr;

	std::vector<Object*> m_bgObjects;
	std::vector<Object*> m_chObjects;
	std::vector<Object*> m_fgObjects;
	std::vector<Tile> m_tiles;

	Map(MapDefinition* def, Game* owner);

	Map(IntVec2 bounds, Game* owner);

	bool IsPositionInBound(IntVec2 pos);

	IntVec2 IndexToPos(int index);
	Vec2 GridToScreen(IntVec2 gridPos);
	
	void Update(float deltaSecond);
	void RemoveObjects();

	void Render();

	void OnEnter(Object* player, IntVec2 pos, SpriteDirection dir = SpriteDirection::SOUTH);
	void OnLeave();

	void HandleKeyPressed();

	void CreateTiles();

	bool IsBlocked(IntVec2 pos);

	Object* SpawnObject(ObjectType type, IntVec2 pos, Interaction* interaction=nullptr, SpriteDirection dir=SpriteDirection::SOUTH);

	Object* FindObjectAtPosition(IntVec2 pos);

	//IntVec2 GetMouseCell();



	//Path Finding------------------------------
	IntVec2 GetPathToPlayer(IntVec2 origin);

	TileHeatMap CreateHeatMap(IntVec2 targetCoords);

	

	void FloorToGrassPerlinNoise(float threshold = -.1f);

	IntVec2 GetRandomEmptyPos();

	IntVec2 GetFurthestEmptyPos(IntVec2 begin, TileHeatMap& out_heatMap);

	IntVec2 EnemyWander(IntVec2 pos, int index, int step);
};