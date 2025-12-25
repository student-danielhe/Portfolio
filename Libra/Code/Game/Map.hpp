#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Entity.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Tile.hpp"
#include <vector>
#include "GameCommon.hpp"
#include "TileDefinition.hpp"
#include "Engine/Core/TileHeatMap.hpp"

class Map {
	
    public:
		std::vector<TileType> m_wormTile{TILE_TYPE_STONE_FLOOR, TILE_TYPE_WALL};
		std::vector<int> m_numWorm{5,5};
		std::vector<int> m_wormLength{4,4};

		std::vector<Tile>       m_tiles; // Note: this is NOT a 2D array!
		IntVec2                   m_dimensions; // # of tiles wide (x) and high (y)
		Entity* m_player = nullptr;

		int m_maxHeat=0;
		int m_mapIndex=0;
		TileHeatMap* m_testHeatMap=nullptr;

		std::vector< Entity* >      m_allEntities;
		EntityList	m_entityListsByType[NUM_ENTITYTYPE];
		EntityList	m_agentListsByFaction[NUM_FACTION];
		EntityList	m_bulletListsByFaction[NUM_FACTION];

		Map(int x=30, int y=30);
		~Map();
		void Render()const;
		void Update(float deltaSecond);
		void GenerateTiles();
		void StartUp();
		void BeginFrame();
		void EndFrame();
		std::vector<AABB2*> GetAdjacentWall(Entity entity);
		Vec2 CameraPosition();
		Entity* SpawnNewEntity(EntityType type,Faction faction, Vec2 const& position, float orientationDegrees);
		void AddEntityToMap(Entity* e);
		void RemoveEntityFromMap(Entity* e);
		void RemoveEntityFromList(Entity* e, EntityList &entityList);
		Entity* CreateEntityOfType(EntityType type, Faction faction, Vec2 const& position, float orientationDegrees);
		void AddEntityToList(Entity* e, EntityList &entityList);
		void GenerateWorm(TileType type, int length);

		bool IsPointInSolid(Vec2 point);
		bool IsTileSolid(Tile tile); 
		bool HasLineOfSight(Vec2 const& PosA, Vec2 const& PosB, float range);
		void SpawnStartingEnemies();

		RaycastResult2D RaycastVsTiles(Vec2 start, Vec2 end);
		Vec2 FindImpactNormal(Vec2 impactPos, Vec2 stepForward) const;
		bool IsOutOfBound(Entity* e);
		void RemoveGarbageEntity();

		void CheckAgentCollision();
		void CheckBulletCollision(float deltaSeond);
		void CheckTileCollision();
		void HandleGoodBullet(Entity& bullet, float deltaSecond);
		void HandleEvilBullet(Entity& bullet, float deltaSecond);
		void PhysicsAgainstAries(Entity& Bullet,Entity& Aries, Vec2 forward);
		void PhysicsAgainstTile(Entity& Bullet, Vec2 forward);
		void GenerateStartArea(int col, int row);
		void GenerateEndArea(int col, int row);
		void CreateHeatMap();
		void CreateDistanceField(TileHeatMap& heatMap,  IntVec2 startCoords);
		std::vector<IntVec2> SpreadHeat(TileHeatMap& heatMap, std::vector<IntVec2> previousTiles, int currentHeat);
		bool IsSpreadable(TileHeatMap& heatMap, IntVec2 tile);
		IntVec2 EntityTilePosition(Entity e)const;
		void FillHoles();
};