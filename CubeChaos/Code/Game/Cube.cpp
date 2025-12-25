#include "Cube.hpp"
#include "Map.hpp"
#include "Game.hpp"
#include "VFX.hpp"
#include "Engine/Core/StringUtils.hpp"
Cube::Cube(CubeDefinition const& def, IntVec2 pos, FactionType faction, Map* owner): m_def(def), m_Position(pos), m_faction(faction), m_owner(owner)
{
	m_dead = false;
    
	m_maxHealth     = def.m_maxHealth;
    m_currentHealth = def.m_currentHealth;
    m_manaCost      = def.m_manaCost;
	m_melee         = def.m_melee;
	m_charging      = def.m_charging;
	m_burrowed      = def.m_burrowed;
	m_leader        = def.m_leader;

	m_climbing   = def. m_climbing;
	m_flying     = def. m_flying;
	m_liquid     = def. m_liquid;
	m_retaliate  = def. m_retaliate;
	m_projectile = def. m_projectile;
	m_regen      = def. m_regen;
	m_growth     = def. m_growth;
	m_temporary  = def. m_temporary;

    m_meleeTimer     = new Timer(m_melee.y, Game::g_gameClock);
    m_chargeTimer    = new Timer(m_charging, Game::g_gameClock);

	m_liquidTimer    = new Timer(.16f, Game::g_gameClock);
	m_drownTimer     = new Timer(10.f, Game::g_gameClock);
	m_temporaryTimer = new Timer((float)m_temporary, Game::g_gameClock);

	m_regenTimer     = new Timer(60.f/(float)m_regen, Game::g_gameClock);
	m_growTimer      = new Timer(60.f / (float)m_growth, Game::g_gameClock);

    m_meleeTimer    ->Start();
    m_chargeTimer   ->Start();
	m_liquidTimer   ->Start();
	m_drownTimer    ->Start();
	m_regenTimer    ->Start();
	m_growTimer     ->Start();
	m_temporaryTimer->Start();

	InitializeSpecialTimer();
	CreateDescriptions();
	OnSpawn();

	
}



void Cube::InitializeSpecialTimer()
{
	CubeType myType = CubeDefinition::ParseTypeFromStr(m_def.m_name);

	if (myType == CubeType::BUNNY) {
	    m_specialTimer = new Timer(4.f, Game::g_gameClock);
		m_specialTimer2 = new Timer(20.f, Game::g_gameClock);
	}

	if (myType == CubeType::MAGIC_MISSILE_MAGE) {
		m_specialTimer = new Timer(5.f, Game::g_gameClock);
	}

	if (myType == CubeType::RAIN_CLOUD) {
		m_specialTimer = new Timer(5.f, Game::g_gameClock);
	}

	if (myType == CubeType::ROYAL_GUARD) {
		m_specialTimer = new Timer(2.f, Game::g_gameClock);
	}

	if (myType == CubeType::SUNFLOWER) {
		m_specialTimer = new Timer(30.f, Game::g_gameClock);
	}

	if (myType == CubeType::TINYCASTLE) {
		m_specialTimer = new Timer(30.f, Game::g_gameClock);
	}
	if(m_specialTimer)
	    m_specialTimer->Start();
    if(m_specialTimer2)
	    m_specialTimer2->Start();
	
}

void Cube::CreateDescriptions()
{
	if (m_melee.x > 0) {
		std::string line = "Melee: Every " + std::to_string(((int)(m_melee.y*100.f)/100)) + " sec, deal " + std::to_string((int)m_melee.x) + " damage to adjacent non allies.";
		m_description.push_back(line);
	}

	if (m_charging > 0) {
		std::string line = "Charging: Every "+std::to_string(((int)(m_charging*100.f)/100))+" sec, move forward.";
		m_description.push_back(line);
	}

	if (m_leader) {
	    std::string line  = "LEADER: Any side without a leader loses the game.";
		m_description.push_back(line);
	}

	if (m_burrowed) {
		std::string line = "Burrowed: Can't be attack by melee unit above.";
		m_description.push_back(line);
	}

	if (m_climbing) {
		std::string line  = "Climbing: Move upwards diagonally when blocked.";
		m_description.push_back(line);
	}

	if (m_flying) {
		std::string line = "Flying: Ignore gravity.";
		m_description.push_back(line);
	}
	if (m_liquid) {
		std::string line  = "Liquid: Randomly move sideways every 1/6 sec,";
		m_description.push_back(line);
		line = "exchange position with cube moving into it.";
		m_description.push_back(line);
	}
	if (m_projectile > 0) {
		std::string line  = "Projectile: Deal "+ std::to_string(m_projectile)+" damage first enemy on contact, then die.";
		m_description.push_back(line);
	}
	if (m_retaliate > 0) {
		std::string line  = "Retaliate: Deal" + std::to_string(m_retaliate)+ " damage back when damaged by non ally.";
		m_description.push_back(line);
	}
	if (m_growth > 0) {
		std::string line = "Grow: Gain max heal " + std::to_string(m_growth)+ " times per minute.";
		m_description.push_back(line);
	}
	if (m_regen > 0) {
		std::string line = "Regenerate: Heal self " + std::to_string(m_regen) + " times for 1 HP per minute.";
		m_description.push_back(line);
	}
	if (m_temporary > 0) {
		std::string line  = "Temporary: This dies after "+ std::to_string(m_temporary)+ " seconds.";
		m_description.push_back(line);
	}

	std::vector<std::string> descriptions= SplitStringOnDelimiter(m_def.m_text,'?');
	for (std::string line : descriptions) {
		m_description.push_back(line);
	}
}

void Cube::PhyscisUpdate()
{
    
	if (m_currentHealth <= 0) {
		m_dead = true;
		OnDeath();

		VFX* deathVFX = new VFX();
		deathVFX->m_timer = new Timer(1.f, Game::g_gameClock);
		deathVFX->m_timer->Start();
		deathVFX->m_texture = &m_def.m_spriteSheet->GetTexture();
		Vec2 BL = m_owner->GridToScreen(m_Position);
		AABB2 uv = m_def.m_spriteSheet->GetSpriteUVsTrimmed(m_def.m_spriteIndex);
		if (m_faction == FactionType::ENEMY) {
			float x = uv.m_mins.x;
			uv.m_mins.x = uv.m_maxs.x;
			uv.m_maxs.x = x;
		}
		AddVertsForRect(deathVFX->m_verts, Vec2(BL.x + CUBESIZE, BL.y), Vec2(BL.x + CUBESIZE, BL.y + CUBESIZE), Vec2(BL.x, BL.y + CUBESIZE), BL, Rgba8(255,255,255,150), uv);
		m_owner->m_VFXs.push_back(deathVFX);
	}

    
    
    //Update based on abilities
    MeleeUpdate();
    ChargingUpDate();
	LiquidUpdate();
	RegenAndGrowUpdate();
	TemporaryUpdate();
	SpecialAbilityUpdate();
    //m_updated=true;
}

void Cube::UpdateGravity()
{
	//gravity
	if (!m_flying) {
		if (m_owner->IsPositionInBound(IntVec2(m_Position.x, m_Position.y - 1)))
			m_owner->MoveCube(this, IntVec2(m_Position.x, m_Position.y - 1));
	}
	
}

void Cube::Render()
{
    g_theRenderer->BindTexture(&m_def.m_spriteSheet->GetTexture());
    std::vector<Vertex_PCU> verts;
    Vec2 BL = m_owner->GridToScreen(m_Position);
    AABB2 uv = m_def.m_spriteSheet->GetSpriteUVsTrimmed(m_def.m_spriteIndex);
    if (m_faction == FactionType::ENEMY) {
        float x = uv.m_mins.x;
        uv.m_mins.x = uv.m_maxs.x;
        uv.m_maxs.x = x;
    }
    AddVertsForRect(verts,AABB2(BL,Vec2(BL.x+CUBESIZE, BL.y+CUBESIZE)), Rgba8::WHITE, uv);//BR TR TL BR TL BL
	if (m_def.m_blockLight) {
		std::vector<bool> adjBlockLight = CheckAdjacentBlockLight();

		Rgba8 slightDark = Rgba8(200, 200, 200);
		Rgba8 deepDark = Rgba8(100,100,100);
		if (adjBlockLight[0] || adjBlockLight[1]) {
			verts[5].m_color = slightDark;
		}
		if (adjBlockLight[0] && adjBlockLight[1]) {
			verts[5].m_color = deepDark;
		}
		if (adjBlockLight[1] || adjBlockLight[2]) {
			verts[0].m_color = slightDark;
			verts[3].m_color = slightDark;
		}
		if (adjBlockLight[1] && adjBlockLight[2]) {
			verts[0].m_color = deepDark;
			verts[3].m_color = deepDark;
		}
		if (adjBlockLight[2] || adjBlockLight[3]) {
			verts[1].m_color = slightDark;
		}
		if (adjBlockLight[2] && adjBlockLight[3]) {
			verts[1].m_color = deepDark;
		}
		if (adjBlockLight[3] || adjBlockLight[0]) {
			verts[2].m_color = slightDark;
			verts[4].m_color = slightDark;
		}
		if (adjBlockLight[3] && adjBlockLight[0]) {
			verts[2].m_color = deepDark;
			verts[4].m_color = deepDark;
		}
	}
	

    g_theRenderer->DrawVertexArray(verts);
}

void Cube::Damage(int value,Cube* other)
{
    int finalValue = value;
    //Apply damage calculation

    if (finalValue < 0) {
        finalValue = 0;
    }
    other->m_currentHealth-=finalValue;
	
	//Retaliate
	if (other->m_retaliate > 0&&other->m_faction!=m_faction) {
		m_currentHealth-=other->m_retaliate;
	}


	g_theAudio->StartSound(Game::m_Damage);
	

    
    //Apply on hit effects
	VFX* vfx = new VFX;
	g_font->AddVertsForText2D(vfx->m_verts, m_owner->GridToScreen(other->m_Position),1.f, std::to_string(value),Rgba8::RED);
	vfx->m_timer= new Timer(.5f, Game::g_gameClock);
	vfx->m_timer->Start();
	vfx->m_texture = &g_font->GetTexture();
	m_owner->m_VFXs.push_back(vfx);
}

void Cube::Heal(int value, Cube* other)
{
	int finalValue = value;
	//Apply damage calculation

	if (finalValue < 0) {
		finalValue = 0;
	}
	other->m_currentHealth += finalValue;
	g_theAudio->StartSound(Game::m_Heal);

	//Apply on heal effects
}

void Cube::MeleeUpdate()
{
    if (m_melee.x != 0) {
		std::vector<Cube*> hitlist;
		while (m_meleeTimer->DecrementPeriodIfElapsed()) {

			Cube* cube;

			if (m_owner->IsPositionInBound(IntVec2(m_Position.x + 1, m_Position.y))) {
				cube = m_owner->m_cubesOnMap[m_owner->PosToIndex(IntVec2(m_Position.x + 1, m_Position.y))];
				if (cube && cube->m_faction != m_faction) {
					hitlist.push_back(cube);
				}
			}

			if (m_owner->IsPositionInBound(IntVec2(m_Position.x - 1, m_Position.y))) {
				cube = m_owner->m_cubesOnMap[m_owner->PosToIndex(IntVec2(m_Position.x - 1, m_Position.y))];
				if (cube && cube->m_faction != m_faction) {
					hitlist.push_back(cube);
				}
			}

			if (m_owner->IsPositionInBound(IntVec2(m_Position.x, m_Position.y + 1))) {
				cube = m_owner->m_cubesOnMap[m_owner->PosToIndex(IntVec2(m_Position.x, m_Position.y + 1))];
				if (cube && cube->m_faction != m_faction) {
					hitlist.push_back(cube);
				}
			}

			if (m_owner->IsPositionInBound(IntVec2(m_Position.x, m_Position.y - 1))) {
				cube = m_owner->m_cubesOnMap[m_owner->PosToIndex(IntVec2(m_Position.x, m_Position.y - 1))];
				if (cube && cube->m_faction != m_faction && !cube->m_burrowed) {
					hitlist.push_back(cube);
				}
			}
		}

		for (Cube* cube : hitlist) {
			Damage((int)m_melee.x, cube);

			g_theAudio->StartSound(Game::m_Melee);
			
		}
    }   
}

void Cube::ChargingUpDate()
{
    if (m_charging != 0) {
		while (m_chargeTimer->DecrementPeriodIfElapsed()) {
			IntVec2 TargetPos = GetFront();

			if (m_owner->IsPositionInBound(TargetPos))
			{
				if(m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)]){
				    if(m_climbing)
					    TargetPos.y+=1;
				}
				m_owner->MoveCube(this, TargetPos);
			}
				
		}
    }
    
}

void Cube::LiquidUpdate()
{
	if (m_liquid) {
		if (m_liquidTimer->DecrementPeriodIfElapsed()) {
		    IntVec2 left = IntVec2(m_Position.x - 1, m_Position.y);
		    bool leftClear = m_owner->IsPositionInBound(left) && !m_owner->m_cubesOnMap[m_owner->PosToIndex(left)];
			IntVec2 right = IntVec2(m_Position.x + 1, m_Position.y);
			bool rightClear = m_owner->IsPositionInBound(right) && !m_owner->m_cubesOnMap[m_owner->PosToIndex(right)];

			if (leftClear && rightClear) {
				if (g_RNG->RollRandomFloatZeroToOne() > .5f)
					m_owner->MoveCube(this, left);
				else
				    m_owner->MoveCube(this, right);
			}
			else {
				if (leftClear)
				    m_owner->MoveCube(this, left);
				if(rightClear)
				    m_owner->MoveCube(this, right);
			}

		}
		if (m_drownTimer->DecrementPeriodIfElapsed()) {
		    IntVec2 firstEnemyPos = m_Position;
			Cube* firstEnemy = nullptr;
			while (m_owner->IsPositionInBound(firstEnemyPos) && !firstEnemy) {
				
				if (m_owner->m_cubesOnMap[m_owner->PosToIndex(firstEnemyPos)] && m_owner->m_cubesOnMap[m_owner->PosToIndex(firstEnemyPos)]->m_faction != m_faction) {
					firstEnemy = m_owner->m_cubesOnMap[m_owner->PosToIndex(firstEnemyPos)];
				}
				firstEnemyPos = IntVec2(firstEnemyPos.x, firstEnemyPos.y - 1);
			}

			if (firstEnemy) {
				Damage(1, firstEnemy);
			}
		}
	}
}

void Cube::RegenAndGrowUpdate()
{
	if (m_regen > 0) {
		while (m_regenTimer->DecrementPeriodIfElapsed()) {
			Heal(1, this);
	    }  
	}

	if (m_growth > 0) {
		while (m_growTimer->DecrementPeriodIfElapsed()) {
			m_maxHealth += 1;
			Heal(1, this);
	    }
	}
}

void Cube::TemporaryUpdate()
{
	if (m_temporary > 0 && m_temporaryTimer->DecrementPeriodIfElapsed()) {
		m_dead=true;
		OnDeath();
	}
}

void Cube::OnSpawn()
{
	CubeType myType = CubeDefinition::ParseTypeFromStr(m_def.m_name);
	if (myType == CubeType::EXCAVATE) {
	    IntVec2 below = IntVec2(m_Position.x, m_Position.y-1);
		if (m_owner->IsPositionInBound(below) && m_owner->m_cubesOnMap[m_owner->PosToIndex(below)]) {
			Damage(5,m_owner->m_cubesOnMap[m_owner->PosToIndex(below)]);
		}
	}
}

void Cube::OnDeath()
{
	CubeType myType = CubeDefinition::ParseTypeFromStr(m_def.m_name);
	if (myType == CubeType::LARGE_WARRIOR_SLIME) {
		m_owner->m_cubesOnMap[m_owner->PosToIndex(m_Position)] = new Cube(*(CubeDefinition::g_cubeDefinitions[(int)CubeType::MEDIUM_WARRIOR_SLIME]), m_Position, m_faction, m_owner);
		delete this;
	}

	if (myType == CubeType::MEDIUM_WARRIOR_SLIME) {
		m_owner->m_cubesOnMap[m_owner->PosToIndex(m_Position)] = new Cube(*(CubeDefinition::g_cubeDefinitions[(int)CubeType::SMALL_WARRIOR_SLIME]), m_Position, m_faction, m_owner);
		delete this;
	}
}

void Cube::SpecialAbilityUpdate()
{
	CubeType myType = CubeDefinition::ParseTypeFromStr(m_def.m_name);
	if (m_specialTimer&&m_specialTimer->DecrementPeriodIfElapsed()) {
		if (myType == CubeType::BUNNY) {
			m_owner->MoveCube(this, IntVec2(m_Position.x, m_Position.y + 1));
		}

		if (myType == CubeType::MAGIC_MISSILE_MAGE) {
			m_owner->SpawnCube(CubeType::MAGIC_MISSILE, IntVec2(m_Position.x, m_Position.y + 1), m_faction);
		}

		if (myType == CubeType::RAIN_CLOUD) {
			m_owner->SpawnCube(CubeType::WATER, IntVec2(m_Position.x, m_Position.y - 1), FactionType::NEUTRAL);
		}

		if (myType == CubeType::ROYAL_GUARD) {
			Cube* leader = nullptr;
			if (m_faction == FactionType::ALLY) {
			    leader = m_owner->m_ally->m_UI->m_leader;
		    }
			if (m_faction == FactionType::ENEMY) {
				leader = m_owner->m_enemy->m_UI->m_leader;
			}
			if(leader&&leader->m_currentHealth<leader->m_maxHealth)
			    m_owner->MoveCube(this, GetFront());
		}

		if (myType == CubeType::SUNFLOWER) {
		    m_owner->MoveCube(this, IntVec2(m_Position.x, m_Position.y+1));
			m_owner->SpawnCube(CubeType::PLANT_STEM, IntVec2(m_Position.x, m_Position.y - 1), m_faction);
		}

		if (myType == CubeType::TINYCASTLE) {
		    m_owner->SpawnCube(CubeType::KNIGHT, IntVec2(m_Position.x, m_Position.y+1), m_faction);
		}
	}

	if (myType == CubeType::BUNNY&&m_specialTimer2->DecrementPeriodIfElapsed()) {
	    m_owner->SpawnCube(CubeType::BUNNY,GetFront(), m_faction);
	}
	
}

IntVec2 Cube::GetFront()
{
	IntVec2 TargetPos = m_Position;
	if (m_faction == FactionType::ALLY)
		TargetPos = IntVec2(TargetPos.x + 1, TargetPos.y);
	if (m_faction == FactionType::ENEMY)
		TargetPos = IntVec2(TargetPos.x - 1, TargetPos.y);

	return TargetPos;
}

std::vector<bool> Cube::CheckAdjacentBlockLight()
{
    std::vector<bool> result;
	result.resize(4);
	IntVec2 TargetPos = IntVec2(m_Position.x-1, m_Position.y);
	if (!m_owner->IsPositionInBound(TargetPos)) {
	    result[0] = true;
	}
	else {
		result[0] = (m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)] && m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)]->m_def.m_blockLight);
	}

	TargetPos = IntVec2(m_Position.x, m_Position.y-1);
	if (!m_owner->IsPositionInBound(TargetPos)) {
		result[1] = true;
	}
	else {
		result[1] = (m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)] && m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)]->m_def.m_blockLight);
	}

	TargetPos = IntVec2(m_Position.x+1, m_Position.y);
	if (!m_owner->IsPositionInBound(TargetPos)) {
		result[2] = true;
	}
	else {
		result[2] = (m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)] && m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)]->m_def.m_blockLight);
	}

	TargetPos = IntVec2(m_Position.x, m_Position.y+1);
	if (!m_owner->IsPositionInBound(TargetPos)) {
		result[3] = true;
	}
	else {
		result[3] = (m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)] && m_owner->m_cubesOnMap[m_owner->PosToIndex(TargetPos)]->m_def.m_blockLight);
	}
	
	return result;
}
