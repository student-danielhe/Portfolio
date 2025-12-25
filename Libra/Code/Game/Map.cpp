#include "Map.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Aries.hpp"
#include "Leo.hpp"
#include "Scorpio.hpp"
#include "Bullet.hpp"
#include "Engine/Math/MathUtils.hpp"

float WORLD_SIZE_X;
float WORLD_SIZE_Y;
void Map::Render()const {
	//render tiles
	std::vector<Vertex_PCU> vertexArray;
	vertexArray.reserve(m_dimensions.x * m_dimensions.y * 6);
	g_theRenderer->BindTexture(&g_terrainSheet->GetTexture());
	for (Tile tile : m_tiles) {
		TileType type = tile.m_tileType;
		Rgba8 color=TileDefinition::s_definitions[type].m_TintColor;
		AABB2 UV = TileDefinition::s_definitions[type].m_SpriteSheetUV;
		
		float left = tile.m_tileCoords.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
		float bottom = tile.m_tileCoords.y* g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
		Vertex_PCU BL(Vec3(left, bottom,0), color, Vec2(UV.m_mins));
		Vertex_PCU BR(Vec3(left + g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), bottom,0), color, Vec2(UV.m_maxs.x, UV.m_mins.y));
		Vertex_PCU TR(Vec3(left + g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), bottom + g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), 0), color, Vec2(UV.m_maxs));
		Vertex_PCU TL(Vec3(left, bottom + g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), 0), color, Vec2(UV.m_mins.x,UV.m_maxs.y));
		AddVertsForRect(vertexArray, BL,BR,TR,TL);

	}

	g_theRenderer->DrawVertexArray(vertexArray);

	if (g_theGame->m_renderHeaMap) {
		std::vector<Vertex_PCU> verts;
		m_testHeatMap->AddVertsForDebugDraw(verts, AABB2(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y)), FloatRange(0, static_cast<float>(m_maxHeat)));
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray(verts);
	}
	
//_____________________________________________________________________________________________________________________________________________
	std::vector<Vertex_PCU> scorpioBases;
	Texture* scorpioBaseTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyTurretBase.png");
	g_theRenderer->BindTexture(scorpioBaseTexture);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO].size(); i++) {
		if (m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO][i] != nullptr)
			m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO][i]->RenderBase(scorpioBases);
	}
	g_theRenderer->DrawVertexArray(scorpioBases);

	std::vector<Vertex_PCU> scorpioVerts;
	Texture* scorpioTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyCannon.png");
	g_theRenderer->BindTexture(scorpioTexture);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO].size(); i++) {
		if (m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO][i] != nullptr)
			m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO][i]->Render(scorpioVerts);
	}
	g_theRenderer->DrawVertexArray(scorpioVerts);
	std::vector<Vertex_PCU> scorpioLaser;
	g_theRenderer->BindTexture(nullptr);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO].size(); i++) {
		if (m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO][i] != nullptr)
			m_entityListsByType[ENTITYTYPE_EVIL_SCORPIO][i]->RenderLaser(scorpioLaser);
	}
	g_theRenderer->DrawVertexArray(scorpioLaser);
	

	//render Leo
	std::vector<Vertex_PCU> leoVerts;
	Texture* leoTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyTank4.png");
	g_theRenderer->BindTexture(leoTexture);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_EVIL_LEO].size(); i++) {
		if (m_entityListsByType[ENTITYTYPE_EVIL_LEO][i] != nullptr)
		    m_entityListsByType[ENTITYTYPE_EVIL_LEO][i]->Render(leoVerts);
	}
	g_theRenderer->DrawVertexArray(leoVerts);
	//Render Aries
	std::vector<Vertex_PCU> ariesVerts;
	Texture* ariesTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyTank1.png");
	g_theRenderer->BindTexture(ariesTexture);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_EVIL_ARIES].size(); i++) {
		if(m_entityListsByType[ENTITYTYPE_EVIL_ARIES][i]!=nullptr)
		    m_entityListsByType[ENTITYTYPE_EVIL_ARIES][i]->Render(ariesVerts);
	}
	g_theRenderer->DrawVertexArray(ariesVerts);
	//Render Evil Bullet
	std::vector<Vertex_PCU> evilBulletVerts;
	Texture* evilBulletTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/EnemyShell.png");
	g_theRenderer->BindTexture(evilBulletTexture);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_EVIL_BULLET].size(); i++) {
		if (m_entityListsByType[ENTITYTYPE_EVIL_BULLET][i] != nullptr)
		    m_entityListsByType[ENTITYTYPE_EVIL_BULLET][i]->Render(evilBulletVerts);
	}
	g_theRenderer->DrawVertexArray(evilBulletVerts);
	//Render Good bullet
	std::vector<Vertex_PCU> goodBulletVerts;
	Texture* goodBulletTexture = g_theRenderer->CreateOrGetTextureFromFile("Images/FriendlyShell.png");
	g_theRenderer->BindTexture(goodBulletTexture);
	for (int i = 0; i < m_entityListsByType[ENTITYTYPE_GOOD_BULLET].size(); i++) {
		if (m_entityListsByType[ENTITYTYPE_GOOD_BULLET][i] != nullptr)
		    m_entityListsByType[ENTITYTYPE_GOOD_BULLET][i]->Render(goodBulletVerts);
	}
	g_theRenderer->DrawVertexArray(goodBulletVerts);
	//-----------------------------------------------------------
	
	std::vector<Vertex_PCU> playerUnused;
	m_player->Render(playerUnused);

	if (g_theGame->m_debugLine) {
		g_theRenderer->BindTexture(nullptr);
		for (int i = 0; i < m_allEntities.size(); i++) {
			if (m_allEntities[i]) {
				m_allEntities[i]->DebugRender();
			}
		}
	}



}

void Map::Update(float deltaSecond) {
	for (int i = 0; i < m_allEntities.size(); i++) {
		if (m_allEntities[i]) {
			m_allEntities[i]->Update(deltaSecond);
		}
	}
 	CheckAgentCollision();
	CheckTileCollision();
	CheckBulletCollision(deltaSecond);
	
	CreateDistanceField(*m_testHeatMap, EntityTilePosition(*m_player));
}

void Map::EndFrame() {
	RemoveGarbageEntity();
}
void Map::BeginFrame() {

}
void Map::GenerateTiles() {
	for (int row = 0; row < m_dimensions.y; row++) {
		for (int col = 0; col < m_dimensions.x; col++) {
			//walls at border
			if (col==0||row==0||col==m_dimensions.x-1||row==m_dimensions.y-1) {
				Tile newTile = Tile();
				newTile.m_tileCoords = IntVec2(col, row);
				newTile.m_tileType = TILE_TYPE_WALL;
				m_tiles.push_back(newTile);
			}
			//fixed bonkers
			else if (col<6&&row<6) {
				GenerateStartArea(col,row);
			}
			else if (col>m_dimensions.x-7&&row>m_dimensions.y-7) {
				GenerateEndArea(col, row);
			}
			else {
				Tile newTile = Tile();
				newTile.m_tileCoords = IntVec2(col, row);
				newTile.m_tileType = TILE_TYPE_GRASS;
				m_tiles.push_back(newTile);
			}
			
		}
	}
	//generate worms
	for (int i = 0; i < m_wormTile.size();i++) {
		for (int j = 0; j < m_numWorm[i]; j++) {
			GenerateWorm(m_wormTile[i], m_wormLength[i]);
		}
	}
}
void Map::StartUp() {
	TileDefinition::InitiateTileType();
	GenerateTiles();
	SpawnStartingEnemies();
	CreateHeatMap();
	CreateDistanceField(*m_testHeatMap, IntVec2(1, 1));
	while (m_testHeatMap->GetHeat(IntVec2(m_dimensions.x - 3,m_dimensions.y-3)) == -1) {
		GenerateTiles();
		CreateHeatMap();
		CreateDistanceField(*m_testHeatMap, IntVec2(1, 1));
	}
	FillHoles();
}
void Map::FillHoles() {
	for (Tile tile : m_tiles) {
		if (m_testHeatMap->GetHeat(tile.m_tileCoords) == -1) {
			tile.m_tileType = TILE_TYPE_WALL;
		}
	}
}
Map::Map(int x, int y) {
	m_dimensions = IntVec2(x,y);
}

std::vector<AABB2*> Map:: GetAdjacentWall(Entity entity) {
	int xCoord = RoundDownToInt(entity.m_position.x / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	int yCoord = RoundDownToInt(entity.m_position.y / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	
	std::vector<AABB2*> adjacentWall;
	for (int y = yCoord - 1; y <= yCoord + 1; y++) {
		for (int x = xCoord - 1; x <= xCoord + 1; x++) {
			if (x>=0&&y>=0&&x<m_dimensions.x&&y<m_dimensions.y) {
				if (true) {
					TileType type = m_tiles[x + y * m_dimensions.x].m_tileType;
					if (TileDefinition::s_definitions[type].m_IsSolid) {

						IntVec2 coord = m_tiles[x + y * m_dimensions.x].m_tileCoords;
						Vec2 mins(coord.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), coord.y * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
						Vec2 maxs((coord.x + 1) * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), (coord.y + 1) * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
						adjacentWall.push_back(new AABB2(mins, maxs));
					}
				}
			}
			
			
		}
	}
	return adjacentWall;
}
Vec2 Map::CameraPosition() {
	Vec2 position = m_player->m_position;
	//set position to bottom left corner
	position.x -= g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f) / 2;
	position.y -= g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f) / 2;
	if (position.x < 0) {
		position.x = 0;
	}
	if (position.y < 0) {
		position.y = 0;
	}
	if (position.x > WORLD_SIZE_X- g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f)) {
		position.x = WORLD_SIZE_X - g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f);
	}
	if (position.y > WORLD_SIZE_Y - g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f)) {
		position.y = WORLD_SIZE_Y - g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f);
	}
	return position;
}

Entity* Map::SpawnNewEntity(EntityType type, Faction faction, Vec2 const& position, float orientationDegrees) {
	Entity* e=CreateEntityOfType(type, faction, position, orientationDegrees);
	if (type == ENTITYTYPE_EVIL_BULLET || type == ENTITYTYPE_GOOD_BULLET) {
		g_theAudio->StartSound(g_theGame->m_Shoot);
	}
	AddEntityToMap(e);
	return e;
}
void Map::AddEntityToMap(Entity* e) {
	if (e->m_type == ENTITYTYPE_EVIL_BULLET || e->m_type == ENTITYTYPE_GOOD_BULLET) {
		AddEntityToList(e, m_bulletListsByFaction[e->m_faction]);
	}
	else {
		if (e->m_type == ENTITYTYPE_GOOD_PLAYER) {
			m_player = e;
			m_player->m_owner = this;
			m_player->m_position = Vec2(1.5f * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), 1.5f * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
		}
		AddEntityToList(e, m_agentListsByFaction[e->m_faction]);
	}
	//---------------
	AddEntityToList(e, m_entityListsByType[e->m_type]);
	AddEntityToList(e, m_allEntities);
}
void Map::AddEntityToList(Entity* e, EntityList& entityList) {
    entityList.push_back(e);
}

Entity* Map::CreateEntityOfType(EntityType type, Faction faction, Vec2 const& position, float orientationDegrees) {
	Entity* e;
	switch (type) {
	case ENTITYTYPE_EVIL_ARIES:
		 e=new Aries(this,position,orientationDegrees);
		 e->m_faction = faction;
		 e->m_type = type;
		 return e;
		break;

	case ENTITYTYPE_EVIL_BULLET:
		e = new Bullet(this, position, orientationDegrees);
		e->m_faction = faction;
		e->m_type = type;
		return e;
		break;
	case ENTITYTYPE_EVIL_LEO:
		e = new Leo(this, position, orientationDegrees);
		e->m_faction = faction;
		e->m_type = type;
		return e;
		break;
	case ENTITYTYPE_EVIL_SCORPIO:
		e = new Scorpio(this, position, orientationDegrees);
		e->m_faction = faction;
		e->m_type = type;
		return e;
		break;
	case ENTITYTYPE_GOOD_BULLET:
		e = new Bullet(this, position, orientationDegrees);
		e->m_faction = faction;
		e->m_type = type;
		e->m_health = 3;
		return e;

		break;
	case ENTITYTYPE_GOOD_PLAYER:
		m_player = new Player(this, Vec2(1.5f* g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), 1.5f* g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f)), .0f);
		m_player->m_faction = FACTION_GOOD;
		return m_player;
		break;
	}
	return nullptr;
}

void Map::RemoveEntityFromMap(Entity* e) {
	if (e->m_type == ENTITYTYPE_EVIL_BULLET || e->m_type == ENTITYTYPE_GOOD_BULLET) {
		RemoveEntityFromList(e, m_bulletListsByFaction[e->m_faction]);
	}
	else {
		g_theAudio->StartSound(g_theGame->m_Explosion);
		RemoveEntityFromList(e, m_agentListsByFaction[e->m_faction]);
	}
	//---------------
	RemoveEntityFromList(e, m_entityListsByType[e->m_type]);
	RemoveEntityFromList(e, m_allEntities);
	delete e;
}
void Map::RemoveEntityFromList(Entity* e, EntityList& entityList) {
	for (int i = 0; i < entityList.size(); i++) {
		if (entityList[i] == e) {
			entityList.erase(entityList.begin() + i);
			return;
		}
	}
}
//-------------------------------------

bool Map::HasLineOfSight(Vec2 const& PosA, Vec2 const& PosB, float range) {
	float distSq = GetDistanceSquared2D(PosA, PosB);
	if (distSq > range * range) {
		return false;
	}
	if (RaycastVsTiles(PosA, PosB).m_didImpact) {
		return false;

	}
	return true;

}

RaycastResult2D Map::RaycastVsTiles(Vec2 start, Vec2 end){
	constexpr int unitPerStep = 100;
	constexpr float distPerStep = 1.f / static_cast<float>(unitPerStep);
	Vec2 stepForward = (end - start).GetNormalized() * distPerStep;
	int numSteps=static_cast<int>((end-start).GetLength()*static_cast<float>(unitPerStep));
	
	RaycastResult2D result;
	for (int i = 0; i < numSteps; i++) {
		Vec2 position = start + stepForward * static_cast<float>(i);
		if (IsPointInSolid(position)) {
			result.m_didImpact = true;
			result.m_impactPos = position;
			result.m_impactDist = distPerStep * i;
			result.m_impactNormal = FindImpactNormal(position, stepForward);
			return result;
		}
		

			

	}
	result.m_impactDist = (end - start).GetLength();
	result.m_impactPos = end;
	return result;
}

bool Map::IsPointInSolid(Vec2 point) {
	int xCoord = RoundDownToInt(point.x / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	int yCoord = RoundDownToInt(point.y / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	if (xCoord < 0 || yCoord < 0 || xCoord >= m_dimensions.x || yCoord >= m_dimensions.y) {
		return true;
	}
	Tile tile = m_tiles[xCoord+yCoord * m_dimensions.x];
	TileType type = tile.m_tileType;
	TileDefinition def = TileDefinition::s_definitions[type];
	return TileDefinition::s_definitions[type].m_IsSolid;
}
IntVec2 Map::EntityTilePosition(Entity e)const {
	int xCoord = RoundDownToInt(e.m_position.x / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	int yCoord = RoundDownToInt(e.m_position.y / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	return IntVec2(xCoord, yCoord);
}
bool Map:: IsTileSolid(Tile tile) {
	TileDefinition t = TileDefinition::s_definitions[tile.m_tileType];
	return TileDefinition::s_definitions[tile.m_tileType].m_IsSolid;
}

Vec2 Map:: FindImpactNormal(Vec2 impactPos, Vec2 stepForward) const {
	Vec2 result = stepForward.GetNormalized();
	int impactX = RoundDownToInt(impactPos.x / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	int impactY = RoundDownToInt(impactPos.y / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	Vec2 beforeImpact = impactPos - stepForward;
	int beforeX = RoundDownToInt(beforeImpact.x / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	int beforeY = RoundDownToInt(beforeImpact.y / g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
	if (beforeX != impactX) {
		result.x *= -1;
	}
	if (beforeY != impactY) {
		result.y *= -1;
	}
	return result;
}

bool Map::IsOutOfBound(Entity* e) {
	float x = e->m_position.x;
	float y = e->m_position.y;
	return x <= 0 || y <= 0 || x >= m_dimensions.x * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f) ||y>m_dimensions.y* g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
}
void Map::RemoveGarbageEntity() {
	for (int i = 0; i < m_allEntities.size(); i++) {
		if (m_allEntities[i]!=nullptr) {
			if (m_allEntities[i]->m_isGarbage || IsOutOfBound(m_allEntities[i])) {
				RemoveEntityFromMap(m_allEntities[i]);
			}
		}
	}
}

void Map::SpawnStartingEnemies() {
	//SpawnNewEntity(ENTITYTYPE_EVIL_LEO, FACTION_EVIL, Vec2(6.5 * GRID_SIZE, 6.5 * GRID_SIZE), 0);
	for (int i = 0; i < m_tiles.size(); i++) {
	
			int spawnRate = g_RNG->RollRandomIntLessThan(100);
			if (spawnRate > 90) {
				//create random enemy
				Vec2 position((m_tiles[i].m_tileCoords.x + .5f) * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f), (m_tiles[i].m_tileCoords.y + .5f) * g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f));
				if (!TileDefinition::s_definitions[m_tiles[i].m_tileType].m_IsSolid) {
					int spawnType = g_RNG->RollRandomIntLessThan(3);
					if (spawnType == 0) {
						SpawnNewEntity(ENTITYTYPE_EVIL_ARIES, FACTION_EVIL, position, 0);
					}
					if (spawnType == 1) {
						SpawnNewEntity(ENTITYTYPE_EVIL_LEO, FACTION_EVIL, position, 0);
					}
					if (spawnType == 2) {
						SpawnNewEntity(ENTITYTYPE_EVIL_SCORPIO, FACTION_EVIL, position, 0);
					}
				}
				
			}
		
	}
}

void Map::CheckAgentCollision() {
	for (int i = 0; i < m_agentListsByFaction[FACTION_EVIL].size(); i++) {
		if(m_agentListsByFaction[FACTION_EVIL][i]){
			//player collision
			if (!m_agentListsByFaction[FACTION_EVIL][i]->m_pushable) {
				PushDiscOutOfDisc2D(m_player->m_position, m_player->m_physicsRadius, m_agentListsByFaction[FACTION_EVIL][i]->m_position, m_agentListsByFaction[FACTION_EVIL][i]->m_physicsRadius);
			}
			else {
				PushDiscsOutOfEachOther2D(m_agentListsByFaction[FACTION_EVIL][i]->m_position, m_agentListsByFaction[FACTION_EVIL][i]->m_physicsRadius, m_player->m_position, m_player->m_physicsRadius);
			}
			//other enemy collision
			for (int j = 0; j < m_agentListsByFaction[FACTION_EVIL].size(); j++) {
				if (m_agentListsByFaction[FACTION_EVIL][j]&&i!=j) {
					bool IPushJ = false;
					bool JpushI=false;
					if (m_agentListsByFaction[FACTION_EVIL][i]->m_pushes && m_agentListsByFaction[FACTION_EVIL][j]->m_pushable) {
						IPushJ = true;
					}
					if (m_agentListsByFaction[FACTION_EVIL][j]->m_pushes && m_agentListsByFaction[FACTION_EVIL][i]->m_pushable) {
						JpushI = true;
					}
					if (JpushI&&!IPushJ) {
						PushDiscOutOfDisc2D(m_agentListsByFaction[FACTION_EVIL][i]->m_position, m_agentListsByFaction[FACTION_EVIL][i]->m_physicsRadius, m_agentListsByFaction[FACTION_EVIL][j]->m_position, m_agentListsByFaction[FACTION_EVIL][j]->m_physicsRadius);
					}
					else if (IPushJ&&!JpushI) {
						PushDiscOutOfDisc2D(m_agentListsByFaction[FACTION_EVIL][j]->m_position, m_agentListsByFaction[FACTION_EVIL][j]->m_physicsRadius, m_agentListsByFaction[FACTION_EVIL][i]->m_position, m_agentListsByFaction[FACTION_EVIL][i]->m_physicsRadius);
					}
					else if(IPushJ&&JpushI){
						PushDiscsOutOfEachOther2D(m_agentListsByFaction[FACTION_EVIL][i]->m_position, m_agentListsByFaction[FACTION_EVIL][i]->m_physicsRadius, m_agentListsByFaction[FACTION_EVIL][j]->m_position, m_agentListsByFaction[FACTION_EVIL][j]->m_physicsRadius);
					}
					
				}
			}
		}
	}
}
void Map::CheckBulletCollision(float deltaSecond) {
	for (int i = 0; i < m_bulletListsByFaction[FACTION_EVIL].size(); i++) {
		if (m_bulletListsByFaction[FACTION_EVIL][i] != nullptr) {
			Vec2 forward = Vec2(g_gameConfigBlackboard.GetValue("BULLET_SPEED", 1.2f) * deltaSecond, 0);
			forward = forward.GetRotatedDegrees(m_bulletListsByFaction[FACTION_EVIL][i]->m_orientationDegrees);
			HandleEvilBullet(*m_bulletListsByFaction[FACTION_EVIL][i], deltaSecond);
			PhysicsAgainstTile(*m_bulletListsByFaction[FACTION_EVIL][i], forward);
		}
	}
	for (int i = 0; i < m_bulletListsByFaction[FACTION_GOOD].size(); i++) {
		if (m_bulletListsByFaction[FACTION_GOOD][i] != nullptr) {
			Vec2 forward = Vec2(g_gameConfigBlackboard.GetValue("BULLET_SPEED", 1.2f) * deltaSecond, 0);
			forward = forward.GetRotatedDegrees(m_bulletListsByFaction[FACTION_GOOD][i]->m_orientationDegrees);
			HandleGoodBullet(*m_bulletListsByFaction[FACTION_GOOD][i], deltaSecond);
			PhysicsAgainstTile(*m_bulletListsByFaction[FACTION_GOOD][i], forward);
		}
	}
}
void Map::CheckTileCollision() {
	for (int i = 0; i < m_allEntities.size(); i++) {
		if (m_allEntities[i]) {
			//no clip only work on player
			if (m_allEntities[i] != m_player || !g_theGame->m_noclip) {
				if (m_allEntities[i]->m_pushedByWall) {
					std::vector<AABB2*> walls = GetAdjacentWall(*m_allEntities[i]);
					m_allEntities[i]->PushOutOfWalls(walls);
				}
				
			}
		}
	}
}

Map::~Map() {
	for (EntityList e : m_entityListsByType) {
		e.clear();
	}
	for (EntityList e : m_agentListsByFaction) {
		e.clear();
	}
	for (EntityList e : m_bulletListsByFaction) {
		e.clear();
	}
	for (int i = 0; i < m_allEntities.size();i++) {
		if (m_allEntities[i] == m_player) {
			m_allEntities.erase(m_allEntities.begin() + i);
		}
		else {
			delete m_allEntities[i];
			m_allEntities.erase(m_allEntities.begin() + i);
		}
	}
}

void Map::HandleGoodBullet(Entity& bullet, float deltaSecond){
	//handle good bullet
	for (int enemy = 0; enemy < m_agentListsByFaction[FACTION_EVIL].size(); enemy++) {
		if (m_agentListsByFaction[FACTION_EVIL][enemy] != nullptr) {
			if (DoDiscsOverlap(m_agentListsByFaction[FACTION_EVIL][enemy]->m_position, m_agentListsByFaction[FACTION_EVIL][enemy]->m_physicsRadius, bullet.m_position, bullet.m_physicsRadius)) {
				
				if (m_agentListsByFaction[FACTION_EVIL][enemy]->m_type == ENTITYTYPE_EVIL_ARIES) {
					Vec2 forward = Vec2(g_gameConfigBlackboard.GetValue("BULLET_SPEED", 1.2f) *deltaSecond, 0);
					forward = forward.GetRotatedDegrees(bullet.m_orientationDegrees);
					PhysicsAgainstAries(bullet, *m_agentListsByFaction[FACTION_EVIL][enemy], forward);
				}
				else {
					bullet.m_isGarbage = true;
					m_agentListsByFaction[FACTION_EVIL][enemy]->m_health -= 1;
				}
				
			}
		}
	}
}
void Map::HandleEvilBullet(Entity& bullet, float deltaSecond) {
	UNUSED(deltaSecond);
	if (DoDiscsOverlap(bullet.m_position, bullet.m_physicsRadius, m_player->m_position, m_player->m_physicsRadius)) {
		m_player->m_health--;
		bullet.m_isGarbage = true;
	}
}
void Map::PhysicsAgainstAries(Entity& bullet, Entity& aries, Vec2 forward) {
	Vec2 ariesForward = Vec2::MakeFromPolarDegrees(aries.m_orientationDegrees);
	if (DotProduct2D(forward, ariesForward) > 0) {
		aries.m_health--;
		bullet.m_isGarbage = true;
	}
	else {
		bullet.m_health--;
		if (bullet.m_health > 0) {
			RaycastResult2D result = RaycastVsDisc2D(bullet.m_position, forward.GetNormalized(), forward.GetLength(), aries.m_position, aries.m_physicsRadius);
			bullet.m_orientationDegrees = result.m_impactNormal.GetOrientationDegrees();
			bullet.m_position = result.m_impactPos;
		}
		else {
			bullet.m_isGarbage = true;
		}
		
	}
}
void Map::PhysicsAgainstTile(Entity& Bullet, Vec2 forward) {
	RaycastResult2D result = RaycastVsTiles(Bullet.m_position, Bullet.m_position + forward);
	if (result.m_didImpact) {
		Bullet.m_health--;
		if (Bullet.m_health > 0) {
			Bullet.m_orientationDegrees = result.m_impactNormal.GetOrientationDegrees();
			Bullet.m_position = result.m_impactPos;
		}
		else {
			Bullet.m_isGarbage = true;
		}
	}
}

void Map::GenerateStartArea(int col, int row) {
	if ((col == 2 && row == 4) || (col == 3 && row == 4) || (col == 4 && row == 4) || (col == 4 && row == 3) || (col == 4 && row == 2)) {
		Tile newTile = Tile();
		newTile.m_tileCoords = IntVec2(col, row);
		newTile.m_tileType = TILE_TYPE_STONE_WALL;
		m_tiles.push_back(newTile);
	}
	else {
		Tile newTile = Tile();
		newTile.m_tileCoords = IntVec2(col, row);
		newTile.m_tileType = TILE_TYPE_STONE_FLOOR;
		m_tiles.push_back(newTile);
	}
}
void Map::GenerateEndArea(int col, int row) {
	int x = m_dimensions.x;
	int y = m_dimensions.y;
	if ((col == x - 3 && row == y - 5) || (col == x - 4 && row == y - 5) || (col == x - 5 && row == y - 5) || (col == x - 5 && row == y - 4) || (col == x - 5 && row == y - 3)) {
		Tile newTile = Tile();
		newTile.m_tileCoords = IntVec2(col, row);
		newTile.m_tileType = TILE_TYPE_STONE_WALL;
		m_tiles.push_back(newTile);
	}
	else if (col==x-3&&row==y-3) {
		Tile newTile = Tile();
		newTile.m_tileCoords = IntVec2(col, row);
		newTile.m_tileType = TILE_TYPE_END;
		m_tiles.push_back(newTile);
	}
	else {
		Tile newTile = Tile();
		newTile.m_tileCoords = IntVec2(col, row);
		newTile.m_tileType = TILE_TYPE_STONE_FLOOR;
		m_tiles.push_back(newTile);
	}
}
void Map::CreateHeatMap() {
	m_testHeatMap=new TileHeatMap(m_dimensions);
	
	
	
}
void Map::CreateDistanceField(TileHeatMap& heatMap, IntVec2 startCoords) {
	//don't change solid
	heatMap.SetAll(-1);
	heatMap.SetHeat(startCoords, 0);
	std::vector<IntVec2> tileToSpread;
	tileToSpread.push_back(startCoords);
	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(heatMap, tileToSpread, currentHeat);
		currentHeat++;
	}
	m_maxHeat = currentHeat;
	
}
std::vector<IntVec2> Map::SpreadHeat(TileHeatMap& heatMap, std::vector<IntVec2> previousTiles, int currentHeat) {
	std::vector<IntVec2> result;
	for (IntVec2 tile : previousTiles) {
		int x = tile.x;
		int y = tile.y;
		//check all adjacent tile, spread heat if possible, then store all tile heated
		if (IsSpreadable(heatMap, IntVec2(x-1, y))) {
			result.push_back(IntVec2(x-1,y));
			heatMap.SetHeat(IntVec2(x-1,y),currentHeat+1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x+1, y))) {
			result.push_back(IntVec2(x+1, y));
			heatMap.SetHeat(IntVec2(x+1, y), currentHeat+ 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x, y-1))) {
			result.push_back(IntVec2(x, y-1));
			heatMap.SetHeat(IntVec2(x, y-1), currentHeat+ 1.f);
		}
		if (IsSpreadable(heatMap, IntVec2(x, y+1))){
			result.push_back(IntVec2(x, y+1));
			heatMap.SetHeat(IntVec2(x, y+1), currentHeat+ 1.f);
		}
		
	}
	return result;
}
bool Map::IsSpreadable(TileHeatMap& heatMap, IntVec2 tile) {
	//check in bound
	if (tile.x < 0 || tile.x >= m_dimensions.x) {
		return false;
	}
	if (tile.y < 0 || tile.y >= m_dimensions.y) {
		return false;
	}
	//check is solid
	
	if (IsTileSolid(m_tiles[tile.x + tile.y * m_dimensions.x])) {
		return false;
	}
	//if heat not yet set, return true
	if (heatMap.GetHeat(tile) == -1.f) {
		//heatMap.SetHeat(tile, currentHeat + 1);
		return true;
	}
	else {
		return false;
	}
	
}
void Map::GenerateWorm(TileType type, int length) {
	IntVec2 startPos(g_RNG->RollRandomIntInRange(1,m_dimensions.x-2),g_RNG->RollRandomIntInRange(1,m_dimensions.y-2));
	IntVec2 currentPos = startPos;
	for (int i = 0; i < length; i++) {
		if (currentPos.x > 5 || currentPos.y > 5) {
			if (currentPos.x < m_dimensions.x - 6 || currentPos.y < m_dimensions.y - 6) {
				if (currentPos.x > 0 && currentPos.x < m_dimensions.x - 1) {
					if (currentPos.y > 0 && currentPos.y < m_dimensions.y - 1) {
						m_tiles[currentPos.x + currentPos.y * m_dimensions.x].m_tileType = type;
					}
				}
				
			}
		}
		int direction = g_RNG->RollRandomIntInRange(1, 4);
		if (direction == 1) {
			currentPos.x += 1;
		}
		if (direction == 2) {
			currentPos.y += 1;
		}
		if (direction == 3) {
			currentPos.x -= 1;
		}
		if (direction == 4) {
			currentPos.y -= 1;
		}
	}
	
}
