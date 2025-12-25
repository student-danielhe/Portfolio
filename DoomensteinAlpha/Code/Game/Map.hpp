#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Player.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>
#include "GameCommon.hpp"
#include "MapDefinition.hpp"
#include "TileDefinition.hpp"
#include "Tile.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Actor.hpp"
#include "Engine/Core/Timer.hpp"
class Actor;

class Map {

public:
	SoundID m_gameMusic = 0;
	SoundPlaybackID m_gameMusicPlayback = 0;
	Map(Game* game, const MapDefinition* defintion);
	~Map();

	void StartUp();
	void CreateTiles();
	void CreateGeometry();
	void AddGeometryForWall(const AABB3& bounds, const AABB2& UVs);
	void AddGeometryForFloor(const AABB3& bounds, const AABB2& UVs);
	void AddGeometryForCeilling(const AABB3& bounds, const AABB2& UVs);
	void CreateBuffers();

	void HandleKeyPress();

	bool IsPositionInBounds(const Vec3& position) const;
	bool AreCoordsInBounds(int x, int y)const;
	const Tile* GetTile(int x, int y) const;
	IntVec2 PositionToCoords(const Vec3& position) const;

	void Update(float deltaSeconds);
	void CollideAllActors();
	void CollideActors(Actor* actorA, Actor* actorB);
	void CollideActorsWithMap();
	void CollideActorWithMap(Actor* actor);

	void Render();
	void RenderPlayer(PlayerController* player);

	void AddDebugMessages();
	void DebugUpdate(float deltaSeconds);
	RaycastResult3D RaycastAll(const Vec3& start, const Vec3& fwd, float dist, Actor* owner=nullptr) const;
	RaycastResult3D RayCastWorldXY(const Vec3& start, const Vec3& fwd, float dist)const;
	RaycastResult3D RayCastWorldZ(const Vec3& start, const Vec3& fwd, float dist)const;
	RaycastResult3D RaycastWorldActors(const Vec3& start, const Vec3& fwd, float dist, Actor* owner = nullptr) const;
	RaycastResult3D RaycastWorldActors(Actor& out, const Vec3& start, const Vec3& fwd, float dist, Actor* owner = nullptr) const;
	void DamageActosInCone(Vec3 pos, EulerAngles orientation, float radiius, float cone, FloatRange damage, float impulse, ActorHandle* owner);
	void RaycastDamage(const Vec3& start, const Vec3& fwd, float dist,  FloatRange damage, float impulse, ActorHandle* owner);

	Game* m_game = nullptr;
	LightConstants* m_lightConstants = nullptr;
	std::vector<Actor*> m_actors;
	bool m_splitScreen = false;
	bool m_gold = false;
private:
	const MapDefinition* m_definition = nullptr;
	std::vector<Tile> m_tiles;
	IntVec2 m_dimensions = IntVec2(10, 10);

	int m_currentPossesedID = 0;
	unsigned int m_nextActorUID = 0;

	std::vector<Vertex_PCU> m_vertexes;
	std::vector<unsigned int> m_indexes;
	Texture* m_texture =nullptr;
	Shader* m_shader =nullptr;
	VertexBuffer* m_vertexBuffer =nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	Vec3 m_tileSize=Vec3(1.f,1.f,1.f);
	Timer* m_physicsTimer = nullptr;

	//-----------actor stuff
public:
	ActorHandle* m_player1Handle = nullptr;
	ActorHandle* m_player2Handle = nullptr;
	//---------------------------------------
	void DeleteDestroyedActors();
	void SpawnPlayer();
	Actor* SpawnInitialActor(const SpawnInfo& spawnInfo);
	Actor* GetActorByHandle(const ActorHandle& handle)const;
	Actor* GetClosestVisibleEnemyWithinRange(ActorHandle* actor);
	void DebugPossessNext();
	void AddProjectileToList(Actor* owner, WeaponDefinition* weaponDef, const EulerAngles& deg);
	ActorHandle* NextPossessible();
	ActorHandle* CreateValidHandle();
	ActorHandle* SpawnNewActor(std::string name, Vec3 position, Vec3 volicty = Vec3());
	PlayerController* GetPlayer(const ActorHandle& handle);
};