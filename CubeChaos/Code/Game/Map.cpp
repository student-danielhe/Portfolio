#include "Map.hpp"
#include "Game.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
Map::Map(Game* owner): m_owner(owner)
{
    
}

void Map::StartUp()
{
    m_descriptionUI = new DescriptionUI();
    CubeDefinition::InitializeCubeDefinition();
	m_cubesOnMap.resize(ROWNUM * COLNUM);
	for (int i = 0; i < m_cubesOnMap.size(); i++) {
		if (m_cubesOnMap[i]) {
			delete m_cubesOnMap[i];
			m_cubesOnMap[i] = nullptr;
		}
	}
	m_PhysicsTimer = new Timer(fixedPhysicsTime, m_owner->g_gameClock);
	m_PhysicsTimer->Start();
    
    
    m_ally = new Faction(FactionType::ALLY, this);
    m_enemy =  new Faction(FactionType::ENEMY, this);

    m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::DWARF_WARRIOR]);
    m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::MAGIC_MISSILE]);
    m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::MAGIC_MISSILE_MAGE]);
    m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::TINYCASTLE]);
    m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::KNIGHT]);
    m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::LARGE_WARRIOR_SLIME]);
    m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::EXCAVATE]);
	m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::BUNNY]);
	m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::RAIN_CLOUD]);
	m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::ROYAL_GUARD]);
	m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::SUNFLOWER]);
	m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::GIANTFLY]);
	m_ally->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::WATER]);
    
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    //m_enemy->m_UI->AddCube(CubeDefinition::g_cubeDefinitions[(int)CubeType::STONE]);
    InitializeCubesOnMap();
}

void Map::InitializeCubesOnMap()
{
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < COLNUM; x++) {
            SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::EARTH], IntVec2(x,y));
        }
    }
    int y=4;
    for (int x = 0; x < 12; x++) {
        SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::EARTH], IntVec2(x, y));
    }
    for (int x = 40; x < COLNUM; x++) {
        SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::EARTH], IntVec2(x, y));
    }
    y = 5;
    for (int x = 0; x < 7; x++) {
        SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::EARTH], IntVec2(x, y));
    }
    for (int x = 45; x < COLNUM; x++) {
        SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::EARTH], IntVec2(x, y));
    }

    SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::LEADER], IntVec2(1, 6), FactionType::ALLY);
    m_ally->m_UI->m_leader = m_cubesOnMap[PosToIndex(IntVec2(1,6))];
    SpawnCube(*CubeDefinition::g_cubeDefinitions[(int)CubeType::LEADER], IntVec2(50, 6), FactionType::ENEMY);
    m_enemy->m_UI->m_leader = m_cubesOnMap[PosToIndex(IntVec2(50, 6))];
}

bool Map::SpawnCube(CubeType type, IntVec2 pos, FactionType faction)
{
    CubeDefinition const& def = *CubeDefinition::g_cubeDefinitions[(int)type];
    return SpawnCube(def, pos, faction);
}

bool Map::SpawnCube(CubeDefinition const& def, IntVec2 pos, FactionType faction /*= FactionType::NEUTRAL*/)
{
	if (IsPositionInBound(pos)) {
		if (m_cubesOnMap[PosToIndex(pos)]) {
			return false;
		}
		m_cubesOnMap[PosToIndex(pos)] = new Cube(def, pos, faction, this);
        if (def.m_projectile > 0) {
            g_theAudio->StartSound(Game::m_Shoot);
        }
		return true;
	}
	return false;
}

void Map::HandleKeyPressed()
{
    if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
        IntVec2 cell = GetMouseCell();
        if (m_debugging||cell.x < 26) {
			CubeDefinition* selected = m_ally->m_UI->m_cubeList[m_ally->m_UI->m_selectedCube];
			if (selected) {
                if (IsPositionInBound(cell)&&!m_cubesOnMap[(int)PosToIndex(cell)]) {
					if (m_debugging||m_ally->m_UI->m_mana > selected->m_manaCost) {
						m_ally->m_UI->m_mana -= selected->m_manaCost;
						g_theAudio->StartSound(Game::m_CubePlacement);
						SpawnCube(*selected, cell, FactionType::ALLY);
					}
                }
			}
        }

        CubeDefinition* allyMouseOver = m_ally->m_UI->GetMouseOver();
        if (allyMouseOver) {
            m_ally->m_UI->m_selectedCube=m_ally->m_UI->GetMouseOverIndex();
        }
    }

    if (g_theInput->WasKeyJustReleased('G')) {
        m_debugging =!m_debugging;
    }
}

IntVec2 Map::GetMouseCell()
{
    Vec2 mouseDelta = g_theWindow->GetNormalizedMouseUV();
    mouseDelta.x*=WORLDSIZEX;
    mouseDelta.y*=WORLDSIZEY;

    AABB2 GameScreen(GridToScreen(IntVec2(0,0)), GridToScreen(IntVec2(COLNUM, ROWNUM)));
    if (GameScreen.IsPointInside(mouseDelta)) {
        mouseDelta.x-= ((WORLDSIZEX - COLNUM) / 2);
        mouseDelta.y-= (WORLDSIZEY - ROWNUM - 5);
        mouseDelta.x/=CUBESIZE;
        mouseDelta.y/=CUBESIZE;
        if (mouseDelta.y == 28)
            mouseDelta.y--;
        if(mouseDelta.x==52)
            mouseDelta.x--;
        return IntVec2(RoundDownToInt(mouseDelta.x), RoundDownToInt(mouseDelta.y));
    }
    return IntVec2(-1,-1);
}

void Map::RenderPreview()
{
    IntVec2 mouseCell = GetMouseCell();
    Rgba8 color = Rgba8::RED;
    if (mouseCell.x < 26) {
        color = Rgba8::GREEN;
    }
    CubeDefinition* selected = m_ally->m_UI->m_cubeList[m_ally->m_UI->m_selectedCube];
    if (mouseCell != IntVec2(-1, -1) && selected) {
        if (!m_cubesOnMap[PosToIndex(mouseCell)]) {
			g_theRenderer->BindTexture(&selected->m_spriteSheet->GetTexture());
			std::vector<Vertex_PCU> verts;
			Vec2 BL = GridToScreen(mouseCell);
			AddVertsForRect(verts, Vec2(BL.x + CUBESIZE, BL.y), Vec2(BL.x + CUBESIZE, BL.y + CUBESIZE), Vec2(BL.x, BL.y + CUBESIZE), BL, color, selected->m_spriteSheet->GetSpriteUVsTrimmed(selected->m_spriteIndex));
			g_theRenderer->DrawVertexArray(verts);
        }
        else {
            RenderDescription(m_cubesOnMap[PosToIndex(mouseCell)]);
        }
    }
    CubeDefinition* allyMouseOver = m_ally->m_UI->GetMouseOver();
    CubeDefinition* enemyMouseOver = m_enemy->m_UI->GetMouseOver();
    if (allyMouseOver) {
        m_reservedForUI = new Cube(*allyMouseOver, IntVec2(-1,-1),FactionType::ALLY, this);
    }
    else if (enemyMouseOver) {
        m_reservedForUI = new Cube(*enemyMouseOver, IntVec2(-1, -1), FactionType::ENEMY, this);
    }
    else {
        m_reservedForUI=nullptr;
    }

    if (m_reservedForUI) {
        RenderDescription(m_reservedForUI);
    }
}

void Map::RenderTime()
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << m_owner->g_gameClock->GetTimeScale();
    std::string time = ss.str();
    if (m_owner->g_gameClock->IsPaused()) {
        time = "0.00";
    }
    std::string timeScaleTxt ="TimeScale: "+ time;

    std::vector<Vertex_PCU> verts;
    g_font->AddVertsForText2D(verts, Vec2(WORLDSIZEX/2.f+5.f, 1.f),1.f, timeScaleTxt);
    g_theRenderer->BindTexture(&g_font->GetTexture());
    g_theRenderer->DrawVertexArray(verts);
}

void Map::RenderDescription(Cube* cube)
{
    g_theRenderer->BindTexture(nullptr);
    std::vector<Vertex_PCU> backGround;
    AddVertsForRect(backGround, m_descriptionUI->MainBody);
    AddVertsForRect(backGround, AABB2(Vec2(10.5f, 25.5f), Vec2(69.5f, 39.5f)), Rgba8::BLACK);
    
    AddVertsForRect(backGround, m_descriptionUI->HealthBox, Rgba8(100,100,100));
    AABB2 redHealth = m_descriptionUI->HealthBox;
    float length = (m_descriptionUI->HealthBox.m_maxs.x-m_descriptionUI->HealthBox.m_mins.x)*((float)cube->m_currentHealth/(float)cube->m_maxHealth);
    redHealth.m_maxs.x=m_descriptionUI->HealthBox.m_mins.x+length;
    AddVertsForRect(backGround, redHealth, Rgba8::RED);

    g_theRenderer->DrawVertexArray(backGround);


    std::vector<Vertex_PCU> texts;
    g_theRenderer->BindTexture(&g_font->GetTexture());
    g_font->AddVertsForText2D(texts, Vec2(11.f,37.5f), 1.5f,cube->m_def.m_name);
    g_font->AddVertsForText2D(texts, Vec2(11.f, 35.f), 1.f, std::to_string(cube->m_def.m_manaCost)+" Mana");
    g_font->AddVertsForText2D(texts, Vec2(13.5f, 32.5f), 1.f, std::to_string(cube->m_currentHealth) + "/" + std::to_string(cube->m_maxHealth));
    std::string factionstr = "Ally";
    if (cube->m_faction == FactionType::ENEMY) {
        factionstr ="Enemy";
    }
    if (cube->m_faction == FactionType::NEUTRAL) {
        factionstr = "Neutral";
    }
    g_font->AddVertsForText2D(texts, Vec2(12.f, 27.f), 1.5f, factionstr);

    float xPos = m_descriptionUI->DescriptionBox.m_mins.x;
    float yPos = m_descriptionUI->DescriptionBox.m_maxs.y-2.f;
    for (std::string line : cube->m_description) {
        yPos-=1.5f;
        g_font->AddVertsForText2D(texts,Vec2(xPos, yPos), .6f, line);
    }
    g_theRenderer->DrawVertexArray(texts);

    std::vector<Vertex_PCU> image;

    AddVertsForRect(image, m_descriptionUI->ImageBox, Rgba8::WHITE,cube->m_def.m_spriteSheet->GetSpriteUVsTrimmed(cube->m_def.m_spriteIndex));
    g_theRenderer->BindTexture(&cube->m_def.m_spriteSheet->GetTexture());
    g_theRenderer->DrawVertexArray(image);

}



void Map::MoveCube(Cube* cube, IntVec2 targetPosition)
{
    if (IsPositionInBound(targetPosition)) {
        Cube* targetCube = m_cubesOnMap[PosToIndex(targetPosition)];
        if (!targetCube) {
            m_cubesOnMap[PosToIndex(targetPosition)] = cube;
            m_cubesOnMap[PosToIndex(cube->m_Position)]=nullptr;
            cube->m_Position = targetPosition;
        }
        else{
            if (cube->m_projectile > 0&& cube->m_faction!=targetCube->m_faction) {
                cube->Damage(cube->m_projectile, targetCube);
                cube->m_dead=true;
                cube->OnDeath();
            }
			if (targetCube->m_liquid) {
				//swap position with liquid
				m_cubesOnMap[PosToIndex(targetPosition)] = cube;
				m_cubesOnMap[PosToIndex(cube->m_Position)] = targetCube;
				targetCube->m_Position = cube->m_Position;
				cube->m_Position = targetPosition;
			}
        }
    }
}


bool Map::IsPositionInBound(IntVec2 pos)
{
   return pos.x>=0&&pos.x<COLNUM&&pos.y>=0&&pos.y<ROWNUM;
}

int Map::PosToIndex(IntVec2 pos)
{
    return pos.x+pos.y*COLNUM;
}

IntVec2 Map::IndexToPos(int index)
{
    int x = index%COLNUM;
    int y = (index-x)/COLNUM;
    return IntVec2(x,y);
}

Vec2 Map::GridToScreen(IntVec2 gridPos)
{
    //need improve
    return Vec2(gridPos.x*CUBESIZE+((WORLDSIZEX-COLNUM)/2), gridPos.y * CUBESIZE+(WORLDSIZEY-ROWNUM-5));
}

void Map::Update(float deltaSecond)
{
    UNUSED(deltaSecond);
    HandleKeyPressed();
    m_ally->m_UI->Update();
    m_enemy->m_UI->Update();
    while (m_PhysicsTimer->DecrementPeriodIfElapsed())
    {
       PhysicsUpdate();
       CleanUpCubeAndVFX();
       for (VFX* vfx : m_VFXs) {
           vfx->Update();
       }
       if (m_owner->m_state != AttractState::ATTRACT) {
           break;
       }
    }
}

void Map::PhysicsUpdate()
{

	for (int i = 0; i < ROWNUM * COLNUM; i++) {
		if (m_cubesOnMap[i]&&!m_cubesOnMap[i]->m_dead) {
            m_cubesOnMap[i]->UpdateGravity();
		}
	}
    for (int i = 0; i < ROWNUM * COLNUM; i++) {
        if (m_cubesOnMap[i]&&!m_cubesOnMap[i]->m_dead) {
            m_cubesOnMap[i]->PhyscisUpdate();
        }
    }
}

void Map::CleanUpCubeAndVFX()
{

    for (int i = 0; i < ROWNUM * COLNUM;i++) {
        if (m_cubesOnMap[i] && m_cubesOnMap[i]->m_dead) {
			if (m_cubesOnMap[i]->m_leader) {

				if (m_cubesOnMap[i]->m_faction == FactionType::ALLY) {
					g_theGame->m_state = AttractState::LOOSE;
				}
				else if (m_cubesOnMap[i]->m_faction == FactionType::ENEMY) {
					g_theGame->m_state = AttractState::WIN;
				}
                return;
			}
            delete m_cubesOnMap[i];
            m_cubesOnMap[i]=nullptr;
        }
    }

    for (int i = 0; i < m_VFXs.size(); i++) {
        if (m_VFXs[i]->m_destroyed) {
            m_VFXs.erase(m_VFXs.begin()+i);
        }
    }

}

void Map::Render()
{
    g_theRenderer->SetDepthMode(DepthMode::DISABLED);
    g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
    g_theRenderer->SetSampleMode(SamplerMode::POINT_CLAMP);
    for (int i = 0; i < ROWNUM * COLNUM; i++) {
        if (m_cubesOnMap[i]) {
            m_cubesOnMap[i]->Render();
        }
        /*else {
            IntVec2 pos = IndexToPos(i);
            RenderEmpty(pos);
        }*/

    }

	for (int i = 0; i < (int)m_VFXs.size(); i++) {
		m_VFXs[i]->Render();
	}

    
    g_theRenderer->BindTexture(nullptr);
    std::vector<Vertex_PCU> verts;
    AddVertsForRect(verts,AABB2(Vec2(0,0), Vec2(WORLDSIZEX, (WORLDSIZEY - ROWNUM - 5))),Rgba8::BLACK);
    AddVertsForRect(verts, AABB2(Vec2(0, (WORLDSIZEY - 5)), Vec2(WORLDSIZEX,WORLDSIZEY)), Rgba8::BLACK);
    g_theRenderer->DrawVertexArray(verts);
    m_ally->m_UI->Render();
    m_enemy->m_UI->Render();
    RenderTime();
    RenderPreview();
}




