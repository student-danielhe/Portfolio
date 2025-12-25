#include "Map.hpp"
#include "Chunk.hpp"
#include "Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "GenerateChunkJob.hpp"
Map::Map()
{
	AddVertsForWorldBasis(m_worldBasis, 3.f);
	AddVertsForWorldBasis(m_playerBasis, 5.f);
	m_liquidFlowTimer = new Timer(PHYSICS_FRAMERATE*20.f, g_gameClock);
	m_liquidFlowTimer->Start();
	m_physicsTimer = new Timer(PHYSICS_FRAMERATE, g_gameClock);
	m_physicsTimer->Start();

	m_temperatureTimer = new Timer(PHYSICS_FRAMERATE*20.f, g_gameClock);
	m_temperatureTimer->Start();
}

Map::~Map()
{
	for (auto pair : m_activeChunks) {
		delete pair.second;
	}
    m_activeChunks.clear();
}

void Map::Update(float deltaSeconds)
{
    HandleKeyPressed();
	if (m_worldTimeFast) {
		m_worldTime+=deltaSeconds*50000.f/(60.f*60.f*24.f);
	}
	else {
		m_worldTime+=deltaSeconds*500.f/(60.f*60.f*24.f);
	}

	while (m_worldTime > 1.f) {
		m_worldTime -= 1.f;
	}
	/*for (auto pair : m_pendingChunks) {
		if (pair.second->m_chunkState == ChunkState::ACTIVATING_GENERATION_COMPLETE) {
			m_activeChunks.insert(pair);
			m_pendingChunks.erase(pair.first);
			pair.second->m_chunkState = ChunkState::ACTIVE;
		}
	}*/

	std::vector <Job*> completedJob = g_jobSystem->ClaimCompletedJobs();

	for (Job* job : completedJob) {
	     GenerateChunkJob* genJob = (GenerateChunkJob*)job;
		Chunk* newChunk = genJob->m_chunk;
		IntVec2 pos = newChunk->m_coords;
		newChunk->m_chunkState = ChunkState::ACTIVE;
		m_activeChunks.insert({pos, newChunk});
		newChunk->InitializeLight();
		//newChunk->InitializeLight();
		m_pendingChunks.erase(pos);
	}
	completedJob.clear();
	
	ProcessDirtyLighting();
	
	RegenerateNearestDirty();
	
	//RegenerateNearestDirty();
	for (int jpf = (int)m_pendingChunks.size(); jpf < MAX_JOB_PER_FRAME; jpf++) {
		
		if (m_activeChunks.size()+m_pendingChunks.size() < MAX_ACTIVE_CHUNKS) {
		    ActivateNearest();
		}
		else {
			DeactiveFurthest();
			break;
		}
	}

	Chunk* currentChunk = g_theGame->m_player->m_character->GetCurrentChunk();

	if (currentChunk && currentChunk->m_chunkState == ChunkState::ACTIVE) {
		std::vector<Chunk*> tempChunk;
		tempChunk = GetAllNearbyChunkWithinRange(3, currentChunk);


	    std::vector<Chunk*> nearByChunk;
		nearByChunk = GetAllNearbyChunkWithinRange(7, currentChunk);

		if (m_temperatureTimer->DecrementPeriodIfElapsed()) {
			for (Chunk* chunk : tempChunk) {
				chunk->HandleTemperature(PHYSICS_FRAMERATE * 20.f);
			}
		}

		
		if (m_liquidFlowTimer->DecrementPeriodIfElapsed()) {
			

			for (Chunk* chunk : nearByChunk) {
		        chunk->HandleFluidPhysics();
			}
		}



		if (m_physicsTimer->DecrementPeriodIfElapsed()) {
			for (Chunk* chunk : nearByChunk) {
				chunk->HandleHumidity();
			}
		    for (Chunk* chunk : nearByChunk) {
			    chunk->Update(PHYSICS_FRAMERATE);
				chunk->HandleRain();
			}
		}

		for (Chunk* chunk : nearByChunk) {
		    if(chunk->m_dirty)
		        chunk->Regenerate();
		}

		
	}
		
	
}

void Map::HandleKeyPressed()
{
	




	if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
	    
		IntVec2 playerCoord = GetPlayerCoord();
		if (m_activeChunks.size() != 0 && m_activeChunks.find(playerCoord)!=m_activeChunks.end()) {
			Chunk* chunk = m_activeChunks.find(playerCoord)->second;
			if(chunk->m_chunkState == ChunkState::ACTIVE)
			    chunk->Dig(GetSurfaceBlock());
		}
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_RIGHT_MOUSE)) {
		IntVec2 playerCoord = GetPlayerCoord();
		if (m_activeChunks.size() != 0 && m_activeChunks.find(playerCoord) != m_activeChunks.end()) {
			Chunk* chunk = m_activeChunks.find(playerCoord)->second;
			if (chunk->m_chunkState == ChunkState::ACTIVE) {
			    Vec3 box = GetSurfaceBlock();
			    chunk->Place(Vec3(box.x, box.y, box.z+1), m_placeingType);
			}

			    
		}
	}

	if (g_theInput->WasKeyJustReleased('1')) {
		m_placeingType=BLOCK_TYPE_WATER;
	}
	if (g_theInput->WasKeyJustReleased('2')) {
		m_placeingType = BLOCK_TYPE_LAVA;
	}
	if (g_theInput->WasKeyJustReleased('3')) {
		m_placeingType = BLOCK_TYPE_SNOW;
	}

	m_worldTimeFast = g_theInput->WasKeyJustPressed('Y');

	
	
}

void Map::RegenerateNearestDirty()
{
	Chunk* nearestDirty = nullptr;
	float dist = FLT_MAX;
	for (auto pair: m_activeChunks) {
	    Chunk* chunk = pair.second;
		if(chunk->ReadyToRegenerate()){
			if (chunk->GetRangeToPlayerSquared() < dist) {
				nearestDirty = chunk;
				dist = chunk->GetRangeToPlayerSquared();
			}
		}
	}

	if (nearestDirty) {
	    //nearestDirty->InitializeLight();
		nearestDirty->Regenerate();
	}
}

void Map::ActivateNearest()
{
	
    
	for (int dist = 0; dist < CHUNK_ACTIVATION_RANGE; dist++) {

		for (int x = -dist; x <= dist; x++) {
		    
		    int y = dist-abs(x);

			if (!GetChunkAtPos(IntVec2(GetPlayerCoord().x + x, GetPlayerCoord().y + y))) {
			    ActivateChunk(IntVec2(GetPlayerCoord().x + x, GetPlayerCoord().y + y));
				return;
			}
			if (!GetChunkAtPos(IntVec2(GetPlayerCoord().x + x, GetPlayerCoord().y - y))) {
			    ActivateChunk(IntVec2(GetPlayerCoord().x + x, GetPlayerCoord().y - y));
				return;
			}
		}
	}
}

Chunk* Map::GetChunkAtPos(IntVec2 pos)
{
	std::map< IntVec2, Chunk* >::iterator found = m_pendingChunks.find(pos);
	if (found != m_pendingChunks.end())
	{
		return found->second;
	}
	std::map< IntVec2, Chunk* >::iterator activefound = m_activeChunks.find(pos);
	if (activefound != m_activeChunks.end())
	{
		return activefound->second;
	}
	return nullptr;
}

IntVec2 Map::GetPlayerCoord()
{
    return IntVec2((int)(g_theGame->m_player->m_position.x/CHUNK_SIZE_X) - 1, (int)(g_theGame->m_player->m_position.y/CHUNK_SIZE_Y) - 1);
}

IntVec2 Map::GetPlayerChunkCoord()
{
    IntVec2 result((int)(g_theGame->m_player->m_position.x - GetPlayerCoord().x*CHUNK_SIZE_X), (int)(g_theGame->m_player->m_position.y - GetPlayerCoord().y*CHUNK_SIZE_Y));
	if (result.x >= CHUNK_SIZE_X) {
		result.x = CHUNK_SIZE_X-1;
	}
	if (result.y >= CHUNK_SIZE_Y) {
		result.y = CHUNK_SIZE_Y-1;
	}
    return result;
}

Vec3 Map::GetSurfaceBlock()
{
	IntVec2 playerCoord = GetPlayerCoord();
	if (m_activeChunks.find(playerCoord) != m_activeChunks.end()) {
		Chunk* chunk = m_activeChunks.find(playerCoord)->second;
		if(chunk->m_chunkState ==ChunkState::ACTIVE){
		    int x = GetPlayerChunkCoord().x;
		    int y = GetPlayerChunkCoord().y;
		    for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--) {
		    	if (chunk->m_blocks[chunk->CoordToIndex(Vec3((float)x, (float)y, (float)z))].m_type != BLOCK_TYPE_AIR) {
		    		return Vec3((float)x, (float)y, (float)z);
		    	}
		    }
		}
	}
	
	return Vec3();
	
}







Block* Map::GetBlockFromIterator(BlockIterator iterator)
{
	if (iterator.chunk->m_chunkState != ChunkState::ACTIVE) {
	    return nullptr;
	}
	int i = iterator.chunk->CoordToIndex(Vec3((float)iterator.pos.x, (float)iterator.pos.y, (float)iterator.pos.z));
	
    return &iterator.chunk->m_blocks[i];
}


void Map::ProcessDirtyLighting()
{
    ProcessIndoor();
	ProcessOutdoor();
}





void Map::ProcessIndoor()
{
    int light = 15;
	while (!m_dirtyLightInDoor.empty() && light > 0) {
	    m_dirtyLightInDoor = SpreadIndoor(m_dirtyLightInDoor, light);
		light--;
	}
}

std::vector<BlockIterator> Map::SpreadIndoor(std::vector<BlockIterator> input, int lightVal)
{
    std::vector<BlockIterator> results;
	
	for (BlockIterator bi : input) {
	    
		std::vector<BlockIterator> nearby = bi.chunk->GetNearby(bi.pos);
		
		for (BlockIterator adj : nearby) {
			
			Block* b = GetBlockFromIterator(adj);
			
			if (b && b->GetIndoor() < lightVal - 1) {
				b->SetIndoor(lightVal-1);
				results.push_back(adj);
			}
		}
	}

	return results;
}

void Map::ProcessOutdoor()
{
	int light = 15;
	while (!m_dirtyLightOutDoor.empty() && light > 0) {
		m_dirtyLightOutDoor = SpreadOutdoor(m_dirtyLightOutDoor, light);
		light--;
	}
}

std::vector<BlockIterator> Map::SpreadOutdoor(std::vector<BlockIterator> input, int lightVal)
{
	std::vector<BlockIterator> results;

	for (BlockIterator bi : input) {

		std::vector<BlockIterator> nearby = bi.chunk->GetNearby(bi.pos);

		for (BlockIterator adj : nearby) {

			Block* b = GetBlockFromIterator(adj);

			if (b && b->GetOutdoor() < lightVal - 1) {
				b->SetOutdoor(lightVal - 1);
				results.push_back(adj);
			}
		}
	}

	return results;
}

std::vector<Chunk*> Map::GetAllNearbyChunkWithinRange(int range, Chunk* current)
{
    std::vector<Chunk*>  result;
	for (auto pair : m_activeChunks) {
	    Chunk* other = pair.second;
		if (abs(other->m_coords.x - current->m_coords.x) + abs(other->m_coords.y - current->m_coords.y) < range) {
			result.push_back(other);
		}
	}

	return result;
}

void Map::DeactiveFurthest()
{
	IntVec2 furthestcoord(12345, 12345);
	float dist = 0.f;
	for (auto pair: m_activeChunks) {
	    Chunk* chunk = pair.second;
	    float distToPlayerSqr = chunk->GetRangeToPlayerSquared();
		if (chunk->m_chunkState==ChunkState::ACTIVE && distToPlayerSqr > dist && distToPlayerSqr > CHUNK_DEACTIVATION_RANGE* CHUNK_DEACTIVATION_RANGE) {
			furthestcoord = pair.first;
			dist = distToPlayerSqr;
		}
	}
	if (furthestcoord!= IntVec2(12345,12345)) {
		DeactivateChunk(furthestcoord);
	}
}

void Map::Render()
{
    g_theRenderer->BindShader(g_theRenderer->m_diffuseShader);
	g_theGame->m_player->m_character->Render();
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	for (auto pair: m_activeChunks) {

	    pair.second->Render();
	}
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetBuffers();
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
    g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	
	g_theRenderer->SetModelConstants();
    g_theRenderer->DrawVertexArray(m_worldBasis);
	
	Mat44 playerBasisPosition = Mat44::MakeTranslation3D(g_theGame->m_player->GetPosition() + g_theGame->m_player->GetFwdVector() * 50.f);
	g_theRenderer->SetModelConstants(playerBasisPosition);
	g_theRenderer->DrawVertexArray(m_playerBasis);

	RenderIMGUI();

	if (m_debugging) {
		std::vector<Vertex_PCU> cube;
		IntVec2 playerCoord = GetPlayerCoord();
		if (m_activeChunks.size() != 0) {
			if (m_activeChunks.find(playerCoord) != m_activeChunks.end()) {
				    
					Chunk* chunk = m_activeChunks.find(playerCoord)->second;
					if (chunk->m_chunkState == ChunkState::ACTIVE) {
						chunk->m_debugRenderHighlight = true;
						Vec3 blockcoord = GetSurfaceBlock();
						AddVertsForAABB3D(cube, AABB3(Vec3(blockcoord.x, blockcoord.y, blockcoord.z + 1), Vec3(blockcoord.x + 1, blockcoord.y + 1, blockcoord.z + 2)));
						Mat44 modelTransform = Mat44::MakeTranslation2D(chunk->m_position);
						g_theRenderer->SetModelConstants(modelTransform);

						g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);

						g_theRenderer->DrawVertexArray(cube);
					}

		    }
			
		}
		
		
	}

	
	
}

void Map::RenderIMGUI()
{
    ImGui::Begin("Settings");

	if (ImGui::CollapsingHeader("Global Setting")) {
		ImGui::SliderInt("Rain Threshold", &m_rainThreshold, 10, 100);
		ImGui::NewLine();
		ImGui::SliderInt("Wind Threshold", &m_windThreshold, 0, 1000);
		ImGui::NewLine();
		if (ImGui::Button("Toggle Lava Rain")) {
			m_debugRainLava = !m_debugRainLava;
		}
	}
	ImGui::NewLine();

	if (ImGui::CollapsingHeader("Current Chunk")) {
	    Chunk* currentChunk = g_theGame->m_player->m_character->GetCurrentChunk();

	    std::string tempertureText = "Current Chunk Average Temperature: " + std::to_string(currentChunk->m_totalTemp/(CHUNK_SIZE_X*CHUNK_SIZE_Y*(CHUNK_SIZE_Z-1)));
		ImGui::Text(tempertureText.c_str());
		ImGui::NewLine();

		ImGui::InputFloat("New Temperature", &m_debugSetTemp);
		ImGui::NewLine();

		if (ImGui::Button("Set Temperature")) {
			currentChunk->DebugAdjustTemp(m_debugSetTemp);
		}
		ImGui::NewLine();

		std::string humidityText = "Current Chunk Humidity: " + std::to_string(currentChunk->m_humidity);
		ImGui::Text(humidityText.c_str());
		ImGui::NewLine();

		ImGui::InputInt("New Humidity", &m_debugSetHumidity);
		ImGui::NewLine();

		if (ImGui::Button("Set Humidity")) {
			currentChunk->DebugAdjustHumidity(m_debugSetHumidity);
		}
	}
	
	ImGui::End();
}

void Map::BeginFrame()
{

}

void Map::EndFrame()
{

}

int Map::GetVertexCount()
{
    int result = 0;
	for (auto pair: m_activeChunks) {
	    Chunk* chunk = pair.second;
	    result += (int)chunk->m_verts.size();
	}
	return result;
}

int Map::GetIndexCount()
{
	int result = 0;
	for (auto pair: m_activeChunks) {
	    Chunk* chunk = pair.second;
		result += (int)chunk->m_indexes.size();
	}
	return result;
}


void Map::ActivateChunk(IntVec2 pos)
{
    Chunk* chunk =  new Chunk(this, Vec3((float)pos.x, (float)pos.y, 0.f));

	m_pendingChunks.insert({pos, chunk});
}

void Map::DeactivateChunk(IntVec2 coord)
{

	std::map< IntVec2, Chunk* >::iterator pendingfound = m_pendingChunks.find(coord);
	if (pendingfound != m_pendingChunks.end())
	{
		m_pendingChunks.erase(pendingfound);

	}
	std::map< IntVec2, Chunk* >::iterator found = m_activeChunks.find(coord);
	if (found != m_activeChunks.end())
	{
		delete found->second;
		m_activeChunks.erase(found);
		
	}
	

}

