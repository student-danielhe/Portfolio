#pragma once
#include "Block.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <atomic>
#include <map>
class Chunk;



struct BlockIterator {
	Chunk* chunk;
	IntVec3 pos;
};
enum class Continentalness {
    DEEP_OCEAN,
    OCEAN,
    COAST,
    NEAR_INLAND,
    MID_INLAND,
    FAR_INLAND,
    COUNT
};

enum class Erosion {
    E0,
    E1,
    E2,
    E3,
    E4,
    E5,
    E6,
    COUNT,
};

enum class PV {
    VALLEYS,
    LOW,
    MID,
    HIGH,
    PEAKS,
    COUNT
};

enum class Temperature {
    T0,
    T1,
    T2,
    T3,
    T4,
    COUNT
};

enum class Humidity {
    H0,
    H1,
    H2,
    H3,
    H4,
    COUNT
};

enum class BiomeTypes {
    BEACH_BIOMES,
    BADLAND_BIOMES,
    MIDDLE_BIOMES,
    PEAK_BIOMES,
    OCEAN_BIOMES,
};

enum class Biome {
    SNOW_BEACH = 0,
    BEACH,
    DESERT,
    SAVANNA,
    SNOW_PLAINS,
    PLAINS,
    SNOW_TAIGA,
    TAIGA,
    FOREST,
    JUNGLE,
    STONE_PEAK,
    SNOW_PEAK,
    SNOW_OCEAN,
    OCEAN,
    DEEP_OCEAN,
    COUNT
};
enum class ChunkState {
	CONSTRUCTING,

	ACTIVATING_QUEQUED_LOAD,
	ACTIVATING_LOADING,
	ACTIVATING_GENERATION_COMPLETE,
	ACTIVE,

	DEACTIVATING_QUEUED_SAVE,
	DEACTIVATING_SAVING,
	DEACTIVATING_SAVED,
	DECONSTRUCTING,

	COUNT
};


struct BlockReplaceRules {

    BlockType m_surfaceReplace = BLOCK_TYPE_STONE;
    BlockType m_belowSeaLevelReplace = BLOCK_TYPE_STONE;

    BlockType m_seaLevelReplace = BLOCK_TYPE_WATER;

    bool m_hasDirtLayer = true;

    std::vector<BlockType> m_trees;

    float m_treeChance = .1f;

    float m_defaltTemperature = 10.f;
};

class Map;

struct ChunkSave {
	std::vector<uint8_t> saveData;
};

struct NearbyChunk {
    Chunk* North;
    Chunk* South;
    Chunk* East;
    Chunk* West;
};

class Chunk {

public:
    Map* m_owner;
    
    float m_totalTemp = 10.f;
    int m_humidity = 0;
    bool m_rainning = false;

    std::vector<Block> m_blocks;
    std::vector<float> m_temperature;
    std::vector<float> m_defaultTemperature;

    IntVec2 m_coords; 
    Vec3 m_position;
	std::vector<Vertex_PCUTBN> m_verts;
	std::vector<unsigned int> m_indexes;

    std::vector<int> m_skyHeight;
	VertexBuffer* m_vb;
	IndexBuffer* m_ib;
    AABB3 m_localBounds;
    AABB3 m_globalBounds;

	VertexBuffer* m_debugVB;
	IndexBuffer* m_debugIB;

    NearbyChunk* m_nearbyChunk;

    std::atomic<ChunkState> m_chunkState;

    bool m_debugRenderHighlight = false;

    static std::map<Biome, BlockReplaceRules> g_biomeRules;


    std::vector<IntVec3> m_dirtyLightInddor;
    std::vector<IntVec3> m_dirtyLightOutDoor;

    bool m_dirty = true;
    bool m_needSave = false;

    Chunk(Map* owner, Vec3 position);
    ~Chunk();

    void CreateBlocks();
    void CreateBlocksFromSave();
    void Regenerate();

    void Update(float deltaSeconds);

    int CoordToIndex(Vec3 coord);
    int CoordToIndex(IntVec3 coord);
    Vec3 IndexToCoord(int i);
    Vec3 LocalToGlobal(Vec3 localCoord);
    Vec3 GloablToLocal(Vec3 globalCoord);

    void Render();
    
    float GetRangeToPlayerSquared();

    ChunkSave CreateSave();
    std::string GetSaveName();
    bool BlockOpaque(Vec3 pos);
    bool CoordInBound(Vec3 pos);

    void Dig(Vec3 Pos);
    void Place(Vec3 pos, uint8_t type);

    

    Biome GetBiome(Continentalness cont, int e, PV pv, int t, int h);

    Continentalness GetContinentalness(float input);

    Erosion GetErosion(float input);

    PV GetPV(float input);

    Temperature GetTemperature(float input);

    Humidity GetHumidity(float input);

    static void InitializeBiomeRules();

    float GetDensity(float x, float y, float z, float continetalness);

    void CreateTree(int x, int y, int z, BlockType treeType);

    Block* GetBlockAtPos(IntVec3 pos);
    float GetTemperatureAtPos(IntVec3 pos);
    void SetTemperatureAtPos(IntVec3 pos, float temperature);

    BlockIterator GetBlockIteratorAtPos(IntVec3 pos);

    std::vector<BlockIterator> GetNearby(IntVec3 pos);

    bool ReadyToRegenerate();

    void InitializeLight();

    Rgba8 GetWorldColor(Rgba8 input, int opacity);

    int FindFirstSolidBlockBelow(IntVec3 coord);


    void HandleTemperature(float deltaSeconds);
    void HeatExchangeAtPos(IntVec3 pos, float deltaSeconds);
    void HeatExchangeBetweenPos(IntVec3 from, IntVec3 to, float deltaSeconds);
    void AdjustToDefault(IntVec3 pos, float deltaSeconds);

    void HandleFluidPhysics();
    void HandleFallingPhysics();
    IntVec3 LiquidPhysics(IntVec3 pos);
    IntVec3 PowderPhysics(IntVec3 pos);
    void ExchangePosition(IntVec3 from, IntVec3 to);

    void HandleHumidity();
    void HandleRain();

    void DebugAdjustTemp(float newTemperature);
    void DebugAdjustHumidity(int newHumidity);
};

