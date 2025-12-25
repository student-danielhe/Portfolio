#include "FactionUI.hpp"
#include "Game.hpp"
#include "Engine/Window/Window.hpp"
void FactionUI::Render()
{
    float xMin = 0.f;
	float xMax = 0.f;
	if (m_factionID == FactionType::ALLY) {
		xMin = 0.f;
		xMax = (float)(WORLDSIZEX-COLNUM)/2.f;
	}
	if (m_factionID == FactionType::ENEMY) {
		xMin = (float)WORLDSIZEX - ((float)(WORLDSIZEX - COLNUM) / 2.f);
		xMax = WORLDSIZEX;
	}

	float yPos = 1.f;
	//UI box
	std::vector<Vertex_PCU> verts;
	g_theRenderer->BindTexture(nullptr);
	AddVertsForRect(verts, AABB2(Vec2(xMin, 0.f), Vec2(xMax, WORLDSIZEY)),Rgba8::BLACK);
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();

	//Cube list text
	g_theRenderer->BindTexture(&g_font->GetTexture());
	for (int i = 1; i <= 9; i++) {
	    Rgba8 color = Rgba8::WHITE;
		if (m_selectedCube+1 == i) {
			color=Rgba8::GREEN;
		}
		g_font->AddVertsForText2D(verts,Vec2(xMin+2.f,WORLDSIZEY-yPos-2.5f),2.f,std::to_string(i), color);
		yPos += 4.f;
	}
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();
	//Render image
	yPos=1.f;
	for (int i = 0; i <= 8; i++) {
		if ((int)m_cubeList.size()>i && m_cubeList[i]) {
			g_theRenderer->BindTexture(&m_cubeList[i]->m_spriteSheet->GetTexture());
			AABB2 box(Vec2(xMin + 5.f, WORLDSIZEY-yPos-UICUBESIZE), Vec2(xMin + 5.f + UICUBESIZE, WORLDSIZEY-yPos));
			AddVertsForRect(verts, box, Rgba8::WHITE, m_cubeList[i]->m_spriteSheet->GetSpriteUVsTrimmed(m_cubeList[i]->m_spriteIndex));
			g_theRenderer->DrawVertexArray(verts);
			verts.clear();
			yPos += 4.f;
	    }
	}

	//Render Mana
	g_theRenderer->BindTexture(nullptr);
	verts.clear();
	if (m_factionID == FactionType::ALLY) {
	    xMin+=0.f;
	}
	AABB2 box = AABB2(Vec2(xMin + 2.5f, 1.5f), Vec2(xMin + 13.5f, 4.5f));
	AddVertsForRect(verts, box, Rgba8::WHITE);
	box = AABB2(Vec2(xMin + 3.f, 2.f), Vec2(xMin + 13.f, 4.f));
	AddVertsForRect(verts, box, Rgba8::BLACK);
	if (m_mana < m_cubeList[m_selectedCube]->m_manaCost) {
		box = AABB2(Vec2(xMin + 3.f, 2.f), Vec2(xMin + 3.f + ((float)m_cubeList[m_selectedCube]->m_manaCost / (float)m_maxMana) * 10.f, 4.f));
		AddVertsForRect(verts, box, Rgba8::RED);
	}
	box = AABB2(Vec2(xMin+3.f, 2.f), Vec2(xMin+3.f+((float)m_mana/(float)m_maxMana)*10.f, 4.f));
	AddVertsForRect(verts, box, Rgba8::BLUE);
	if (m_mana >= m_cubeList[m_selectedCube]->m_manaCost) {
		box = AABB2(Vec2(xMin + 3.f, 2.f), Vec2(xMin + 3.f + ((float)m_cubeList[m_selectedCube]->m_manaCost / (float)m_maxMana) * 10.f, 4.f));
		AddVertsForRect(verts, box, Rgba8::GREEN);
	}
	g_theRenderer->DrawVertexArray(verts);

	//RenderHealth
	verts.clear();
	if (m_factionID == FactionType::ALLY) {
		xMin += 10.f;
	}
	if (m_factionID == FactionType::ENEMY) {
		xMin-=13.f;
	}
	box = AABB2(Vec2(xMin + 2.5f, WORLDSIZEY - 4.5f), Vec2(xMin + 13.5f, WORLDSIZEY - 1.5f));
	AddVertsForRect(verts, box, Rgba8::WHITE);
	box = AABB2(Vec2(xMin + 3.f, WORLDSIZEY - 4.f), Vec2(xMin + 13.f, WORLDSIZEY - 2.f));
	AddVertsForRect(verts, box, Rgba8::BLACK);

	box = AABB2(Vec2(xMin + 3.f, WORLDSIZEY-4.f), Vec2(xMin + 3.f + ((float)m_leader->m_currentHealth/ (float)m_leader->m_maxHealth) * 10.f, WORLDSIZEY-2.f));
	AddVertsForRect(verts, box, Rgba8::RED);
	
	g_theRenderer->DrawVertexArray(verts);
}

FactionUI::FactionUI(FactionType factionID): m_factionID(factionID)
{
    m_cubeList.resize(9);
	m_manaTimer = new Timer(1.f, Game::g_gameClock);
	m_manaTimer->Start();
}

void FactionUI::AddCube(CubeDefinition* def)
{
	for (int i = 0; i < m_cubeList.size(); i++) {
		if (!m_cubeList[i]) {
			m_cubeList[i]=def;
			return;
		}
	}
}

void FactionUI::Update()
{
    if(m_factionID==FactionType::ALLY)
        HandleKeyPressed();

	while (m_manaTimer->DecrementPeriodIfElapsed()) {
		if (m_mana < m_maxMana) {
			m_mana++;
		}
	}

	if (m_factionID == FactionType::ENEMY) {
		EnemyPlaceCube();
	}
}

void FactionUI::HandleKeyPressed()
{
    int oldSelected = m_selectedCube;
    if (g_theInput->WasKeyJustReleased('1')) {
        m_selectedCube = 0;
    }
	if (g_theInput->WasKeyJustReleased('2')) {
		m_selectedCube = 1;
	}
	if (g_theInput->WasKeyJustReleased('3')) {
		m_selectedCube = 2;
	}
	if (g_theInput->WasKeyJustReleased('4')) {
		m_selectedCube = 3;
	}
	if (g_theInput->WasKeyJustReleased('5')) {
		m_selectedCube = 4;
	}
	if (g_theInput->WasKeyJustReleased('6')) {
		m_selectedCube = 5;
	}
	if (g_theInput->WasKeyJustReleased('7')) {
		m_selectedCube = 6;
	}
	if (g_theInput->WasKeyJustReleased('8')) {
		m_selectedCube = 7;
	}
	if (g_theInput->WasKeyJustReleased('9')) {
		m_selectedCube = 8;
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_UP)) {
		m_selectedCube ++;
		m_selectedCube=m_selectedCube%9;
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_DOWN)) {
		m_selectedCube --;
		if (m_selectedCube < 0) {
			m_selectedCube=8;
		}
	}
	if (oldSelected > m_selectedCube) {
		g_theAudio->StartSound(Game::m_Tick);
	}
	if (oldSelected < m_selectedCube) {
		g_theAudio->StartSound(Game::m_Tock);
	}
}

void FactionUI::EnemyPlaceCube()
{
    CubeDefinition* cube = m_cubeList[m_selectedCube];
	if (cube&&m_mana >= cube->m_manaCost) {
		IntVec2 pos = GetValidAIPlacement();
		if (pos != IntVec2(-1, -1)) {
			Game::g_map->SpawnCube(*cube, pos, FactionType::ENEMY);
			m_mana -= cube->m_manaCost;
		}
		if (m_selectedCube<8&&m_cubeList[m_selectedCube + 1]) {
		    m_selectedCube++;
		}
		else {
			m_selectedCube=0;
		}
	}
}

IntVec2 FactionUI::GetValidAIPlacement()
{
    std::vector<IntVec2> possiblePlacements;
	for (int x = 26; x < 52; x++) {
		for( int y =0; y<ROWNUM;y++){
		    IntVec2 pos = IntVec2(x, y);
			if (AICheckThisPosition(pos)) {
				possiblePlacements.push_back(pos);
			}

		}
    }
	if (possiblePlacements.size() == 0) {
		return IntVec2(-1,-1);
	}
	int result = g_RNG->RollRandomIntLessThan((int)possiblePlacements.size());
	return possiblePlacements[result];
}

bool FactionUI::AICheckThisPosition(IntVec2 pos)
{
	if (!Game::g_map->IsPositionInBound(pos)||Game::g_map->m_cubesOnMap[Game::g_map->PosToIndex(pos)]) {
	return false;
	}
	std::vector<bool> checkList = m_cubeList[m_selectedCube]->m_AIPlacement;

	//prevent enemy block
	if (Game::g_map->IsPositionInBound(IntVec2(pos.x,pos.y-1))) {
		Cube* below =Game::g_map->m_cubesOnMap[Game::g_map->PosToIndex(IntVec2(pos.x, pos.y - 1))];
		if(below && below->m_faction==m_factionID && below->m_def.m_AIPlacement[(int)AIPlacement::AINORTHEMPTY])
	        return false;
		if (checkList[(int)AIPlacement::AIENEMYSOUTH]) {
			if(!(below && OppositeFaction(below))) {
			    return false;
			}
		}
	}

	if (!(checkList[(int)AIPlacement::AIFLYING] || checkList[(int)AIPlacement::AIPROJECTILE])) {
		if (Game::g_map->IsPositionInBound(IntVec2(pos.x, pos.y - 1)) && !Game::g_map->m_cubesOnMap[Game::g_map->PosToIndex(IntVec2(pos.x, pos.y - 1))]) {
			return false;
		}
	}


	if (checkList[(int)AIPlacement::AIWARRIOR]) {
		if (pos.x > 40||pos.x<30) {
			return false;
		}
	}

	if (checkList[(int)AIPlacement::AIDEFENSE]) {
		if (pos.x < 40||pos.x>50) {
			return false;
		}
	}

	if (checkList[(int)AIPlacement::AIFRONTLINE]) {
		if (pos.x > 30) {
			return false;
		}
	}

	if (checkList[(int)AIPlacement::AIBACKLINE]) {
		if (pos.x < 50) {
			return false;
		}
	}


	if (checkList[(int)AIPlacement::AISTACKING]) {
		std::vector<IntVec2> positions;
		positions.push_back(IntVec2(pos.x + 1, pos.y));
		positions.push_back(IntVec2(pos.x - 1, pos.y));
		positions.push_back(IntVec2(pos.x, pos.y + 1));
		positions.push_back(IntVec2(pos.x, pos.y - 1));
		bool hasNearbyAlly = false;
		for (IntVec2 position : positions) {
			if (Game::g_map->IsPositionInBound(position)) {
			    Cube* cube = Game::g_map->m_cubesOnMap[Game::g_map->PosToIndex(position)];
				if(cube&&cube->m_faction==m_factionID)
				hasNearbyAlly=true;
			}
		}
		if (!hasNearbyAlly) {
			return false;
		}
	}

	if (checkList[(int)AIPlacement::AIPROJECTILE]) {
		std::vector<IntVec2> positions;
		for (int x = 0; x < pos.x; x++) {
		    positions.push_back(IntVec2(x,pos.y));
		}
		bool alignedWithEnemy = false;
		for (IntVec2 position : positions) {
			if (Game::g_map->IsPositionInBound(position)) {
			    Cube* cube = Game::g_map->m_cubesOnMap[Game::g_map->PosToIndex(position)];
				if(cube&&OppositeFaction(cube))
				alignedWithEnemy = true;
			}
		}
		if (!alignedWithEnemy) {
			return false;
		}
	}
	return true;
}

bool FactionUI::OppositeFaction(Cube* cube)
{
	if (m_factionID == FactionType::ALLY) {
		return cube->m_faction==FactionType::ENEMY;
	}
	if (m_factionID == FactionType::ENEMY) {
		return cube->m_faction==FactionType::ALLY;
	}
	return false;
}

CubeDefinition* FactionUI::GetMouseOver()
{
	Vec2 mouseDelta = g_theWindow->GetNormalizedMouseUV();
	mouseDelta.x *= WORLDSIZEX;
	mouseDelta.y *= WORLDSIZEY;


	float xMin = 0.f;
	float xMax = 0.f;
	if (m_factionID == FactionType::ALLY) {
		xMin = 0.f;
		xMax = (float)(WORLDSIZEX - COLNUM) / 2.f;
	}
	if (m_factionID == FactionType::ENEMY) {
		xMin = (float)WORLDSIZEX - ((float)(WORLDSIZEX - COLNUM) / 2.f);
		xMax = WORLDSIZEX;
	}

	float yPos = 1.f;
	for (int i = 0; i <= 8; i++) {
		if ((int)m_cubeList.size() > i && m_cubeList[i]) {
			
			AABB2 box(Vec2(xMin + 5.f, WORLDSIZEY - yPos - UICUBESIZE), Vec2(xMin + 5.f + UICUBESIZE, WORLDSIZEY - yPos));
			if (box.IsPointInside(mouseDelta)) {
				return m_cubeList[i];
			}
			
			yPos += 4.f;

		}
	}
	return nullptr;
}

int FactionUI::GetMouseOverIndex()
{
	Vec2 mouseDelta = g_theWindow->GetNormalizedMouseUV();
	mouseDelta.x *= WORLDSIZEX;
	mouseDelta.y *= WORLDSIZEY;


	float xMin = 0.f;
	float xMax = 0.f;
	if (m_factionID == FactionType::ALLY) {
		xMin = 0.f;
		xMax = (float)(WORLDSIZEX - COLNUM) / 2.f;
	}
	if (m_factionID == FactionType::ENEMY) {
		xMin = (float)WORLDSIZEX - ((float)(WORLDSIZEX - COLNUM) / 2.f);
		xMax = WORLDSIZEX;
	}

	float yPos = 1.f;
	for (int i = 0; i <= 8; i++) {
		if ((int)m_cubeList.size() > i && m_cubeList[i]) {

			AABB2 box(Vec2(xMin + 5.f, WORLDSIZEY - yPos - UICUBESIZE), Vec2(xMin + 5.f + UICUBESIZE, WORLDSIZEY - yPos));
			if (box.IsPointInside(mouseDelta)) {
				return i;
			}

			yPos += 4.f;

		}
	}
	return -1;
}
