#include "Chunk.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Map.hpp"
#include "Game.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Math/Easing.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <cmath>
#include "GenerateChunkJob.hpp"
#include "SaveChunkJob.hpp"

std::map<Biome, BlockReplaceRules> Chunk::g_biomeRules = {};
CurvePiecewise* g_Continent_Height_Offset_Curve = nullptr;
CurvePiecewise* g_Continent_Squashing_Curve = nullptr;

Chunk::Chunk(Map* owner, Vec3 coords)
{
    m_chunkState = ChunkState::CONSTRUCTING;
    m_owner = owner;
	m_coords.x = (int)coords.x;
	m_coords.y = (int)coords.y;
    m_position.x = coords.x*CHUNK_SIZE_X;
	m_position.y = coords.y*CHUNK_SIZE_Y;

	m_localBounds = AABB3(Vec3(), Vec3((float)CHUNK_SIZE_X, (float)CHUNK_SIZE_Y, (float)CHUNK_SIZE_Z));
	m_globalBounds = AABB3(m_position, m_position + Vec3((float)CHUNK_SIZE_X, (float)CHUNK_SIZE_Y, (float)CHUNK_SIZE_Z));


	
	m_ib = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
	m_vb = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));

	m_debugIB = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
	m_debugVB = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	
	/*if(!DoesFileExists(GetSaveName()))
	    CreateBlocks();
	else {
		CreateBlocksFromSave();
	}*/

	m_nearbyChunk = new NearbyChunk;
	m_nearbyChunk->North = m_owner->GetChunkAtPos(IntVec2(m_coords.x, m_coords.y + 1));
	m_nearbyChunk->South = m_owner->GetChunkAtPos(IntVec2(m_coords.x, m_coords.y - 1));
	m_nearbyChunk->East = m_owner->GetChunkAtPos(IntVec2(m_coords.x + 1, m_coords.y));
	m_nearbyChunk->West = m_owner->GetChunkAtPos(IntVec2(m_coords.x - 1, m_coords.y));

	if (m_nearbyChunk->North)
		m_nearbyChunk->North->m_nearbyChunk->South = this;

	if (m_nearbyChunk->South)
		m_nearbyChunk->South->m_nearbyChunk->North = this;

	if (m_nearbyChunk->East)
		m_nearbyChunk->East->m_nearbyChunk->West = this;

	if (m_nearbyChunk->West)
		m_nearbyChunk->West->m_nearbyChunk->East = this;


	std::vector<Vertex_PCUTBN> verts;
	std::vector<unsigned int> indeces;

	AddVertsForAABB3D(verts, indeces, m_localBounds);

	g_theRenderer->CopyIndexedVertexCPUTBNToBuffers(verts, indeces, m_debugVB, m_debugIB);

	GenerateChunkJob* job = new GenerateChunkJob();
	job->m_chunk = this;
    g_jobSystem->AddPendingJob(job);
	m_chunkState = ChunkState::ACTIVATING_QUEQUED_LOAD;
	
}

Chunk::~Chunk()
{
    //if (m_needSave) {
	//
	//	m_chunkState = ChunkState::DEACTIVATING_SAVING;
	//	ChunkSave save = CreateSave();
	//	FileWriteBuffer(&save.saveData, GetSaveName());
	//	m_chunkState = ChunkState::DEACTIVATING_SAVED;
	//}

	m_chunkState = ChunkState::DECONSTRUCTING;
    delete m_vb;
	m_vb = nullptr;


	delete m_ib;
	m_ib = nullptr;

	m_verts.clear();
	m_indexes.clear();

	delete m_debugVB;
	m_debugVB = nullptr;

	delete m_debugIB;
	m_debugIB = nullptr;

	g_theRenderer->SetBuffers();

	Chunk* n = m_nearbyChunk->North;
	Chunk* s = m_nearbyChunk->South;
	Chunk* e = m_nearbyChunk->East;
	Chunk* w = m_nearbyChunk->West;

	if (n)
		n->m_nearbyChunk->South = nullptr;
	if (s)
		s->m_nearbyChunk->North = nullptr;
	if (e)
		e->m_nearbyChunk->West = nullptr;
	if (w)
		w->m_nearbyChunk->East = nullptr;

	m_nearbyChunk->North = nullptr;
	m_nearbyChunk->South = nullptr;
	m_nearbyChunk->East = nullptr;
	m_nearbyChunk->West = nullptr;

	
	m_blocks.clear();
}

void Chunk::CreateBlocks()
{
    int fullSize = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
    m_blocks.resize(fullSize);
	m_temperature.resize(fullSize);
	m_defaultTemperature.resize(fullSize);

	int XYSize = CHUNK_SIZE_X*CHUNK_SIZE_Y;

	std::vector<float>   continentalMapXY;
	continentalMapXY.resize(XYSize);

	std::vector<float>   erosionMapXY;
	erosionMapXY.resize(XYSize);

	std::vector<float>   PVMapXY;
	PVMapXY.resize(XYSize);

	std::vector<float> humidityMapXY;
	humidityMapXY.resize(XYSize);

	std::vector<float> temperatureMapXY;
	temperatureMapXY.resize(XYSize);

	std::vector<float> treeMapXY;
	treeMapXY.resize(XYSize);
	
	int continentSeed = GAME_SEED;
	int erosionSeed = continentSeed+1;
	int PVSeed = erosionSeed +1;
	int humiditySeed = PVSeed + 1;
	int temperatureSeed = humiditySeed + 1;
	int treeSeed = temperatureSeed + 1;


	for (int y = 0; y < CHUNK_SIZE_Y; y++) {
		for (int x = 0; x < CHUNK_SIZE_X; x++) {
		    
			float globalX = m_position.x + (float)x;
			float globalY = m_position.y + (float)y;
			
			int index = x + y* CHUNK_SIZE_X;

			continentalMapXY[index] = Compute2dPerlinNoise(globalX, globalY, CONTINENT_NOISE_SCALE, CONTINENT_NOISE_OCTAVES,
			    DEFAULT_OCTAVE_PERSISTANCE, DEFAULT_NOISE_OCTAVE_SCALE, true, GAME_SEED);

			erosionMapXY[index] = Compute2dPerlinNoise(globalX, globalY, EROSION_NOISE_SCALE, EROSION_NOISE_OCTAVES,
				DEFAULT_OCTAVE_PERSISTANCE, DEFAULT_NOISE_OCTAVE_SCALE, true, erosionSeed);

			PVMapXY[index] =1-abs(3 * abs(Compute2dPerlinNoise(globalX, globalY, HILLINESS_NOISE_SCALE, HILLINESS_NOISE_OCTAVES,
				DEFAULT_OCTAVE_PERSISTANCE, DEFAULT_NOISE_OCTAVE_SCALE, true, PVSeed)) -2);

			humidityMapXY[index] = Compute2dPerlinNoise(globalX, globalY, HUMIDITY_NOISE_SCALE, HUMIDITY_NOISE_OCTAVES,
				DEFAULT_OCTAVE_PERSISTANCE, DEFAULT_NOISE_OCTAVE_SCALE, true, humiditySeed);

			temperatureMapXY[index] = Compute2dPerlinNoise(globalX, globalY, TEMPERATURE_NOISE_SCALE, TEMPERATURE_NOISE_OCTAVES,
				DEFAULT_OCTAVE_PERSISTANCE, DEFAULT_NOISE_OCTAVE_SCALE, true, temperatureSeed);

			treeMapXY[index] = Get2dNoiseZeroToOne((int)globalX, (int)globalY, treeSeed);
		    
		}
	}

	//Apply Terrain

	for (int z = 0; z < CHUNK_SIZE_Z; z++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int x = 0; x < CHUNK_SIZE_X; x++) {
			    
				Vec3 local((float)x, (float)y, (float)z);
				Vec3 global = LocalToGlobal(local);

				int idx = CoordToIndex(local);
				int idxXY = x + y * CHUNK_SIZE_X;


				int type = BLOCK_TYPE_AIR;

				float density = GetDensity( global.x, global.y, global.z, continentalMapXY[idxXY]);

				if (density < 0) {
					type = BLOCK_TYPE_STONE;
				}

				Block block;
				block.m_type = (uint8_t)type;
	            m_blocks[idx] = block;

			}
		}
	}

	//Apply Biome effect


	for (int y = 0; y < CHUNK_SIZE_Y; y++) {
		for (int x = 0; x < CHUNK_SIZE_X; x++) {
			
			int idxXY = x + y * CHUNK_SIZE_X;

			float continentalness = continentalMapXY[idxXY];
			float erosion = erosionMapXY[idxXY];
			float PV = PVMapXY[idxXY];
			float humidity = humidityMapXY[idxXY];
			float temperature = temperatureMapXY[idxXY];

			Biome biome = GetBiome(GetContinentalness(continentalness), (int)GetErosion(erosion), GetPV(PV), (int)GetTemperature(temperature), (int)GetHumidity(humidity));
            
			auto pair = g_biomeRules.find(biome);
			if (pair == g_biomeRules.end()) {
			    ERROR_AND_DIE("Biome Not Found");
			}

			BlockReplaceRules rule = pair->second;

			int depth = 0;

			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--) {
				Vec3 local((float)x, (float)y, (float)z);
				Vec3 global = LocalToGlobal(local);

				int idx = CoordToIndex(local);

				bool IsSolid = m_blocks[idx].m_type == BLOCK_TYPE_STONE;

				uint8_t type = m_blocks[idx].m_type;

				if (depth > 0) {
					if (rule.m_hasDirtLayer&&depth <= MAX_DIRT_OFFSET_Z) {
						if (biome == Biome::DESERT) {
						    type = (uint8_t)BLOCK_TYPE_SAND;
						}
						else {
						    type = (uint8_t)BLOCK_TYPE_DIRT;
						}
					}
					else {

						if (global.z <= LAVA_Z) {
							type = (uint8_t)BLOCK_TYPE_LAVA;
						}

						else if (global.z <= OBSIDIAN_Z) {
							type = (uint8_t)BLOCK_TYPE_OBSIDIAN;
						}

						else {
							float oreNoise = Get3dNoiseZeroToOne((int)global.x, (int)global.y, (int)global.z);

							if (oreNoise < DIAMOND_CHANCE) {
								type = (uint8_t)BLOCK_TYPE_DIAMOND;
							}
							else if (oreNoise < GOLD_CHANCE) {
								type = (uint8_t)BLOCK_TYPE_GOLD;
							}
							else if (oreNoise < IRON_CHANCE) {
								type = (uint8_t)BLOCK_TYPE_IRON;
							}
							else if (oreNoise < COAL_CHANCE) {
								type = (uint8_t)BLOCK_TYPE_COAL;
							}
							else {
								type = (uint8_t)BLOCK_TYPE_STONE;
							}
						}
					}
				    depth++;
				}
				else {
					if (z == SEA_LEVEL_Z && !IsSolid) {
						//water surface
						type = (uint8_t)rule.m_seaLevelReplace;
					}

					bool underWater = z < SEA_LEVEL_Z;

					if (underWater) {
						if (IsSolid && depth == 0) {
							//underwater surface
							type =(uint8_t) rule.m_belowSeaLevelReplace;
							depth++;
						}
						else {
							type = (uint8_t)BLOCK_TYPE_WATER;
						}
					}

					if (IsSolid && depth == 0) {
						type = (uint8_t)rule.m_surfaceReplace;


						if ( treeMapXY[idxXY] < rule.m_treeChance&& !rule.m_trees.empty()) {
							bool higher = true;

							int minX = x - 1;
							int maxX = x + 1;
							int minY = y - 1;
							int maxY = y + 1;

							if (x - 1 < 0) {
								minX = x;
							}

							if (x + 1 >= CHUNK_SIZE_X) {
							   maxX = x;
							}

							if (y - 1 < 0) {
							    minY = y;
							}
							if (y + 1 >= CHUNK_SIZE_Y) {
							    maxY = y;
							}

							for (int otherX = minX; otherX <= maxX; otherX++) {
								for (int otherY = minY; otherY <= maxY; otherY++) {
									int otherIdx = otherX + otherY * CHUNK_SIZE_X;
									if ((float)treeMapXY[otherIdx] <= (float)treeMapXY[idxXY]&&!(otherX==x&&otherY==y)) {
										higher = false;
									}
								}
							}

							if (higher) {
							    int treeIndex = 0;

								treeIndex = (int)(Get2dNoiseZeroToOne(x,y,treeSeed+1)*rule.m_trees.size());
								

								CreateTree(x, y, z, rule.m_trees[treeIndex]);
							}
							    
						}

						depth++;
					}

				}

				
				m_blocks[idx].m_type = type;
				
				if (z == 0) {
				    m_defaultTemperature[idx] = 5000;
				}
				else {
					m_defaultTemperature[idx] = rule.m_defaltTemperature;
				}

				m_temperature[idx] = m_defaultTemperature[idx];

			}


		}
	}
	
}

void Chunk::CreateBlocksFromSave()
{
	std::string name = GetSaveName();

	std::vector<uint8_t> saveBuffer;

	FileReadToBuffer(saveBuffer, name);

	int saveIndex = 8;
	int blockIndex = 0;
	while (blockIndex < m_blocks.size()) {

	    uint8_t type = saveBuffer[saveIndex];
		int count = saveBuffer[saveIndex + 1];
		saveIndex += 2;

		for (int i = 0; i < count; i++) {

			m_blocks[blockIndex+i].m_type = type;

		}

		blockIndex += count;
	}
}


void Chunk::Regenerate()
{
    Vec3 camPos = g_theGame->m_player->GetPosition();
	if (m_chunkState == ChunkState::ACTIVE) {
		m_indexes.clear();
		m_verts.clear();
		m_dirty = false;
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				for (int x = 0; x < CHUNK_SIZE_X; x++) {

					BlockDefinition* def = BlockDefinition::g_blockDefinitions[m_blocks[CoordToIndex(Vec3((float)x, (float)y, (float)z))].m_type];
					if (def != BlockDefinition::g_blockDefinitions[BLOCK_TYPE_AIR]) {
						int indexStart = 0;

						int UVIndex = 0;
						AABB2 UV;
						Vec3 tangent;
						Vec3 bitangent;
						Vec3 normal;


						if (!BlockOpaque(Vec3((float)x, (float)y, (float)(z + 1)))&&!(GetBlockAtPos(IntVec3(x, y, z + 1))&&GetBlockAtPos(IntVec3(x, y, z + 1))->m_type==def->m_type)) {
						    
							//Add Top
							indexStart = (int)m_verts.size();
							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 2);
							m_indexes.push_back(indexStart + 1);

							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 3);
							m_indexes.push_back(indexStart + 2);


							UVIndex = (int)def->m_topSpriteCoord.x + (int)def->m_topSpriteCoord.y * 8;
							UV = g_sprite->GetSpriteUVs(UVIndex);

							tangent = Vec3(1.f, 0.f, 0.f);
							bitangent = Vec3(0.f, 1.f, 0.f);
							normal = Vec3(0.f, 0.f, 1.f);

							Rgba8 color;
							Block* b = GetBlockAtPos(IntVec3(x, y, z + 1));
							if (b) {
								color = Rgba8((unsigned char)b->GetOutdoor(), (unsigned char)b->GetIndoor(), 255);
							}
							else {
								color = Rgba8((unsigned char)15, (unsigned char)15, 255);
							}



							color = GetWorldColor(color, 255);
							
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y, (float)z + 1.f),             color, UV.m_mins, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y, (float)z + 1.f),       color, Vec2(UV.m_maxs.x, UV.m_mins.y), tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y + 1.f, (float)z + 1.f), color, UV.m_maxs, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y + 1, (float)z + 1),           color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal));
						}

						if (!BlockOpaque(Vec3((float)x, (float)y, (float)(z - 1)))&&!(GetBlockAtPos(IntVec3(x, y, z - 1))&&GetBlockAtPos(IntVec3(x, y, z - 1))->m_type==def->m_type)) {
							//Add Bottom
							indexStart = (int)m_verts.size();
							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 2);
							m_indexes.push_back(indexStart + 1);

							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 3);
							m_indexes.push_back(indexStart + 2);

							UVIndex = (int)def->m_bottomSpriteCoord.x + (int)def->m_bottomSpriteCoord.y * 8;
							UV = g_sprite->GetSpriteUVs(UVIndex);

							tangent = Vec3(1.f, 0.f, 0.f);
							bitangent = Vec3(0.f, -1.f, 0.f);
							normal = Vec3(0.f, 0.f, -1.f);

							Rgba8 color;
							Block* b = GetBlockAtPos(IntVec3(x, y, z - 1));
							if (b) {
								color = Rgba8((unsigned char)b->GetOutdoor(), (unsigned char)b->GetIndoor(), 255);
							}
							else {
								color = Rgba8((unsigned char)15, (unsigned char)15, 255);
							}



							color = GetWorldColor(color, 255);

							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y + 1.f, (float)z),       color, UV.m_mins, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y + 1.f, (float)z), color, Vec2(UV.m_maxs.x, UV.m_mins.y), tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y, (float)z),       color, UV.m_maxs, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y, (float)z),             color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal));
						}



						//Add Sides
						UVIndex = (int)def->m_sideSpriteCoord.x + (int)def->m_sideSpriteCoord.y * 8;
						UV = g_sprite->GetSpriteUVs(UVIndex);
						bitangent = Vec3(0.f, 0.f, 1.f);


						if (!BlockOpaque(Vec3((float)x, (float)(y - 1), (float)z))&&!(GetBlockAtPos(IntVec3(x, y - 1, z))&&GetBlockAtPos(IntVec3(x, y - 1, z))->m_type==def->m_type)) {
							//Front
							indexStart = (int)m_verts.size();
							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 2);
							m_indexes.push_back(indexStart + 1);

							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 3);
							m_indexes.push_back(indexStart + 2);

							tangent = Vec3(1.f, 0.f, 0.f);
							normal = Vec3(0.f, 1.f, 0.f);

							Rgba8 color;
							Block* b = GetBlockAtPos(IntVec3(x, y - 1, z));
							if (b) {
								color = Rgba8((unsigned char)b->GetOutdoor(), (unsigned char)b->GetIndoor(), 255);
							}
							else {
								color = Rgba8((unsigned char)15, (unsigned char)15, 255);
							}



							color = GetWorldColor(color, 255);

							

							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y, (float)z),             color, UV.m_mins, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y, (float)z),       color, Vec2(UV.m_maxs.x, UV.m_mins.y), tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y, (float)z + 1.f), color, UV.m_maxs, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y, (float)z + 1.f),       color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal));
						}

						if (!BlockOpaque(Vec3((float)x, (float)(y + 1), (float)z))&&!(GetBlockAtPos(IntVec3(x, y + 1, z))&&GetBlockAtPos(IntVec3(x, y + 1, z))->m_type==def->m_type)) {

							//Back
							indexStart = (int)m_verts.size();
							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 2);
							m_indexes.push_back(indexStart + 1);

							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 3);
							m_indexes.push_back(indexStart + 2);

							tangent = Vec3(-1.f, 0.f, 0.f);
							normal = Vec3(0.f, -1.f, 0.f);

							Rgba8 color;
							Block* b = GetBlockAtPos(IntVec3(x, y + 1, z));
							if (b) {
								color = Rgba8((unsigned char)b->GetOutdoor(), (unsigned char)b->GetIndoor(), 255);
							}
							else {
								color = Rgba8((unsigned char)15, (unsigned char)15, 255);
							}

							color = GetWorldColor(color, 255);

							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y + 1, (float)z),       color, UV.m_mins, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y + 1, (float)z),             color, Vec2(UV.m_maxs.x, UV.m_mins.y), tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y + 1, (float)z + 1.f),       color, UV.m_maxs, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y + 1, (float)z + 1.f), color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal));
						}

						if (!BlockOpaque(Vec3((float)(x - 1), (float)y, (float)z))&&!(GetBlockAtPos(IntVec3(x - 1, y, z))&&GetBlockAtPos(IntVec3(x - 1, y, z))->m_type==def->m_type)) {
							//Left
							indexStart = (int)m_verts.size();
							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 2);
							m_indexes.push_back(indexStart + 1);

							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 3);
							m_indexes.push_back(indexStart + 2);

							tangent = Vec3(0.f, -1.f, 0.f);
							normal = Vec3(-1.f, 0.f, 0.f);

							Rgba8 color;
							Block* b = GetBlockAtPos(IntVec3(x - 1, y, z));
							if (b) {
								color = Rgba8((unsigned char)b->GetOutdoor(), (unsigned char)b->GetIndoor(), 255);
							}
							else {
								color = Rgba8((unsigned char)15, (unsigned char)15, 255);
							}


							color = GetWorldColor(color, 255);

							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y + 1.f, (float)z),       color, UV.m_mins, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y, (float)z),             color, Vec2(UV.m_maxs.x, UV.m_mins.y), tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y, (float)z + 1.f),       color, UV.m_maxs, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x, (float)y + 1.f, (float)z + 1.f), color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal));
						}

						if (!BlockOpaque(Vec3((float)(x + 1), (float)y, (float)z))&&!(GetBlockAtPos(IntVec3(x + 1, y, z))&&GetBlockAtPos(IntVec3(x + 1, y, z))->m_type==def->m_type)) {
							//Right
							indexStart = (int)m_verts.size();
							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 2);
							m_indexes.push_back(indexStart + 1);


							m_indexes.push_back(indexStart);
							m_indexes.push_back(indexStart + 3);
							m_indexes.push_back(indexStart + 2);



							tangent = Vec3(0.f, 1.f, 0.f);
							normal = Vec3(1.f, 0.f, 0.f);
							Rgba8 color;
							Block* b = GetBlockAtPos(IntVec3(x + 1, y, z));
							if (b) {
							    color = Rgba8((unsigned char)b->GetOutdoor(), (unsigned char)b->GetIndoor(), 255);
							}
							else {
								color = Rgba8((unsigned char)15, (unsigned char)15, 255);
							}

							color = GetWorldColor(color, 255);

							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y, (float)z),             color, UV.m_mins, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y + 1.f, (float)z),       color, Vec2(UV.m_maxs.x, UV.m_mins.y), tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y + 1.f, (float)z + 1.f), color, UV.m_maxs, tangent, bitangent, normal));
							m_verts.push_back(Vertex_PCUTBN(Vec3((float)x + 1.f, (float)y, (float)z + 1.f),       color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal));
						}


					}

				}
			}
		}



		g_theRenderer->CopyIndexedVertexCPUTBNToBuffers(m_verts, m_indexes, m_vb, m_ib);
	}
    

}

void Chunk::Update(float deltaSeconds)
{

    UNUSED(deltaSeconds);
	/*if (m_dirty) {
		Regenerate();
	}*/

	//HandleTemperature(deltaSeconds);
	HandleFallingPhysics();
}

int Chunk::CoordToIndex(Vec3 coord)
{
	int y = ((int)coord.y << 4 & CHUNK_MASK_Y);
	int z = ((int)coord.z << 8 & CHUNK_MASK_Z);
	return (int)coord.x+y+z;
}

int Chunk::CoordToIndex(IntVec3 coord)
{
	int y = (coord.y << 4 & CHUNK_MASK_Y);
	int z = (coord.z << 8 & CHUNK_MASK_Z);
	return coord.x + y + z;
}

Vec3 Chunk::IndexToCoord(int i)
{
    int x = i&CHUNK_MASK_X;
	int y = (i&CHUNK_MASK_Y)>>4;
	int z = (i&CHUNK_MASK_Z)>>8;
	return Vec3((float)x, (float)y, (float)z);
}

Vec3 Chunk::LocalToGlobal(Vec3 localCoord)
{
    return localCoord + m_position;
}

Vec3 Chunk::GloablToLocal(Vec3 globalCoord)
{
    return globalCoord - m_position;
}

void Chunk::Render()
{
    g_theRenderer->BindTexture(&g_sprite->GetTexture());
    Mat44 modelTransform = Mat44::MakeTranslation2D(m_position);
    g_theRenderer->SetModelConstants(modelTransform);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
    g_theRenderer->DrawIndexedVertexBuffer(m_vb, m_ib);
	//debug render-----------------------------------------
	if (m_owner->m_debugging) {
		if (m_debugRenderHighlight) {

		
		g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawIndexedVertexBuffer(m_debugVB, m_debugIB);
		m_debugRenderHighlight = false;
		}
	}
	
}


float Chunk::GetRangeToPlayerSquared()
{
    Vec2 disp = m_position - g_theGame->m_player->GetPosition();
	return disp.GetLengthSquared();
}

ChunkSave Chunk::CreateSave()
{
    ChunkSave save;
	save.saveData.push_back('G');
	save.saveData.push_back('C');
	save.saveData.push_back('H');
	save.saveData.push_back('K');
	save.saveData.push_back('1');
	save.saveData.push_back(CHUNK_BITS_X);
	save.saveData.push_back(CHUNK_BITS_Y);
	save.saveData.push_back(CHUNK_BITS_Z);
	
	int i = 0;
	uint8_t type = (uint8_t)BLOCK_TYPE_COUNT;
	while (i < m_blocks.size()) {
		if (m_blocks[i].m_type != type || save.saveData[save.saveData.size() - 1] == 255) {
		    type = m_blocks[i].m_type;
			save.saveData.push_back(type);
			save.saveData.push_back((uint8_t)1);
		}
		else {
			save.saveData[save.saveData.size() - 1] ++;
		}

		i++;
	}

	return save;
}

std::string Chunk::GetSaveName()
{
	
	std::string name = "Saves/Chunk(";
	name += std::to_string(m_coords.x);
	name += ",";
	name += std::to_string(m_coords.y);
	name += ").chunk";
	return name;
}

bool Chunk::BlockOpaque(Vec3 pos)
{
    Block* b = GetBlockAtPos(IntVec3((int)pos.x, (int)pos.y, (int)pos.z));
	return b&&b->m_type!=BLOCK_TYPE_AIR&&b->m_type!=BLOCK_TYPE_WATER&&BlockDefinition::g_blockDefinitions[b->m_type]->m_meltTo!=BLOCK_TYPE_AIR;
}

bool Chunk::CoordInBound(Vec3 pos)
{
	if (pos.x < 0 || pos.x >= CHUNK_SIZE_X) {
		return false;
	}

	if (pos.y < 0 || pos.y >= CHUNK_SIZE_Y) {
		return false;
	}

	if (pos.z < 0 || pos.z >= CHUNK_SIZE_Z) {
		return false;
	}
	return true;
}

void Chunk::Dig(Vec3 Pos)
{
	if (CoordInBound(Pos)) {
	    m_blocks[CoordToIndex(Pos)].m_type= BLOCK_TYPE_AIR;
		BlockIterator bi;
		bi.chunk = this;
		bi.pos = IntVec3((int)Pos.x, (int)Pos.y, (int)Pos.z);
		InitializeLight();
		m_needSave = true;
		m_dirty = true;
	}
}

void Chunk::Place(Vec3 pos, uint8_t type)
{
	if (CoordInBound(pos)) {
	    m_blocks[CoordToIndex(pos)].SetOutdoor(0);
		m_blocks[CoordToIndex(pos)].SetIsSky(false);
		m_blocks[CoordToIndex(pos)].m_type = type;
		BlockIterator bi;
		bi.chunk = this;
		bi.pos = IntVec3((int)pos.x, (int)pos.y, (int)pos.z);
		std::vector<BlockIterator> nearby = GetNearby(IntVec3((int)pos.x, (int)pos.y, (int)pos.z));
		
		if (type == BLOCK_TYPE_GLOWSTONE) {
		    m_blocks[CoordToIndex(pos)].SetIndoor(15);
		}

		if (type == BLOCK_TYPE_LAVA) {
			m_temperature[CoordToIndex(pos)] = 5000;
		}
		InitializeLight();
		m_needSave = true;
		m_dirty = true;
	}
}

Biome Chunk::GetBiome(Continentalness cont, int erosion, PV pv, int temperature, int humidity)
{
    BiomeTypes type = BiomeTypes::OCEAN_BIOMES;

	if (cont != Continentalness::OCEAN && cont != Continentalness::DEEP_OCEAN) {
	    
		
		if (pv == PV::VALLEYS) {
			if (cont == Continentalness::COAST) {
				type = BiomeTypes::BEACH_BIOMES;
			}
			else {
				if (temperature < 4) {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
				else {
					type = BiomeTypes::BADLAND_BIOMES;
				}
			}
		}
		
		if (pv == PV::LOW) {
			if (erosion <= 1) {
				if (cont == Continentalness::COAST) {
					type = BiomeTypes::BEACH_BIOMES;
				}
				else{
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}

			}
			else if (erosion <= 3) {
				if (cont == Continentalness::COAST) {
					type = BiomeTypes::BEACH_BIOMES;
				}
				else if(cont == Continentalness::NEAR_INLAND) {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
				else {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}

			}else{
				if (cont == Continentalness::COAST) {
					type = BiomeTypes::BEACH_BIOMES;
				}
				else {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
			}

		}
		
		if (pv == PV::MID) {
			if (erosion <= 1) {
				if (cont == Continentalness::COAST) {
					type = BiomeTypes::BEACH_BIOMES;
				}
				else {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
			}
			else if (erosion == 2) {
				if (cont == Continentalness::COAST) {
					type = BiomeTypes::BEACH_BIOMES;
				}
				else if (cont == Continentalness::NEAR_INLAND) {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
				else if (cont == Continentalness::MID_INLAND) {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
				else {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
			}
			else if (erosion == 3) {
				if (cont == Continentalness::COAST||cont==Continentalness::NEAR_INLAND) {
				    type = BiomeTypes::MIDDLE_BIOMES;
				}
				else {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
			}
			else if (erosion == 4) {
			    type = BiomeTypes::MIDDLE_BIOMES;
			}
			else{
				if (cont == Continentalness::COAST) {
					type = BiomeTypes::BEACH_BIOMES;
				}
				else {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
			}
		}
		
		if (pv == PV::HIGH) {
			if (erosion == 0) {
				if (cont == Continentalness::COAST || cont == Continentalness::NEAR_INLAND) {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
				else {
					type = BiomeTypes::PEAK_BIOMES;
				}
			}
			else if (erosion == 1) {
				if (cont == Continentalness::NEAR_INLAND) {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
				else {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
			}
			else if (erosion == 2) {
			    type = BiomeTypes::MIDDLE_BIOMES;
			}
			else if (erosion == 3) {
				if(cont == Continentalness::MID_INLAND) {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
				else {
				    type = BiomeTypes::MIDDLE_BIOMES;
				}
			}
			else {
			    type = BiomeTypes::MIDDLE_BIOMES;
			}
		}
		
		if (pv == PV::PEAKS) {
			if (erosion == 0) {
			    type = BiomeTypes::PEAK_BIOMES;
			}
			else if (erosion == 1) {
				if (cont == Continentalness::COAST || cont == Continentalness::NEAR_INLAND) {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
				else {
					type = BiomeTypes::PEAK_BIOMES;
				}
			}
			else if (erosion == 2) {
			    type = BiomeTypes::MIDDLE_BIOMES;
			}
			else if (erosion == 3) {
				if (cont == Continentalness::MID_INLAND) {
					if (temperature < 4) {
						type = BiomeTypes::MIDDLE_BIOMES;
					}
					else {
						type = BiomeTypes::BADLAND_BIOMES;
					}
				}
				else {
					type = BiomeTypes::MIDDLE_BIOMES;
				}
			}
			else {
			    type = BiomeTypes::MIDDLE_BIOMES;
			}
		}

	}

	if (type == BiomeTypes::BEACH_BIOMES) {
		if (temperature == 0) {
			return Biome::SNOW_BEACH;
		}
		else if (temperature <= 3) {
			return Biome::BEACH;
		}
		else {
			return Biome::DESERT;
		}
	}

	if (type == BiomeTypes::BADLAND_BIOMES) {
		if (humidity <= 2) {
			return Biome::DESERT;
		}
		else {
			return Biome::SAVANNA;
		}
	}

	if (type == BiomeTypes::MIDDLE_BIOMES) {
		if (temperature == 4) {
			return Biome::DESERT;
		}
		else if (humidity <= 1) {
			    if (temperature == 0) {
			    	return Biome::SNOW_TAIGA;
			    }
			    else if(temperature <= 2){
			        return Biome::PLAINS;
			    }
			    else{
			        return Biome::SAVANNA;
			    }
		}
		else if (humidity == 2) {
			if (temperature == 0) {
				return Biome::SNOW_TAIGA;
			}
			else if(temperature <= 2){
			    return Biome::FOREST;
			}
			else {
				return Biome::PLAINS;
			}
		}
		else if (humidity == 3) {
			if (temperature == 0) {
				return Biome::SNOW_TAIGA;
			}
			else if (temperature == 1) {
				return Biome::TAIGA;
			}
			else if (temperature == 2) {
				return Biome::FOREST;
			}
			else {
				return Biome::JUNGLE;
			}
		}
		else {
			if (temperature <= 1) {
				return Biome::TAIGA;
			}
			else {
				return Biome::JUNGLE;
			}
		}
	}

	if (type == BiomeTypes::PEAK_BIOMES) {
		if (temperature <= 2) {
			return Biome::SNOW_PEAK;
		}
		else {
			return Biome::STONE_PEAK;
		}
	}

	if (type == BiomeTypes::OCEAN_BIOMES) {
		if (temperature == 0) {
			return Biome::SNOW_OCEAN;
		}
		else {
			if (cont == Continentalness::OCEAN) {
			    return Biome::OCEAN;
			}
			else {
				if (temperature == 1) {
					return Biome::SNOW_OCEAN;
				}
				return Biome::DEEP_OCEAN;
			}
		}
	}

	return Biome::COUNT;

}

Continentalness Chunk::GetContinentalness(float input)
{
	if (input <= -.455f) {
		return Continentalness::DEEP_OCEAN;
	}

	if (input <= -.19f) {
		return Continentalness::OCEAN;
	}

	if (input <= -.11f) {
		return Continentalness::COAST;
	}

	if (input <= .03f) {
	    return Continentalness::NEAR_INLAND;
	}

	if (input == .3f) {
		return Continentalness::MID_INLAND;
	}

	return Continentalness::FAR_INLAND;
}

Erosion Chunk::GetErosion(float input)
{
	if (input <= -.78f) {
		return Erosion::E0;
	}
	if (input <= -.375f) {
		return Erosion::E1;
	}
	if (input <= -.2225f) {
		return Erosion::E2;
	}
	if (input <= .05f) {
		return Erosion::E3;
	}
	if (input <= .45f) {
		return Erosion::E4;
	}
	if (input <= .55f) {
		return Erosion::E5;
	}
	return Erosion::E6;
}

PV Chunk::GetPV(float input)
{
	if (input <= -.85f) {
		return PV::VALLEYS;
	}

	if (input <= -.2f) {
		return PV::LOW;
	}

	if (input <= .2f) {
		return PV::MID;
	}

	if (input <= .7f) {
		return PV::HIGH;
	}

	return PV::PEAKS;
	
}

Temperature Chunk::GetTemperature(float input)
{
	if (input <= -.45f) {
		return Temperature::T0;
	}

	if (input <= -.15f) {
		return Temperature::T1;
	}

	if (input <= .2f) {
		return Temperature::T2;
	}

	if (input <= .55f) {
		return Temperature::T3;
	}

	return Temperature::T4;
}

Humidity Chunk::GetHumidity(float input)
{
	if (input < -.35f) {
		return Humidity::H0;
	}

	if (input < -.10f) {
		return Humidity::H1;
	}

	if (input < .10f) {
		return Humidity::H2;
	}

	if (input < .30f) {
		return Humidity::H3;
	}

	return Humidity::H4;
}

void Chunk::InitializeBiomeRules()
{

	std::map<float, float> heightOffest = { {-1.0f, -0.6f}, {-0.4f, -0.4f}, {0.4f, 0.4f}, {1.0f, 0.6f} };
	std::map<float, float> squash = { {-1.0f, 0.0f}, {-0.5f, 0.0f}, {-0.25f, 2.0f}, {0.25f, 2.0f}, {1.0f, -1.5f} };
    //std::map<float, float> heightOffest ={{ - 1.0, -1.0}, { - 0.45, -0.85}, { - 0.42, -0.38}, { - 0.4, -0.35}, { - 0.38, -0.33}, { - 0.27, -0.32}, { - 0.25, -0.3}, { - 0.23, -0.27}, { - 0.22, -0.05}, { - 0.2, -0.02}, { - 0.15, 0.0}, { - 0.10, 0.02}, {0.05, 0.1}, {0.20, 0.3}, {0.35, 0.4}, {0.65, 0.5}, {0.85, 0.85}, {1.00, 1.0}};
	//std::map<float, float> squash ={{-1.0, 0.0}, {-0.45, 0.02}, {-0.2, 0.0}, {-0.05, 0.02}, {0.05, 0.05}, {0.20, 0.05}, {0.35, 0.05}, {0.45, 0.02}, {1.0, 0.0}};
	g_Continent_Height_Offset_Curve = new CurvePiecewise(heightOffest);;
	g_Continent_Squashing_Curve = new CurvePiecewise(squash);

    BlockReplaceRules OceanRule;
	OceanRule.m_surfaceReplace = BLOCK_TYPE_SAND;
	OceanRule.m_belowSeaLevelReplace = BLOCK_TYPE_DIRT;

	g_biomeRules.insert({Biome::OCEAN, OceanRule});

	BlockReplaceRules DeapOceanRule;
	DeapOceanRule.m_hasDirtLayer = false;
	DeapOceanRule.m_surfaceReplace = BLOCK_TYPE_SAND;
	DeapOceanRule.m_defaltTemperature = 1.f;
	g_biomeRules.insert({Biome::DEEP_OCEAN, DeapOceanRule});
	
	BlockReplaceRules FrozenOceanRule;
	FrozenOceanRule.m_hasDirtLayer = false;
	FrozenOceanRule.m_surfaceReplace = BLOCK_TYPE_SNOW;
	FrozenOceanRule.m_seaLevelReplace = BLOCK_TYPE_ICE;
	FrozenOceanRule.m_defaltTemperature = -50.f;
	g_biomeRules.insert({Biome::SNOW_OCEAN, FrozenOceanRule});

	BlockReplaceRules BeachRule;
	BeachRule.m_surfaceReplace = BLOCK_TYPE_SAND;

	g_biomeRules.insert({Biome::BEACH, BeachRule});

	BlockReplaceRules SnowBeachRule;
	SnowBeachRule.m_surfaceReplace = BLOCK_TYPE_SNOW;
	SnowBeachRule.m_defaltTemperature = -30.f;
	g_biomeRules.insert({Biome::SNOW_BEACH, SnowBeachRule});

	BlockReplaceRules DesertRule;
	DesertRule.m_seaLevelReplace = BLOCK_TYPE_SAND;
	DesertRule.m_trees.push_back(BLOCK_TYPE_CACTUSLOG);
	DesertRule.m_treeChance=0.001f;
	DesertRule.m_defaltTemperature = 60.f;
	g_biomeRules.insert({Biome::DESERT,DesertRule});

	BlockReplaceRules SavannaRule;
	SavannaRule.m_surfaceReplace = BLOCK_TYPE_YELLOW_GRASS;
	SavannaRule.m_trees.push_back(BLOCK_TYPE_CACTUSLOG);
	SavannaRule.m_trees.push_back(BLOCK_TYPE_ACACIALEAVES);
	SavannaRule.m_treeChance=0.003f;
	SavannaRule.m_defaltTemperature=40.f;
	g_biomeRules.insert({Biome::SAVANNA, SavannaRule});

	BlockReplaceRules PlainsRule;
	PlainsRule.m_surfaceReplace = BLOCK_TYPE_LIGHT_GRASS;

	g_biomeRules.insert({Biome::PLAINS, PlainsRule});

	BlockReplaceRules SnowPlainsRule;
	SnowPlainsRule.m_surfaceReplace = BLOCK_TYPE_SNOW;
	SnowPlainsRule.m_seaLevelReplace = BLOCK_TYPE_ICE;
	SnowPlainsRule.m_defaltTemperature = -10.f;
	g_biomeRules.insert({Biome::SNOW_PLAINS, SnowPlainsRule});

	BlockReplaceRules ForestRule;
	ForestRule.m_surfaceReplace = BLOCK_TYPE_GRASS;
	ForestRule.m_trees.push_back(BLOCK_TYPE_OAKLEAVES);
	ForestRule.m_trees.push_back(BLOCK_TYPE_BIRCHLEAVES);
	ForestRule.m_treeChance=.03f;
	ForestRule.m_defaltTemperature = 20.f;
	g_biomeRules.insert({Biome::FOREST, ForestRule});

	BlockReplaceRules JungleRule;
	JungleRule.m_surfaceReplace = BLOCK_TYPE_DARK_GRASS;
	JungleRule.m_trees.push_back(BLOCK_TYPE_JUNGLELEAVES);
	JungleRule.m_treeChance = .03f;
	JungleRule.m_defaltTemperature = 35.f;
	g_biomeRules.insert({Biome::JUNGLE, JungleRule});

	BlockReplaceRules TaigaRule;
	TaigaRule.m_surfaceReplace = BLOCK_TYPE_LIGHT_GRASS;
	TaigaRule.m_trees.push_back(BLOCK_TYPE_SPRUCELEAVES);
	TaigaRule.m_treeChance = .005f;

	g_biomeRules.insert({Biome::TAIGA, TaigaRule});

	BlockReplaceRules SnowTaigaRule;
	SnowTaigaRule.m_surfaceReplace = BLOCK_TYPE_SNOW;
	SnowTaigaRule.m_seaLevelReplace = BLOCK_TYPE_ICE;
	SnowTaigaRule.m_trees.push_back(BLOCK_TYPE_SPRUCELEAVESSNOW);
	SnowTaigaRule.m_treeChance = 0.003f;
	SnowTaigaRule.m_defaltTemperature = -20.f;
	g_biomeRules.insert({Biome::SNOW_TAIGA, SnowTaigaRule});

	BlockReplaceRules StonePeakRule;
	StonePeakRule.m_hasDirtLayer = false;
	StonePeakRule.m_defaltTemperature = 1.f;
	g_biomeRules.insert({Biome::STONE_PEAK, StonePeakRule});

	BlockReplaceRules SnowPeakRule;
	SnowPeakRule.m_hasDirtLayer = false;
	SnowPeakRule.m_surfaceReplace = BLOCK_TYPE_SNOW;
	SnowPeakRule.m_seaLevelReplace = BLOCK_TYPE_ICE;
	SnowPeakRule.m_defaltTemperature = -50.f;
	g_biomeRules.insert({Biome::SNOW_PEAK, SnowPeakRule});

}

float Chunk::GetDensity(float x, float y, float z, float continentalness)
{
    float n = Compute3dPerlinNoise(x,y,z,DEFAULT_DENSITY_NOISE_SCALE, DEFAULT_DENSITY_NOISE_OCTAVE, 
	DEFAULT_OCTAVE_PERSISTANCE, DEFAULT_NOISE_OCTAVE_SCALE, true, GAME_SEED);

	float b = DEFAULT_BIAS_PER_Z *(z-DEFAULT_TERRAIN_HEIGHT);

	float h = g_Continent_Height_Offset_Curve->Evaluate(continentalness);

	float s = g_Continent_Squashing_Curve->Evaluate(continentalness);

	float b2 = DEFAULT_TERRAIN_HEIGHT + (h * CHUNK_SIZE_Z/2.f);

	float t = (z - b2)/b2;

	return n + b - h + ( s * t);
}

void Chunk::CreateTree(int x, int y, int z, BlockType leafType)
{
    BlockType log = BLOCK_TYPE_AIR;
	BlockType leaves = leafType;

	int treeHeight = 3;
	int leaveHeight = 3;
	int leaveWidth = 2;

	if (leafType == BLOCK_TYPE_CACTUSLOG) {
	    log = leafType;
		leaves = BLOCK_TYPE_AIR;
		treeHeight = 2;
	}

	if (leafType == BLOCK_TYPE_ACACIALEAVES) {
		log = BLOCK_TYPE_ACACIALOG;
		leaveWidth = 3;

	}

	if (leafType == BLOCK_TYPE_BIRCHLEAVES) {
		log = BLOCK_TYPE_BIRCHLOG;
	}

	if (leafType == BLOCK_TYPE_OAKLEAVES) {
		log = BLOCK_TYPE_OAKLOG;
	}

	if (leafType == BLOCK_TYPE_SPRUCELEAVES || leafType == BLOCK_TYPE_SPRUCELEAVESSNOW) {
		log = BLOCK_TYPE_SPRUCELOG;
		treeHeight = 8;
		leaveHeight = 6;
	}

	if (leafType == BLOCK_TYPE_JUNGLELEAVES) {
		log = BLOCK_TYPE_JUNGLELOG;
		leaveWidth = 3;
		treeHeight = 10;
	}



	for (int localZ = 1; localZ <= treeHeight; localZ++) {
	    int index = CoordToIndex(Vec3((float)x, (float)y, (float)(z + localZ)));
		m_blocks[index].m_type = (uint8_t)log;
	}

	if (leafType == BLOCK_TYPE_SPRUCELEAVES || leafType == BLOCK_TYPE_SPRUCELEAVESSNOW) {
		bool shrink = true;
		for (int localZ = 1; localZ <= leaveHeight; localZ++) {
			for (int localX = -leaveWidth; localX <= leaveWidth; localX++) {
				for (int localY = -(leaveWidth); localY <= leaveWidth; localY++) {

					if (CoordInBound(Vec3((float)(x + localX), (float)(y + localY), (float)(z + localZ + treeHeight)))) {
						int index = CoordToIndex(Vec3((float)(x + localX), (float)(y + localY), (float)(z + localZ + treeHeight)));
						m_blocks[index].m_type = (uint8_t)leaves;
					}

				}
			}
			if(shrink)
			    leaveWidth=1;
			else
			    leaveWidth=2;
			shrink=!shrink;
		}
	}
	else if (leaves == BLOCK_TYPE_JUNGLELEAVES||leaves == BLOCK_TYPE_ACACIALEAVES) {
		for (int localZ = 1; localZ <= leaveHeight; localZ++) {
			for (int localX = -leaveWidth; localX <= leaveWidth; localX++) {
				for (int localY = -(leaveWidth); localY <= leaveWidth; localY++) {

					if (CoordInBound(Vec3((float)(x + localX), (float)(y + localY), (float)(z + localZ + treeHeight)))) {
						int index = CoordToIndex(Vec3((float)(x + localX), (float)(y + localY), (float)(z + localZ + treeHeight)));
						m_blocks[index].m_type = (uint8_t)leaves;
					}

				}
			}
			leaveWidth-=2;
		}
	}
	else{
		for (int localZ = 1; localZ <= leaveHeight; localZ++) {
			for (int localX = -leaveWidth; localX <= leaveWidth; localX++) {
				for (int localY = -(leaveWidth)+abs(localX); localY <= leaveWidth - abs(localX); localY++) {

					if (CoordInBound(Vec3((float)(x + localX), (float)(y + localY), (float)(z + localZ + treeHeight)))) {
						int index = CoordToIndex(Vec3((float)(x + localX), (float)(y + localY), (float)(z + localZ + treeHeight)));
						m_blocks[index].m_type = (uint8_t)leaves;
					}

				}
			}
			leaveWidth--;
		}
	}

	
}

Block* Chunk::GetBlockAtPos(IntVec3 pos)
{

    int x  = pos.x;
    int y  = pos.y;
    int z  = pos.z;
	Chunk* targetChunk = this;
	if (x < 0) {
	    targetChunk = m_nearbyChunk->West;
		x=CHUNK_SIZE_X-1;
	}
	if (x >= CHUNK_SIZE_X) {
	    targetChunk = m_nearbyChunk->East;
		x=0;
	}
	if (y < 0) {
	     targetChunk = m_nearbyChunk->South;
		 y=CHUNK_SIZE_Y-1;
	}
	if (y >= CHUNK_SIZE_Y) {
	    targetChunk = m_nearbyChunk->North;
		y = 0;
	}

	if (z < 0 || z >= CHUNK_SIZE_Z) {
	    targetChunk = nullptr;
	}

	if (targetChunk&&targetChunk->m_chunkState==ChunkState::ACTIVE) {
	    return &targetChunk->m_blocks[CoordToIndex(Vec3((float)x, (float)y, (float)z))];
	}
	else {

		return nullptr;
	}
}

float Chunk::GetTemperatureAtPos(IntVec3 pos)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;
	Chunk* targetChunk = this;
	if (x < 0) {
		targetChunk = m_nearbyChunk->West;
		x = CHUNK_SIZE_X - 1;
	}
	if (x >= CHUNK_SIZE_X) {
		targetChunk = m_nearbyChunk->East;
		x = 0;
	}
	if (y < 0) {
		targetChunk = m_nearbyChunk->South;
		y = CHUNK_SIZE_Y - 1;
	}
	if (y >= CHUNK_SIZE_Y) {
		targetChunk = m_nearbyChunk->North;
		y = 0;
	}

	if (z < 0 || z >= CHUNK_SIZE_Z) {
		targetChunk = nullptr;
	}

	if (targetChunk && targetChunk->m_chunkState == ChunkState::ACTIVE) {
		return targetChunk->m_temperature[CoordToIndex(IntVec3(x, y, z))];
	}
	else {
		return FLT_MAX;
	}
}

void Chunk::SetTemperatureAtPos(IntVec3 pos, float temperature)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;
	Chunk* targetChunk = this;
	if (x < 0) {
		targetChunk = m_nearbyChunk->West;
		x = CHUNK_SIZE_X - 1;
	}
	if (x >= CHUNK_SIZE_X) {
		targetChunk = m_nearbyChunk->East;
		x = 0;
	}
	if (y < 0) {
		targetChunk = m_nearbyChunk->South;
		y = CHUNK_SIZE_Y - 1;
	}
	if (y >= CHUNK_SIZE_Y) {
		targetChunk = m_nearbyChunk->North;
		y = 0;
	}

	if (z < 0 || z >= CHUNK_SIZE_Z) {
		targetChunk = nullptr;
	}

	if (targetChunk && targetChunk->m_chunkState == ChunkState::ACTIVE) {
		targetChunk->m_temperature[CoordToIndex(IntVec3(x, y, z))] = temperature;
	}
}

BlockIterator Chunk::GetBlockIteratorAtPos(IntVec3 pos)
{
	Chunk* targetChunk = this;
	if (pos.x < 0) {
		targetChunk = m_nearbyChunk->West;
		pos.x = CHUNK_SIZE_X - 1;
	}
	if (pos.x >= CHUNK_SIZE_X) {
		targetChunk = m_nearbyChunk->East;
		pos.x = 0;
	}
	if (pos.y < 0) {
		targetChunk = m_nearbyChunk->South;
		pos.y = CHUNK_SIZE_Y - 1;
	}
	if (pos.y >= CHUNK_SIZE_Y) {
		targetChunk = m_nearbyChunk->North;
		pos.y = 0;
	}

	if (pos.z < 0 || pos.z >= CHUNK_SIZE_Z) {
		targetChunk = nullptr;
	}

	BlockIterator result;
	result.chunk = targetChunk;
	result.pos = pos;
	return result;
}

std::vector<BlockIterator> Chunk::GetNearby(IntVec3 pos)
{
    std::vector<BlockIterator> result;
    BlockIterator west = GetBlockIteratorAtPos(pos + IntVec3(1, 0, 0));
	if (west.chunk&&west.chunk->m_chunkState == ChunkState::ACTIVE) {
	    result.push_back(west);
	}

	BlockIterator east = GetBlockIteratorAtPos(pos + IntVec3(-1, 0, 0));
	if (east.chunk&&east.chunk->m_chunkState == ChunkState::ACTIVE) {
		result.push_back(east);
	}

	BlockIterator north = GetBlockIteratorAtPos(pos + IntVec3(0, 1, 0));
	if (north.chunk&&north.chunk->m_chunkState == ChunkState::ACTIVE) {
		result.push_back(north);
	}

	BlockIterator south = GetBlockIteratorAtPos(pos + IntVec3(0, -1, 0));
	if (south.chunk&&south.chunk->m_chunkState == ChunkState::ACTIVE) {
		result.push_back(south);
	}

	BlockIterator up = GetBlockIteratorAtPos(pos + IntVec3(0, 0, 1));
	if (up.chunk&&up.chunk->m_chunkState == ChunkState::ACTIVE) {
		result.push_back(up);
	}

	BlockIterator down = GetBlockIteratorAtPos(pos + IntVec3(0, 0, -1));
	if (down.chunk&&down.chunk->m_chunkState == ChunkState::ACTIVE) {
		result.push_back(down);
	}

	return result;
}

bool Chunk::ReadyToRegenerate()
{
	if (!m_nearbyChunk->North||m_nearbyChunk->North->m_chunkState!=ChunkState::ACTIVE) {
	    return false;
	}

	if (!m_nearbyChunk->South || m_nearbyChunk->South->m_chunkState != ChunkState::ACTIVE) {
		return false;
	}

	if (!m_nearbyChunk->East || m_nearbyChunk->East->m_chunkState != ChunkState::ACTIVE) {
		return false;
	}

	if (!m_nearbyChunk->West || m_nearbyChunk->West->m_chunkState != ChunkState::ACTIVE) {
		return false;
	}
    return m_chunkState == ChunkState::ACTIVE&&m_dirty;
}

void Chunk::InitializeLight()
{
    m_skyHeight.resize(CHUNK_SIZE_X*CHUNK_SIZE_Y);
    

	for (int y = 0; y < CHUNK_SIZE_Y; y++) {
		for (int x = 0; x < CHUNK_SIZE_X; x++) {

			int idxXY = x + y * CHUNK_SIZE_X;

			bool sky = true;

			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--) {

			    //int idx = CoordToIndex(Vec3((float)x, (float)y, (float)z));

				Block* b = &m_blocks[CoordToIndex(Vec3((float)x, (float)y, (float)z))];
				b->SetIndoor(0);
				b->SetOutdoor(0);
				if (sky) {
					if (!b->m_type == BLOCK_TYPE_AIR) {
					    
						sky = false;
						m_skyHeight[idxXY] = z+1;
					}
				}

				b->SetIsSky(sky);
                
				if (sky) {
					if (m_rainning) {
						b->SetOutdoor(5);
					}
					else if (m_humidity > 15) {
						b->SetOutdoor(10);
					}
					else {
					    b->SetOutdoor(15);
					}
				    

					BlockIterator bi;
					bi.chunk = this;
					bi.pos = IntVec3(x, y, z);
					m_owner->m_dirtyLightOutDoor.push_back(bi);
				}

				if (b->m_type == BLOCK_TYPE_GLOWSTONE) {
				    
					b->SetIndoor(15);

				    BlockIterator bi;
					bi.chunk = this;
					bi.pos = IntVec3(x ,y, z);
					m_owner->m_dirtyLightInDoor.push_back(bi);

				}

			}
		}
	}

	
	
}

Rgba8 Chunk::GetWorldColor(Rgba8 input, int opacity)
{
    Vec3 OutDoorLightingColor(255.f, 255.f, 255.f);
	Vec3 InDoorLightingColor(255.f, 230.f, 204.f);
	Vec3 outDoor = input.r/15.f * OutDoorLightingColor;
	Vec3 inDoor = input.g/15.f * InDoorLightingColor;
	Vec3 white(1.f, 1.f, 1.f);
	Vec3 finalColor = white - (white - outDoor/255.f) * (white - inDoor/255.f);

	return Rgba8((unsigned char)roundf(finalColor.x*255.f), (unsigned char)roundf(finalColor.y*255.f), (unsigned char)roundf(finalColor.z*255.f),(unsigned char)opacity);
	
}

int Chunk::FindFirstSolidBlockBelow(IntVec3 coord)
{
    int x = coord.x;
	int y = coord.y;

	for (int z = coord.z; z >= 0; z--) {
		Block* block = GetBlockAtPos(IntVec3(x,y,z));
		if (block&&block->m_type != BLOCK_TYPE_AIR&&block->m_type != BLOCK_TYPE_WATER) {
			return z;
		}
	}
	return 0;
}

void Chunk::HandleTemperature(float deltaSeconds)
{
	for (int z = 0; z < CHUNK_SIZE_Z; z++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int x = 0; x < CHUNK_SIZE_X; x++) {
			    IntVec3 pos(x, y, z);
				HeatExchangeAtPos(pos, deltaSeconds);
			    AdjustToDefault(pos, deltaSeconds);
			}
		}
	}

	for (int z = 0; z < CHUNK_SIZE_Z; z++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int x = 0; x < CHUNK_SIZE_X; x++) {
				int idx = CoordToIndex(Vec3((float)x, (float)y, (float)z));
				uint8_t prevType = m_blocks[idx].m_type;

				if (prevType == BLOCK_TYPE_WATER && m_temperature[idx] <= 10.f && z>0&& m_blocks[CoordToIndex(Vec3((float)x, (float)y, (float)z - 1))].m_type == BLOCK_TYPE_AIR) {
				    m_blocks[idx].m_type = BLOCK_TYPE_SNOW;
					m_dirty = true;
				}
				else {
					if (m_blocks[idx].SetNewTemperature(m_temperature[idx])) {
						m_dirty = true;
					}
				}

				
				if (prevType == BLOCK_TYPE_WATER && m_blocks[idx].m_type == BLOCK_TYPE_AIR) {
					m_humidity++;
				}
			}
		}
	}
}

void Chunk::HeatExchangeAtPos(IntVec3 pos, float deltaSeconds)
{
    HeatExchangeBetweenPos(pos, pos + IntVec3(1, 0, 0), deltaSeconds);
	HeatExchangeBetweenPos(pos, pos + IntVec3(-1, 0, 0), deltaSeconds);
	HeatExchangeBetweenPos(pos, pos + IntVec3(0, 1, 0), deltaSeconds);
	HeatExchangeBetweenPos(pos, pos + IntVec3(0, -1, 0), deltaSeconds);
	HeatExchangeBetweenPos(pos, pos + IntVec3(0, 0, 1), deltaSeconds);
	HeatExchangeBetweenPos(pos, pos + IntVec3(0, 0, -1), deltaSeconds);

}

void Chunk::HeatExchangeBetweenPos(IntVec3 from, IntVec3 to, float deltaSeconds)
{
    float fromTemp = GetTemperatureAtPos(from);
	float toTemp = GetTemperatureAtPos(to);
	if (toTemp != FLT_MAX||fabsf(fromTemp-toTemp)<1.f) {
		float exchange = (fromTemp - toTemp) * deltaSeconds*.1f;
		SetTemperatureAtPos(from, fromTemp - exchange);
		SetTemperatureAtPos(to, toTemp + exchange);
	}
}

void Chunk::AdjustToDefault(IntVec3 pos, float deltaSeconds)
{
    float exchange = ( GetTemperatureAtPos(pos) - m_defaultTemperature[CoordToIndex(pos)]) * deltaSeconds * .01f;
	SetTemperatureAtPos(pos, GetTemperatureAtPos(pos) - exchange);
}

void Chunk::HandleFluidPhysics()
{
    std::vector<bool> moved;
	moved.resize(CHUNK_SIZE_X*CHUNK_SIZE_Y*CHUNK_SIZE_Z);
	for (int i = 0; i < moved.size(); i++) {
		moved[i] = false;
	}

	for (int z = 0; z < CHUNK_SIZE_Z; z++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int x = 0; x < CHUNK_SIZE_X; x++) {
				
				int idx = CoordToIndex(Vec3((float)x, (float)y, (float)z));
				IntVec3 pos(x, y, z);
				if (!moved[idx]) {
				    Block b = m_blocks[idx];
					BlockDefinition* def = BlockDefinition::g_blockDefinitions[b.m_type];
					if (def->m_liquid) {
					    IntVec3 newPos = LiquidPhysics(pos);
						if (newPos != pos) {
							ExchangePosition(pos, newPos);
							moved[CoordToIndex(Vec3((float)newPos.x, (float)newPos.y, (float)newPos.z))] = true;
							m_dirty = true;
						}
					}
					
				}

			}
		}
	}
}

void Chunk::HandleFallingPhysics()
{
	for (int z = 0; z < CHUNK_SIZE_Z; z++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int x = 0; x < CHUNK_SIZE_X; x++) {

				int idx = CoordToIndex(Vec3((float)x, (float)y, (float)z));
				IntVec3 pos(x, y, z);

					Block b = m_blocks[idx];
					BlockDefinition* def = BlockDefinition::g_blockDefinitions[b.m_type];
					if (def->m_liquid||def->m_powder) {
						IntVec3 newPos = PowderPhysics(pos);
						if (newPos != pos) {
							ExchangePosition(pos, newPos);
							m_dirty = true;
						}
					}

				

			}
		}
	}
}

IntVec3 Chunk::LiquidPhysics(IntVec3 pos)
{
    std::vector<IntVec3> possiblePosition;

	if (GetBlockAtPos(pos + IntVec3(1, 0, 0))&&GetBlockAtPos(pos + IntVec3(1, 0, 0))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(1, 0, 0));
	}

	if (GetBlockAtPos(pos + IntVec3(-1, 0, 0))&&GetBlockAtPos(pos + IntVec3(-1, 0, 0))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(-1, 0, 0));
	}

	if (GetBlockAtPos(pos + IntVec3(0, 1, 0))&&GetBlockAtPos(pos + IntVec3(0, 1, 0))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(0, 1, 0));
	}

	if (GetBlockAtPos(pos + IntVec3(0, -1, 0))&&GetBlockAtPos(pos + IntVec3(0, -1, 0))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(0, -1, 0));
	}

	if(!possiblePosition.empty()){
	    return possiblePosition[g_RNG->RollRandomIntLessThan((int)possiblePosition.size())];
	}

    return pos;
}

IntVec3 Chunk::PowderPhysics(IntVec3 pos)
{
/*
	std::vector<IntVec3> possiblePosition;

	if (GetBlockAtPos(pos + IntVec3(1, 0, -1))&&GetBlockAtPos(pos + IntVec3(1, 0, -1))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(1, 0, -1));
	}

	if (GetBlockAtPos(pos + IntVec3(-1, 0, -1))&&GetBlockAtPos(pos + IntVec3(-1, 0, -1))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(-1, 0, -1));
	}

	if (GetBlockAtPos(pos + IntVec3(0, 1, -1))&&GetBlockAtPos(pos + IntVec3(0, 1, -1))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(0, 1, -1));
	}

	if (GetBlockAtPos(pos + IntVec3(0, -1, -1))&&GetBlockAtPos(pos + IntVec3(0, -1, -1))->m_type == BLOCK_TYPE_AIR) {
		possiblePosition.push_back(pos + IntVec3(0, -1, -1));
	}*/

	if (GetBlockAtPos(pos + IntVec3(0, 0, -1))&&GetBlockAtPos(pos + IntVec3(0, 0, -1))->m_type == BLOCK_TYPE_AIR) {
		return (pos + IntVec3(0, 0, -1));
	}

/*
	if (!possiblePosition.empty()) {
		return possiblePosition[g_RNG->RollRandomIntLessThan(possiblePosition.size())];
	}*/

	return pos;
}

void Chunk::ExchangePosition(IntVec3 from, IntVec3 to)
{
    //int fromidx = CoordToIndex(from);
	//int toidx = CoordToIndex(to);

	Block* blockFrom = GetBlockAtPos(from);
	Block* blockTo = GetBlockAtPos(to);

	uint8_t fromType = blockFrom->m_type;
	float fromTemperature = GetTemperatureAtPos(from);

	blockFrom->m_type = blockTo->m_type;
	SetTemperatureAtPos(from, GetTemperatureAtPos(to));

	blockTo->m_type = fromType;
	SetTemperatureAtPos(to, fromTemperature);
}

void Chunk::HandleHumidity()
{
    m_totalTemp = 0.f;
	for (int i = CHUNK_SIZE_X*CHUNK_SIZE_Y; i < m_temperature.size(); i++) {
	    m_totalTemp+=m_temperature[i];
	}

	std::vector<Chunk*> nearbyChunks;
	
	Chunk* north = m_nearbyChunk->North;
	if ( north && north->m_chunkState == ChunkState::ACTIVE) {
		nearbyChunks.push_back(north);
	}

	Chunk* south = m_nearbyChunk->South;
	if (south && south->m_chunkState == ChunkState::ACTIVE) {
		nearbyChunks.push_back(south);
	}

	Chunk* west = m_nearbyChunk->West;
	if (west && west->m_chunkState == ChunkState::ACTIVE) {
		nearbyChunks.push_back(west);
	}

	Chunk* east = m_nearbyChunk->East;
	if (east && east->m_chunkState == ChunkState::ACTIVE) {
		nearbyChunks.push_back(east);
	}

	for (Chunk* chunk : nearbyChunks) {
		if (chunk->m_totalTemp < m_totalTemp&& chunk->m_humidity > 0) {
			for (int i = 0; i < chunk->m_humidity; i++) {
				if (g_RNG->RollRandomIntLessThan((int)(m_totalTemp - chunk->m_totalTemp)) > m_owner->m_windThreshold) {
				    chunk->m_humidity--;
					m_humidity++;
				}
			}
		}
	}


}

void Chunk::HandleRain()
{
	if (m_humidity > m_owner->m_rainThreshold) {
		m_rainning= true;
	}

	if (m_humidity == 0) {
		m_rainning = false;
	}

	if (m_rainning) {
		int xPox = g_RNG->RollRandomIntLessThan(CHUNK_SIZE_X);
		int yPos = g_RNG->RollRandomIntLessThan(CHUNK_SIZE_Y);

		int idx = CoordToIndex(IntVec3(xPox, yPos, CHUNK_SIZE_Z - 1));

		if (m_owner->m_debugRainLava) {
			m_blocks[idx].m_type = BLOCK_TYPE_LAVA;
			m_temperature[idx] = 5000.f;
		}
		else {
			m_blocks[idx].m_type = BLOCK_TYPE_WATER;
			m_temperature[idx] = 10.f;
		}

		m_dirty = true;

		m_humidity--;
	}
}

void Chunk::DebugAdjustTemp(float newTemperature)
{
	for (int i = 0; i < m_temperature.size(); i++) {
		m_temperature[i] = newTemperature;
	}
}

void Chunk::DebugAdjustHumidity(int newHumidity)
{
    m_humidity = newHumidity;
}

