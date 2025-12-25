#include "Map.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Object.hpp"
#include "Game.hpp"
#include "MonsterInteraction.hpp"
Map::Map(MapDefinition* def, Game* owner)
{
	m_owner = owner;
	m_definition = def;
	m_dimensions = def->m_size;


	m_bgObjects.resize((int)m_dimensions.x * (int)m_dimensions.y);
	m_chObjects.resize((int)m_dimensions.x * (int)m_dimensions.y);
	m_fgObjects.resize((int)m_dimensions.x * (int)m_dimensions.y);

	CreateTiles();
	


	m_movementTimer = new Timer(MOVEMENT_TIME, g_gameClock);
}

Map::Map(IntVec2 bounds, Game* owner)
{
	m_owner = owner;
	m_dimensions = bounds;


	m_bgObjects.resize((int)m_dimensions.x * (int)m_dimensions.y);
	m_chObjects.resize((int)m_dimensions.x * (int)m_dimensions.y);
	m_fgObjects.resize((int)m_dimensions.x * (int)m_dimensions.y);

	m_movementTimer = new Timer(MOVEMENT_TIME, g_gameClock);
}

bool Map::IsPositionInBound(IntVec2 pos)
{
    return pos.x >= 0.f && pos.x < m_dimensions.x && pos.y >= 0.f && pos.y < m_dimensions.y;
}


IntVec2 Map::IndexToPos(int index)
{
    IntVec2 result;
	result.x = index%(int)m_dimensions.x;
	result.y =(int) ((index-result.x)/m_dimensions.x);
	return result;
}

Vec2 Map::GridToScreen(IntVec2 gridPos)
{
    return Vec2(gridPos.x*50.f/m_dimensions.x+25.f, gridPos.y*SCREEN_SIZE_Y/m_dimensions.y);
}

void Map::Update(float deltaSecond)
{
     UNUSED(deltaSecond);
	 TileHeatMap map = CreateHeatMap(m_player->m_position);
	 m_heatMap = &map;
     HandleKeyPressed();
	 for (int i = 0; i < m_chObjects.size(); i++) {
	     if(m_chObjects[i])
		     m_chObjects[i]->Update(deltaSecond);
	 }

	 RemoveObjects();
}

void Map::RemoveObjects()
{
	for (int i = 0; i < m_dimensions.x * m_dimensions.y; i++) {
		if (m_fgObjects[i] && m_fgObjects[i]->m_isDestroyed) {
			delete m_fgObjects[i];
			m_fgObjects[i] = nullptr;
		}

		if (m_chObjects[i] && m_chObjects[i]->m_isDestroyed) {
			delete m_chObjects[i];
			m_chObjects[i] = nullptr;
		}

		if (m_bgObjects[i] && m_bgObjects[i]->m_isDestroyed) {
			delete m_bgObjects[i];
			m_bgObjects[i] = nullptr;
		}
	}
}

void Map::Render()
{
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->SetSampleMode(SamplerMode::POINT_CLAMP);
	for (Tile tile : m_tiles) {
		TileDefinition* def = TileDefinition::g_tileDefinitions[(int)tile.m_type];
		g_theRenderer->BindTexture(&def->m_spriteSheet->GetTexture());

		std::vector<Vertex_PCU> verts;
		Vec2 mins = GridToScreen(tile.m_pos);
		Vec2 maxs = GridToScreen(tile.m_pos + IntVec2(1, 1));
		AddVertsForRect(verts, AABB2(mins,maxs), Rgba8::WHITE, def->m_spriteSheet->GetSpriteUVs(def->m_spriteIndex));
		g_theRenderer->DrawVertexArray(verts);
	}

	for (Object* obj : m_bgObjects) {
		if (obj) {
			obj->Render();
		}

	}
	for (Object* obj : m_chObjects) {
		if (obj) {
			obj->Render();
		}
	}
	for (Object* obj : m_fgObjects) {
		if (obj) {
			obj->Render();
		}
	}
}

void Map::OnEnter(Object* player, IntVec2 pos, SpriteDirection dir)
{
	m_movementTimer->Start();
	m_player = player;
	player->m_position = pos;
	player->m_map = this;
	m_chObjects[(int)(pos.x+pos.y*m_dimensions.x)] = player;
	player->m_currentDirection = dir;
}

void Map::OnLeave()
{
    m_movementTimer->Stop();
	m_chObjects[(int)(m_player->m_position.x + m_player->m_position.y*m_dimensions.x)] = nullptr;
	m_player = nullptr;
}

void Map::HandleKeyPressed()
{

	if (g_theInput->WasKeyJustPressed('W')) {
		if (m_movementTimer->HasPeriodElapsed()) {
			m_player->Move(IntVec2(m_player->m_position.x,m_player->m_position.y+1));
			m_movementTimer->Start();
		}
	}
	if (g_theInput->WasKeyJustPressed('A')) {
		if (m_movementTimer->HasPeriodElapsed()) {
			m_player->Move(IntVec2(m_player->m_position.x-1, m_player->m_position.y));
			m_movementTimer->Start();
		}
	}
	if (g_theInput->WasKeyJustPressed('S')) {
		if (m_movementTimer->HasPeriodElapsed()) {
			m_player->Move(IntVec2(m_player->m_position.x, m_player->m_position.y-1));
			m_movementTimer->Start();
		}
	}
	if (g_theInput->WasKeyJustPressed('D')) {
		if (m_movementTimer->HasPeriodElapsed()) {
			m_player->Move(IntVec2(m_player->m_position.x+1, m_player->m_position.y));
			m_movementTimer->Start();
		}
	}

	if (g_theInput->WasKeyJustReleased('E')) {
		if (m_player->GetInteractingObject()) {
		    m_player->GetInteractingObject()->Interact(m_player);
	    }
	    
	}

}

void Map::CreateTiles() {
	std::vector<Rgba8> data = m_definition->m_image.GetPixelData();
	int x = 0;
	int y = 0;
	for (Rgba8 color : data) {
		Tile newTile;

		newTile.m_pos = IntVec2(x,y);
		for (TileDefinition* def : TileDefinition::g_tileDefinitions) {
			if (def->m_color == color) {
				newTile.m_type = def->m_type;
			}
		}
		m_tiles.push_back(newTile);
		x++;
		if (x >= m_dimensions.x) {
			x = 0;
			y++;
		}
	}
}

bool Map::IsBlocked(IntVec2 pos)
{
	if (!IsPositionInBound(pos)) {
		return true;
	}

	if (m_tiles[(int)pos.x + (int)pos.y * (int)m_dimensions.x].m_blockPath) {
		return true;
	}

	TileDefinition* def = TileDefinition::g_tileDefinitions[(int)m_tiles[(int)pos.x + (int)pos.y * (int)m_dimensions.x].m_type];
	if (def->m_solid) {
		return true;
	}

	/*Object* obj = m_chObjects[(int)(pos.x + pos.y * m_dimensions.x)];
	if (obj) {
		return true;
	}*/

	return false;
}

Object* Map::SpawnObject(ObjectType type, IntVec2 pos, Interaction* interaction, SpriteDirection dir)
{
    ObjectDefinition* def = ObjectDefinition::g_objDefinitions[(int)type];

	Object* result = new Object(def, this, pos, interaction);

	int posIndex =(int)(pos.x + pos.y * m_dimensions.x);

	if (def->m_layer == LAYER_BACKGROUND) {

		if (m_bgObjects[posIndex]) {
			delete m_bgObjects[posIndex];
	    }

	    m_bgObjects[posIndex] = result;
	}
	if (def->m_layer == LAYER_CHRACTER) {

		if (m_chObjects[posIndex]) {
			delete m_chObjects[posIndex];
		}

		m_chObjects[posIndex] = result;
	}
	if (def->m_layer == LAYER_FOREGROUND) {

		if (m_fgObjects[posIndex]) {
			delete m_fgObjects[posIndex];
		}

		m_fgObjects[posIndex] = result;
	}
	result->m_currentDirection = dir;
	return result;
}

Object* Map::FindObjectAtPosition(IntVec2 pos)
{
    int posIndex = (int)(pos.x + pos.y * m_dimensions.x);
	if (m_chObjects[posIndex]) {
		return m_chObjects[posIndex];
	}
	if (m_fgObjects[posIndex]) {
		return m_fgObjects[posIndex];
	}
	if (m_bgObjects[posIndex]) {
		return m_bgObjects[posIndex];
	}
	return nullptr;
}


IntVec2 Map::GetPathToPlayer(IntVec2 origin)
{

	IntVec2 targetTile = m_player->m_position;
	IntVec2 myTile = IntVec2((int)origin.x, (int)origin.y);
	
	if (targetTile == myTile) {
		return IntVec2();
	}

	int heat = (int)m_heatMap->GetHeat(myTile);

	if (m_heatMap->GetHeat(IntVec2(myTile.x - 1, myTile.y)) == heat - 1) {
		return IntVec2(-1, 0);
	}
	
	if (m_heatMap->GetHeat(IntVec2(myTile.x + 1, myTile.y)) == heat - 1) {
		return IntVec2(1, 0);
	}
	
	if (m_heatMap->GetHeat(IntVec2(myTile.x, myTile.y - 1)) == heat - 1) {
		return IntVec2(0, -1);
	}
	
	if (m_heatMap->GetHeat(IntVec2(myTile.x, myTile.y + 1)) == heat - 1) {
		return IntVec2(0, 1);
	}
	
	return IntVec2();
}

TileHeatMap Map::CreateHeatMap(IntVec2 targetCoords)
{
	TileHeatMap heatMap(IntVec2((int)m_dimensions.x, (int)m_dimensions.y), -1.f);
	heatMap.SetAll(-1);
	heatMap.SetHeat(targetCoords, 0);
	std::vector<IntVec2> tileToSpread;
	tileToSpread.push_back(targetCoords);

    std::vector<bool> spreadable;

	for (int y = 0; y < m_dimensions.y; y++) {
		for (int x = 0; x < m_dimensions.x; x++) {
		    spreadable.push_back(!IsBlocked(IntVec2(x,y)));
		}
	}

	int currentHeat = 0;
	while (!tileToSpread.empty()) {
		tileToSpread = SpreadHeat(heatMap, tileToSpread, currentHeat,spreadable);
		currentHeat++;
	}
	return heatMap;
}



void Map::FloorToGrassPerlinNoise(float threshold)
{
	for (int y = 0; y < m_dimensions.y; y++) {
		for (int x = 0; x < m_dimensions.x; x++) {
			int index = y * m_dimensions.x + x;
			if (m_tiles[index].m_type == TileType::FLOOR) {
			    float chance =  threshold;
				float roll = Compute2dPerlinNoise((float)x, (float)y, 1.f, 4u, 0.5f, 2.f, true, GAME_SEED);
				if (roll < chance) {
				    m_tiles[index].m_type = TileType::GRASS;
				}
			}
		}
	}
}

IntVec2 Map::GetRandomEmptyPos()
{
    int x = 0;
	int y = 0;
	bool found = false;
	int trial = 0;
	while (!found) {
	    trial++;
	    x = Noise2DGetRandomIntInRange(trial + x, m_dimensions.x, 0, m_dimensions.x);
		y = Noise2DGetRandomIntInRange(trial + y, m_dimensions.y, 0, m_dimensions.y);
	    found = IsBlocked(IntVec2(x,y));
	}

	return IntVec2(x,y);
}

IntVec2 Map::GetFurthestEmptyPos(IntVec2 begin, TileHeatMap& out_heatMap)
{
    out_heatMap = CreateHeatMap(begin);


	IntVec2 far = IntVec2();

	int dist = 0;

	for (int y = 0; y < m_dimensions.y; y++) {
		for (int x = 0; x < m_dimensions.x; x++) {
			int newHeat = (int)out_heatMap.GetHeat(IntVec2(x, y));
			if (newHeat > dist) {
				far = IntVec2(x,y);
				dist = newHeat;
			}
		}
	}
	return far;
}

IntVec2 Map::EnemyWander(IntVec2 pos, int index, int step)
{
    std::vector<IntVec2> possibilities;
	if (!IsBlocked(pos + IntVec2(0, 1))) {
	    possibilities.push_back(pos + IntVec2(0, 1));
	}

	if (!IsBlocked(pos + IntVec2(0, -1))) {
		possibilities.push_back(pos + IntVec2(0, -1));
	}

	if (!IsBlocked(pos + IntVec2(1, 0))) {
		possibilities.push_back(pos + IntVec2(1, 0));
	}

	if (!IsBlocked(pos + IntVec2(-1, 0))) {
		possibilities.push_back(pos + IntVec2(-1, 0));
	}

	int i = Noise4DGetRandomIntInRange(pos.x, pos.y, index, step, 0, possibilities.size()-1);

	return possibilities[i];
}
