#pragma once
#include "Chunk.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <map>
#include <math.h>
#include "Engine/Core/Timer.hpp"
class Map {
	public:
	std::map< IntVec2, Chunk* > m_activeChunks;
	std::map< IntVec2, Chunk* > m_pendingChunks;
	SpriteSheet* g_sprite = nullptr;
	
	uint8_t m_placeingType = BLOCK_TYPE_GLOWSTONE;

	std::vector<Vertex_PCU> m_worldBasis;
	std::vector<Vertex_PCU> m_playerBasis;
	bool m_debugging = true;
	float m_worldTime = 0.f;
	bool m_worldTimeFast = false;

	std::vector<BlockIterator> m_dirtyLightInDoor;
	std::vector<BlockIterator> m_dirtyLightOutDoor;

	Timer* m_liquidFlowTimer;
	Timer* m_physicsTimer;
	Timer* m_temperatureTimer;

	int m_rainThreshold = 20;
	int m_windThreshold=500;
	bool m_debugRainLava=false;
	float m_debugSetTemp = 10.f;
	int m_debugSetHumidity = 100;

	Map();
	~Map();

	void Update(float deltaSeconds);
	void HandleKeyPressed();

	void RegenerateNearestDirty();
	void ActivateNearest();
	void DeactiveFurthest();

	void Render();
	void RenderIMGUI();
	void BeginFrame();
	void EndFrame();

	int GetVertexCount();
	int GetIndexCount();

	void ActivateChunk(IntVec2 pos);
	void DeactivateChunk(IntVec2 pos);

	Chunk* GetChunkAtPos(IntVec2 pos);
	IntVec2 GetPlayerCoord();
	IntVec2 GetPlayerChunkCoord();


	Vec3 GetSurfaceBlock();


	Block* GetBlockFromIterator(BlockIterator iterator);

	void ProcessDirtyLighting();// : processes and propagates all dirty light blocks until none remain.
	void ProcessIndoor();
	std::vector<BlockIterator> SpreadIndoor(std::vector<BlockIterator> input, int lightVal);

	void ProcessOutdoor();
	std::vector<BlockIterator> SpreadOutdoor(std::vector<BlockIterator> input, int lightVal);

	std::vector<Chunk*> GetAllNearbyChunkWithinRange(int range, Chunk* current);
};