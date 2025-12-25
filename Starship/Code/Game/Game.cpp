#include "Game.hpp"
#include "GameCommon.hpp"
#include"PlayerShip.hpp"
#include"Bullet.hpp"
#include"Asteroid.hpp"
#include"Beetle.hpp"
#include"Wasp.hpp"
#include "Debris.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include <math.h>

void Game::StartUp() {
	loadSound();
	 g_WorldCamera = new Camera();
	 g_WorldCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	 g_ScreenCamera = new Camera();
	 g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(1600, 800));
	
	g_RNG = new RandomNumberGenerator();

	m_bulletList = new Bullet*[MAX_BULLETS];
	for (int i = 0; i < MAX_BULLETS; i++) {
		m_bulletList[i] = nullptr;
	}
	m_asteroidList = new Asteroid*[MAX_ASTEROIDS];
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		m_asteroidList[i] = nullptr;
	}
	m_beetleList = new Beetle * [MAX_BEETLE];
	for (int i = 0; i < MAX_BEETLE; i++) {
		m_beetleList[i] = nullptr;
	}
	m_waspList = new Wasp * [MAX_WASP];
	for (int i = 0; i < MAX_WASP; i++) {
		m_waspList[i] = nullptr;
	}
	m_debrisList = new Debris * [MAX_DEBRIS];
	for (int i = 0; i < MAX_DEBRIS; i++) {
		m_debrisList[i] = nullptr;
	}
	g_theRenderer->Set2DDefault();
}
void Game::Update(float deltaSecond) {
	AdjustSound();
	if (m_attractMode) {
		
		if (m_shrink) {
			m_attractScale -= 0.01f*deltaSecond;
			if (m_attractScale <= .8f) {
				m_shrink = false;
			}
		}
		else {
			m_attractScale += 0.01f *deltaSecond;
			if (m_attractScale >= 1.2f) {
				m_shrink = true;
			}
		}
		if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
			XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
			if (controller.IsConnected() && (controller.WasButtonReleased(XBOX_BUTTON_A) || controller.WasButtonReleased(XBOX_BUTTON_START))) {
				ExitAttractMode();

			}
			else if (g_theInput->WasKeyJustReleased(' ') || g_theInput->WasKeyJustReleased('N')) {
				ExitAttractMode();

			}
		}
	}
	else {
		PeriodicalAsteroid(deltaSecond);
		UpdateEntity(deltaSecond);
		HandleKeyPress();
		trySpawnEnemy();
		if (WaveClear()) {
			SpawnWave();
		}

		CheckCollision();
	}
	
}

void Game::UpdateEntity(float deltaSecond) {
	if (g_player->IsAlive()) {
		g_player->Update(deltaSecond);
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			m_bulletList[i]->Update(deltaSecond);
		}
	}
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			m_asteroidList[i]->Update(deltaSecond);
		}
	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			m_beetleList[i]->Update(deltaSecond);
		}
	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			m_waspList[i]->Update(deltaSecond);
		}
	}
	for (int i = 0; i < MAX_DEBRIS; i++) {
		if (m_debrisList[i] != nullptr) {
			m_debrisList[i]->Update(deltaSecond);
		}
	}
}
void Game::Render() {
	if (m_attractMode) {
		
		g_theRenderer->BeginCamera(*g_ScreenCamera);
		g_theRenderer->ClearScreen(Rgba8::BLACK);
		RenderAttractMode();
		g_theRenderer->EndCamera(*g_ScreenCamera);
	}
	else {
		g_theRenderer->ClearScreen(Rgba8::BLACK);
		if (m_trauma > 0) {
			ApplyScreenShake();
			m_trauma--;
		}
		g_theRenderer->BeginCamera(*g_ScreenCamera);
		RenderUI();
		g_theRenderer->EndCamera(*g_ScreenCamera);
		g_theRenderer->BeginCamera(*g_WorldCamera);
		RenderWorld();
		g_theRenderer->EndCamera(*g_WorldCamera);
		
	}
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(16, 8));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	g_theRenderer->EndCamera(*g_ScreenCamera);
	g_theRenderer->BindTexture(nullptr);

}
void Game::RenderWorld() {
	if (g_player->IsAlive()) {
		g_player->Render();
		/*
		* Vec2 hpPos = g_player->m_position;
		hpPos.x -= g_player->m_cosmeticRadius / 2;
		hpPos.y += g_player->m_cosmeticRadius / 2;
		*/
		
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			m_bulletList[i]->Render();
		}
	}
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			m_asteroidList[i]->Render();
			Vec2 hpPos = m_asteroidList[i]->m_position;
			hpPos.x -= m_asteroidList[i]->m_cosmeticRadius / 2;
			hpPos.y += m_asteroidList[i]->m_cosmeticRadius / 2;
			renderHPBar(hpPos, 0.1f, ASTEROID_MAXHP, (float)m_asteroidList[i]->m_health);
		}
	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			m_beetleList[i]->Render();
			Vec2 hpPos = m_beetleList[i]->m_position;
			hpPos.x -= m_beetleList[i]->m_cosmeticRadius/2;
			hpPos.y += m_beetleList[i]->m_cosmeticRadius/2;
			renderHPBar(hpPos, 0.1f, BEETLE_MAXHP, (float)m_beetleList[i]->m_health);
		}
	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			m_waspList[i]->Render();
			Vec2 hpPos = m_waspList[i]->m_position;
			hpPos.x -= m_waspList[i]->m_cosmeticRadius / 2;
			hpPos.y += m_waspList[i]->m_cosmeticRadius / 2;
			renderHPBar(hpPos, 0.1f, WASP_MAXHP, (float)m_waspList[i]->m_health);
		}
	}
	for (int i = 0; i < MAX_DEBRIS; i++) {
		if (m_debrisList[i] != nullptr) {
			m_debrisList[i]->Render();
		}
	}

	if (m_IsDebugging)
		DebugRender();
}
void Game::RenderUI() {
	RenderLives();
	renderHPBar(Vec2(10.f,750.f), 2.f, g_player->m_maxHP, (float)g_player->m_health);
}
void Game::ApplyScreenShake() {
	float MaxShake = m_trauma * m_trauma * 0.001f;
	Vec2 shake = Vec2(g_RNG->RollRandomFloatInRange(-MaxShake, MaxShake), 0);
	g_WorldCamera->Translate2D(shake);
}
void Game::SpawnBullet(Vec2 position, float directionDegree) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] == nullptr) {
			m_bulletList[i] = new Bullet(this, position, directionDegree);
			m_bulletList[i]->m_scale += 0.2f * (float)g_player->m_damage;
			g_theAudio->StartSound(m_Shoot,false,m_volume);
			return;
		}
	}
	ERROR_RECOVERABLE("Bullet Full");
	

}
void Game::SpawnAsteroid() {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] == nullptr) {
			RandomNumberGenerator* rand = new RandomNumberGenerator();
			Vec2 pos = EnemySpawnLocation(ASTEROID_COSMETIC_RADIUS);
			m_asteroidList[i] = new Asteroid(this, pos, rand->RollRandomFloatInRange(0, 359));
			return;
		}
	}
	ERROR_RECOVERABLE("Asteroid Full");
	
}
void Game::SpawnBeetle() {
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] == nullptr) {
			m_beetleList[i] = new Beetle(this, EnemySpawnLocation(4), 0);
			m_beetleRemain--;
			return;
		}
	}
	//ERROR_RECOVERABLE("Beetle Full");
}
void Game::SpawnWasp() {
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] == nullptr) {
			m_waspList[i] = new Wasp(this, EnemySpawnLocation(6), 0);
			m_waspRemain--;
			return;
		}
	}
	//ERROR_RECOVERABLE("Wasp Full");
}

void Game::SpawnDebris(Vec2 position, Vec2 initialVerlocity,Rgba8 parentColor, int num) {
	int j = 0;
	while (j<num) {
		for (int i = 0; i < MAX_DEBRIS; i++) {
			if (j == num) {
				return;
			}
			if (m_debrisList[i] == nullptr) {
				float degree = g_RNG->RollRandomFloatInRange(0, 360);
				m_debrisList[i] = new Debris(this, position, degree,initialVerlocity, parentColor);
				j++;
				
			}
		}
	}
	
}
void Game::RemoveEntity(){
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			if (m_asteroidList[i]->m_isGarbage == true) {
				int num = g_RNG->RollRandomIntInRange(3, 12);
				SpawnDebris(m_asteroidList[i]->m_position, m_asteroidList[i]->m_verlocity, Rgba8(100, 100, 100), num);
				delete m_asteroidList[i];
				m_asteroidList[i] = nullptr;
				g_theAudio->StartSound(m_EnemyDeath,false,m_volume);
			}
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			if (m_bulletList[i]->m_isGarbage == true) {
				int num = g_RNG->RollRandomIntInRange(1, 3);
				SpawnDebris(m_bulletList[i]->m_position, m_bulletList[i]->m_verlocity, Rgba8(255, 0, 0), num);
				delete m_bulletList[i];
				m_bulletList[i] = nullptr;
			}
		}
	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			if (m_beetleList[i]->m_isGarbage == true) {
				int num = g_RNG->RollRandomIntInRange(3, 12);
				SpawnDebris(m_beetleList[i]->m_position, m_beetleList[i]->m_verlocity, Rgba8(0, 255, 0), num);
				delete m_beetleList[i];
				m_beetleList[i] = nullptr;
				g_theAudio->StartSound(m_EnemyDeath,false,m_volume);
			}
		}
	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			if (m_waspList[i]->m_isGarbage == true) {
				int num = g_RNG->RollRandomIntInRange(3, 12);
				SpawnDebris(m_waspList[i]->m_position, m_waspList[i]->m_verlocity, Rgba8(255, 255, 0), num);
				delete m_waspList[i];
				m_waspList[i] = nullptr;
				g_theAudio->StartSound(m_EnemyDeath,false,m_volume);
			}
		}
	}
	for (int i = 0; i < MAX_DEBRIS; i++) {
		if (m_debrisList[i] != nullptr) {
			if (m_debrisList[i]->m_isGarbage == true) {
				delete m_debrisList[i];
				m_debrisList[i] = nullptr;
			}
		}
	}
}
void Game::CheckCollision() {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			//player collision
			if (m_asteroidList[i]->Collide(g_player)) {
				//m_asteroidList[i]->m_health-=g_player->m_damage;
				g_player->m_health-=m_asteroidList[i]->m_damage;
				
				if (g_player->m_health <= 0) {
					PlayerDeath();
				}
				else {
					g_player->KnockedBack(m_asteroidList[i]->m_position);
					int j=g_RNG->RollRandomIntLessThan(3);
					if (j==0) {
						g_theAudio->StartSound(m_Hurt1,false,m_volume);
					}
					else if (j == 1) {
						g_theAudio->StartSound(m_Hurt2, false, m_volume);
					}
					else {
						g_theAudio->StartSound(m_Hurt3, false, m_volume);
					}
				}
				
			}
			//bullet collision
			for (int j = 0; j < MAX_BULLETS; j++) {
				if (m_bulletList[j] != nullptr) {
					if (m_asteroidList[i]->Collide(m_bulletList[j])) {
						m_asteroidList[i]->m_health -= 3*(int)m_bulletList[j]->m_scale;
						//overkill
						if (m_asteroidList[i]->m_health < m_asteroidList[i]->m_health / 4) {
							m_bulletList[j]->m_scale *= 0.5;
						}
						else {
							m_bulletList[j]->m_isGarbage = true;
						}
						
					}

				}
				
			}
		}
	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			//player collision
			if (m_beetleList[i]->Collide(g_player)) {
				//m_beetleList[i]->m_health -= g_player->m_damage;
				g_player->m_health-=m_beetleList[i]->m_damage;
				if (g_player->m_health <= 0) {
					PlayerDeath();
				}
				else {
					g_player->KnockedBack(m_beetleList[i]->m_position);
					int j = g_RNG->RollRandomIntLessThan(3);
					if (j == 0) {
						g_theAudio->StartSound(m_Hurt1, false, m_volume);
					}
					else if (j == 1) {
						g_theAudio->StartSound(m_Hurt2, false, m_volume);
					}
					else {
						g_theAudio->StartSound(m_Hurt3, false, m_volume);
					}
				}
			}
			//bullet collision
			for (int j = 0; j < MAX_BULLETS; j++) {
				if (m_bulletList[j] != nullptr) {
					if (m_beetleList[i]->Collide(m_bulletList[j])) {
						m_beetleList[i]->m_health -= 3 * (int)m_bulletList[j]->m_scale;
						//overkill
						if (m_beetleList[i]->m_health < m_beetleList[i]->m_health / 4) {
							m_bulletList[j]->m_scale *= 0.5;
						}
						else {
							m_bulletList[j]->m_isGarbage = true;
						}
					}
					

				}

			}
		}
	}

	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			//player collision
			if (m_waspList[i]->Collide(g_player)) {
				//m_waspList[i]->m_health -= g_player->m_damage;
				g_player->m_health-=m_waspList[i]->m_damage;
				if (g_player->m_health <= 0) {
					PlayerDeath();
				}
				else {
					g_player->KnockedBack(m_waspList[i]->m_position);
					int soundIndex = g_RNG->RollRandomIntLessThan(3);
					if (soundIndex == 0) {
						g_theAudio->StartSound(m_Hurt1, false, m_volume);
					}
					else if (soundIndex == 1) {
						g_theAudio->StartSound(m_Hurt2, false, m_volume);
					}
					else {
						g_theAudio->StartSound(m_Hurt3, false, m_volume);
					}
				}
			}
			//bullet collision
			for (int j = 0; j < MAX_BULLETS; j++) {
				if (m_bulletList[j] != nullptr) {
					if (m_waspList[i]->Collide(m_bulletList[j])) {
						m_waspList[i]->m_health -= 3 * (int)m_bulletList[j]->m_scale;
						//overkill
						if (m_waspList[i]->m_health < m_waspList[i]->m_health / 4) {
							m_bulletList[j]->m_scale *= 0.5;
						}
						else {
							m_bulletList[j]->m_isGarbage = true;
						}
					}

				}

			}
		}
	}
}
void Game::DebugRender() {
	//entity to player
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			DebugDrawLine(g_player->m_position, m_asteroidList[i]->m_position, 0.2f, Rgba8(50, 50, 50));
			
		}	
	}for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			DebugDrawLine(g_player->m_position, m_bulletList[i]->m_position, 0.2f, Rgba8(50, 50, 50));

		}

	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			DebugDrawLine(g_player->m_position, m_beetleList[i]->m_position, 0.2f, Rgba8(50, 50, 50));

		}

	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			DebugDrawLine(g_player->m_position, m_waspList[i]->m_position, 0.2f, Rgba8(50, 50, 50));

		}

	}


	//forward vector
	Vec2 temp(PLAYER_SHIP_COSMETIC_RADIUS, 0);
	temp.RotateDegrees(g_player->m_orientationDegrees);
	DebugDrawLine(g_player->m_position, g_player->m_position + temp, 0.2f, Rgba8(255, 0, 0));
	for (int i = 0; i < MAX_ASTEROIDS; i++) {

		if (m_asteroidList[i] != nullptr) {
			Vec2 rad(m_asteroidList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_asteroidList[i]->m_orientationDegrees);
			DebugDrawLine(m_asteroidList[i]->m_position, m_asteroidList[i]->m_position + rad, 0.2f, Rgba8(255, 0, 0));

		}	
	}for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			Vec2 rad(m_beetleList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_beetleList[i]->m_orientationDegrees);
			DebugDrawLine(m_beetleList[i]->m_position, m_beetleList[i]->m_position + rad, 0.2f, Rgba8(255, 0, 0));

		}

	}
	for (int i = 0; i < MAX_WASP; i++) {

		if (m_waspList[i] != nullptr) {
			Vec2 rad(m_waspList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_waspList[i]->m_orientationDegrees);
			DebugDrawLine(m_waspList[i]->m_position, m_waspList[i]->m_position + rad, 0.2f, Rgba8(255, 0, 0));

		}
	}

	//left vector
	temp.RotateDegrees(-90);
	DebugDrawLine(g_player->m_position, g_player->m_position+ temp, 0.2f, Rgba8(0, 255, 0));
	for (int i = 0; i < MAX_ASTEROIDS; i++) {

		if (m_asteroidList[i] != nullptr) {
			Vec2 rad(m_asteroidList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_asteroidList[i]->m_orientationDegrees-90);
			DebugDrawLine(m_asteroidList[i]->m_position, m_asteroidList[i]->m_position + rad, 0.2f, Rgba8(0, 255, 0));

		}
	}for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			Vec2 rad(m_bulletList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_bulletList[i]->m_orientationDegrees-90);
			DebugDrawLine(m_bulletList[i]->m_position, m_bulletList[i]->m_position + rad, 0.2f, Rgba8(0, 255, 0));

		}

	}
	for (int i = 0; i < MAX_BEETLE; i++) {

		if (m_beetleList[i] != nullptr) {
			Vec2 rad(m_beetleList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_beetleList[i]->m_orientationDegrees - 90);
			DebugDrawLine(m_beetleList[i]->m_position, m_beetleList[i]->m_position + rad, 0.2f, Rgba8(0, 255, 0));

		}
	}for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			Vec2 rad(m_waspList[i]->m_cosmeticRadius, 0);
			rad.RotateDegrees(m_waspList[i]->m_orientationDegrees - 90);
			DebugDrawLine(m_waspList[i]->m_position, m_waspList[i]->m_position + rad, 0.2f, Rgba8(0, 255, 0));

		}

	}
	//verlocity vector
	DebugDrawLine(g_player->m_position, g_player->m_position + g_player->m_verlocity, 0.2f, Rgba8(255, 255, 0));
	for (int i = 0; i < MAX_ASTEROIDS; i++) {

		if (m_asteroidList[i] != nullptr) {
			DebugDrawLine(m_asteroidList[i]->m_position, m_asteroidList[i]->m_position + m_asteroidList[i]->m_verlocity, 0.2f, Rgba8(255, 255, 0));

		}
	}for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			DebugDrawLine(m_bulletList[i]->m_position, m_bulletList[i]->m_position + m_bulletList[i]->m_verlocity, 0.2f, Rgba8(255, 255, 0));

		}

	}
	for (int i = 0; i < MAX_BEETLE; i++) {

		if (m_beetleList[i] != nullptr) {
			DebugDrawLine(m_beetleList[i]->m_position, m_beetleList[i]->m_position + m_beetleList[i]->m_verlocity, 0.2f, Rgba8(255, 255, 0));

		}
	}for (int i = 0; i < MAX_WASP; i++) {
		if (m_bulletList[i] != nullptr) {
			DebugDrawLine(m_waspList[i]->m_position, m_waspList[i]->m_position + m_waspList[i]->m_verlocity, 0.2f, Rgba8(255, 255, 0));

		}

	}
	//inner circle
	DebugDrawRing(g_player->m_position, PLAYER_SHIP_PHYSICS_RADIUS, 0.2f, Rgba8(0, 255, 255));
	for (int i = 0; i < MAX_ASTEROIDS; i++) {

		if (m_asteroidList[i] != nullptr) {
			DebugDrawRing(m_asteroidList[i]->m_position, m_asteroidList[i]->m_physicsRadius, 0.2f, Rgba8(0, 255, 255));

		}
	}for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			DebugDrawRing(m_bulletList[i]->m_position, m_bulletList[i]->m_physicsRadius, 0.2f, Rgba8(0, 255, 255));

		}

	}
	for (int i = 0; i < MAX_BEETLE; i++) {

		if (m_beetleList[i] != nullptr) {
			DebugDrawRing(m_beetleList[i]->m_position, m_beetleList[i]->m_physicsRadius, 0.2f, Rgba8(0, 255, 255));

		}
	}for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			DebugDrawRing(m_waspList[i]->m_position, m_waspList[i]->m_physicsRadius, 0.2f, Rgba8(0, 255, 255));

		}

	}
	//outer circle
	DebugDrawRing(g_player->m_position, PLAYER_SHIP_COSMETIC_RADIUS, 0.2f, Rgba8(255, 0, 255));
	for (int i = 0; i < MAX_ASTEROIDS; i++) {

		if (m_asteroidList[i] != nullptr) {
			DebugDrawRing(m_asteroidList[i]->m_position, m_asteroidList[i]->m_cosmeticRadius, 0.2f, Rgba8(255,0, 255));

		}
	}for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bulletList[i] != nullptr) {
			DebugDrawRing(m_bulletList[i]->m_position, m_bulletList[i]->m_cosmeticRadius, 0.2f,Rgba8(255,0, 255));

		}

	}
	for (int i = 0; i < MAX_BEETLE; i++) {

		if (m_beetleList[i] != nullptr) {
			DebugDrawRing(m_beetleList[i]->m_position, m_beetleList[i]->m_cosmeticRadius, 0.2f, Rgba8(255, 0, 255));

		}
	}for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			DebugDrawRing(m_waspList[i]->m_position, m_waspList[i]->m_cosmeticRadius, 0.2f, Rgba8(255, 0, 255));

		}

	}
	
}
Vec2 Game::EnemySpawnLocation(float radius) {
	RandomNumberGenerator* rand = new RandomNumberGenerator();
	float side = (float)rand->RollRandomIntLessThan(4);
	float x = -radius;
	float y = -radius;
	//up
	if (side == 0) {
		x = rand->RollRandomFloatInRange(0, WORLD_SIZE_X);
	}
	//down
	if (side == 1) {
		y = WORLD_SIZE_Y + radius;
		x = rand->RollRandomFloatInRange(0, WORLD_SIZE_X);
	}
	//left
	if (side == 2) {
		y = rand->RollRandomFloatInRange(0, WORLD_SIZE_Y);
	}
	//right
	if (side == 3) {
		x = WORLD_SIZE_X + radius;
		y = rand->RollRandomFloatInRange(0, WORLD_SIZE_Y);
	}
	return Vec2(x, y);
}
void Game::Cheat() {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			m_asteroidList[i]->m_isGarbage=true;
		}
	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			m_beetleList[i]->m_isGarbage = true;
		}
	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			m_waspList[i]->m_isGarbage = true;
		}
	}
}
void Game::PlayerDeath() {
	int num = g_RNG->RollRandomIntInRange(5, 30);
	SpawnDebris(g_player->m_position, g_player->m_verlocity, Rgba8(102, 153, 204), num);
	g_theAudio->StartSound(m_Death,false,m_volume);
	m_trauma = 180;
}
void Game::RenderLives() {
	float dist = 16*PLAYER_SHIP_COSMETIC_RADIUS;
	float deg = 90;
	for (int i = 0; i < m_live; i++) {
		Rgba8 color(102, 153, 204);
		float x = i*dist+PLAYER_SHIP_COSMETIC_RADIUS;
		float y = PLAYER_SHIP_COSMETIC_RADIUS;
		Vec2 uv(0, 0);
		//Triangle A
		Vec3 A1Pos(x, y, 0);
		Vec3 A1Rotation(16, 8, 0);
		A1Pos += A1Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU A1(A1Pos, color, uv);
		Vec3 A2Pos(x, y, 0);
		Vec3 A2Rotation(0, 16, 0);
		A2Pos += A2Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU A2(A2Pos, color, uv);
		Vec3 A3Pos(x, y, 0);
		Vec3 A3Rotation(-16, 8, 0);
		A3Pos += A3Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU A3(A3Pos, color, uv);
		//triangle B
		Vertex_PCU B1 = A3;
		Vec3 B2Pos(x, y, 0);
		Vec3 B2Rotation(0, 8, 0);
		B2Pos += B2Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU B2(B2Pos, color, uv);
		Vec3 B3Pos(x, y, 0);
		Vec3 B3Rotation(-16, -8, 0);
		B3Pos += B3Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU B3(B3Pos, color, uv);
		//triangle c
		Vertex_PCU C1 = B2;
		Vertex_PCU C2 = B3;
		Vec3 C3Pos(x, y, 0);
		Vec3 C3Rotation(0, -8, 0);
		C3Pos += C3Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU C3(C3Pos, color, uv);
		//triangle d
		Vertex_PCU D1 = B2;
		Vertex_PCU D2 = C3;
		Vec3 D3Pos(x, y, 0);
		Vec3 D3Rotation(8, 0, 0);
		D3Pos += D3Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU D3(D3Pos, color, uv);
		//triangle e
		Vertex_PCU	E1 = B3;
		Vec3 E2Pos(x, y, 0);
		Vec3 E2Rotation(0, -16, 0);
		E2Pos += E2Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU E2(E2Pos, color, uv);
		Vec3 E3Pos(x, y, 0);
		Vec3 E3Rotation(16, -8, 0);
		E3Pos += E3Rotation.GetRotatedAboutZDegrees(deg);
		Vertex_PCU E3(E3Pos, color, uv);

		Vertex_PCU* arr = new Vertex_PCU[15];
		arr[0] = A1;
		arr[1] = A2;
		arr[2] = A3;
		arr[3] = B1;
		arr[4] = B2;
		arr[5] = B3;
		arr[6] = C1;
		arr[7] = C2;
		arr[8] = C3;
		arr[9] = D1;
		arr[10] = D2;
		arr[11] = D3;
		arr[12] = E1;
		arr[13] = E2;
		arr[14] = E3;
		for (i = 0; i < 15; i++) {
			arr[i].m_position += Vec3(20, 780, 0);
		}
		g_theRenderer->DrawVertexArray(15, arr);
	}
}
void Game::SpawnWave() {
    g_theAudio->StartSound(m_WaveStart,false,m_volume);
	for (int i = 0; i < 3+m_wave*3; i++) {
		/*if (m_wave <= 3)
			SpawnBeetle ();
		//SpawnWasp();
		else
			if (m_wave == 4) {
				SpawnWasp();
				break;
			}
			else {
				SpawnWasp();
				SpawnWasp();
				break;
			}*/
		int maxRatio = m_wave;
		if (m_wave > 10) {
			maxRatio = 10;
		}
		if (g_RNG->RollRandomIntLessThan(maxRatio) > 5) {
			m_waspRemain++;
		}
		else {
			m_beetleRemain++;
		}
		
		    
		//SpawnBeetle();
	}
	m_wave++;
}
bool Game::WaveClear() {
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			return false;
		}
	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			return false;
		}
	}
	//player upgrade
	g_player->m_damage+=1;
	g_player->m_maxHP += 10;
	g_player->m_health = (int)g_player->m_maxHP;
	g_player->m_shootRate+=2;
	if (g_player->m_shootRate >= 16) {
		g_player->m_shootRate = 10;
		g_player->m_multiShoot++;
	}
	m_live++;
	return true;
}
void Game::RenderAttractMode() {
	Vertex_PCU* startIcon = new Vertex_PCU[3];
	for (int i = 0; i < 3; i++) {
		startIcon[i].m_color = Rgba8(50, 255, 50);
		startIcon[i].m_uvTexCoords = Vec2(0, 0);
	}
	startIcon[0].m_position = Vec3((WORLD_CENTER_X - 100 * m_attractScale) , (WORLD_CENTER_Y - 100 * m_attractScale), 0);
	startIcon[1].m_position = Vec3((WORLD_CENTER_X - 100 * m_attractScale) , (WORLD_CENTER_Y + 100 * m_attractScale), 0);
	startIcon[2].m_position = Vec3((WORLD_CENTER_X + 100 * m_attractScale) , (WORLD_CENTER_Y), 0);

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(3, startIcon);
}
bool Game::IsOver() {
	if (m_attractMode) {
		return false;
	}
	else {
		bool lose=!g_player->IsAlive() && m_live == 0;
		bool win = (m_wave >5) && lose;
		if (win) {
			lose = false;
			m_VictoryPlayBack = g_theAudio->StartSound(m_Victory, true, m_volume);
		}
		if (lose) {
			m_DefeatPlayBack = g_theAudio->StartSound(m_Defeat, true, m_volume);
		}
		return lose || win;
	}
}
void Game::EnterAttractMode() {
	m_attractScale = 1;
	g_theAudio->StopSound(m_BGMPlayBack);
	delete g_player;
	g_player = nullptr;
	m_wave = 1;
	m_live = 0;
	m_attractMode = true;
	for (int i = 0; i < MAX_BULLETS;i++) {
		if (m_bulletList[i] != nullptr) {
			delete m_bulletList[i];
			m_bulletList[i] = nullptr;
		}
	}
	for (int i = 0; i < MAX_DEBRIS; i++) {
		if (m_debrisList[i] != nullptr) {
			delete m_debrisList[i];
			m_debrisList[i] = nullptr;
		}
	}
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroidList[i] != nullptr) {
			delete m_asteroidList[i];
			m_asteroidList[i] = nullptr;
		}
	}
	for (int i = 0; i < MAX_BEETLE; i++) {
		if (m_beetleList[i] != nullptr) {
			delete m_beetleList[i];
			m_beetleList[i] = nullptr;
		}
	}
	for (int i = 0; i < MAX_WASP; i++) {
		if (m_waspList[i] != nullptr) {
			delete m_waspList[i];
			m_waspList[i] = nullptr;
		}
	}
}
void Game::ExitAttractMode() {
	g_theAudio->StopSound(m_VictoryPlayBack);
	g_theAudio->StopSound(m_DefeatPlayBack);
	m_attractMode = false;
	g_player = new PlayerShip(this, Vec2(WORLD_CENTER_X, WORLD_CENTER_Y), 0);

	m_BGMPlayBack = g_theAudio->StartSound(m_BGM, true, m_volume);
	g_theAudio->StartSound(m_Start,false,m_volume);
}
void Game::HandleKeyPress() {

	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		//Spawn bullet

		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());

		if (controller.IsConnected() && controller.WasButtonReleased(XBOX_BUTTON_A)) {
			Vec2 head(1, 0);
			head.RotateDegrees(g_player->m_orientationDegrees);
			SpawnBullet(g_player->m_position + head, g_player->m_orientationDegrees);
		}
		else if (g_theInput->IsKeyJustDown(' ')) {
			int bulletNum = g_player->m_multiShoot;
			if (g_player->m_shootPrep > g_player->m_shootDelay) {
				float initialAngle = g_player->m_orientationDegrees - (bulletNum - 1) * 5;
				float currentAngle = initialAngle;
				for (int i = 0; i < bulletNum; i++) {
					Vec2 head(1, 0);
					head.RotateDegrees(g_player->m_orientationDegrees);
					SpawnBullet(g_player->m_position + head, currentAngle);
					currentAngle += 10;
				}
				g_player->m_shootPrep = 0;

			}

		}
		if (controller.IsConnected() && controller.WasButtonReleased(XBOX_BUTTON_B)) {
			SpawnAsteroid();
		}
		else if (g_theInput->WasKeyJustReleased('I')) {
			SpawnAsteroid();
		}
		if (controller.IsConnected() && controller.WasButtonReleased(XBOX_BUTTON_START)) {
			if (g_player->m_isDead) {
				if (m_live > 0) {
					Respawn();
				}

			}
		}
		else if (g_theInput->WasKeyJustReleased('N')) {
			if (g_player->m_isDead) {
				if (m_live > 0) {
					Respawn();
				}

			}
		}
		if (controller.IsConnected() && controller.WasButtonReleased(XBOX_BUTTON_RSTICK)) {
			m_IsDebugging = !m_IsDebugging;
		}
		else if (g_theInput->WasKeyJustReleased(0x70)) {
			m_IsDebugging = !m_IsDebugging;
		}
		if (controller.IsConnected() && controller.WasButtonReleased(XBOX_BUTTON_RIGHT)) {
			Cheat();
		}
		else if (g_theInput->WasKeyJustReleased('K')) {
			Cheat();
		}
	}
}
void Game::Respawn() {
	m_live--;
	g_theAudio->StartSound(m_Respawn, false, m_volume);
	g_player->m_position = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
	g_player->m_isDead = false;
	g_player->m_angularVelocity = 0;
	g_player->m_orientationDegrees = 0;
	g_player->m_verlocity = Vec2(0, 0);
	g_player->m_health = (int)g_player->m_maxHP;
	m_trauma = 0;
}
void Game::AdjustSound(){
	if (g_theInput->WasKeyJustReleased('M')) {
		if (!m_mute) {
			m_storedVolume = m_volume;
			m_volume = 0;
			m_mute = true;
		}
		else {
			m_volume = m_storedVolume;
			m_mute = false;

		}
		g_theAudio->SetSoundPlaybackVolume(m_VictoryPlayBack, m_volume);
		g_theAudio->SetSoundPlaybackVolume(m_DefeatPlayBack, m_volume);
		g_theAudio->SetSoundPlaybackVolume(m_BGMPlayBack, m_volume);
	}
	if (g_theInput->WasKeyJustReleased('2')) {
		if (!m_mute) {
			m_volume += 0.1f;
		}
		else {
			m_volume = m_storedVolume + 0.1f;
			m_mute = false;
		}
		g_theAudio->SetSoundPlaybackVolume(m_VictoryPlayBack, m_volume);
		g_theAudio->SetSoundPlaybackVolume(m_DefeatPlayBack, m_volume);
		g_theAudio->SetSoundPlaybackVolume(m_BGMPlayBack, m_volume);
	}
	if (g_theInput->WasKeyJustReleased('1')) {
		if (!m_mute) {
			m_volume -= 0.1f;
		}
		else {
			m_volume = m_storedVolume - 0.1f;
			m_mute = false;
		}
		g_theAudio->SetSoundPlaybackVolume(m_VictoryPlayBack, m_volume);
		g_theAudio->SetSoundPlaybackVolume(m_DefeatPlayBack, m_volume);
		g_theAudio->SetSoundPlaybackVolume(m_BGMPlayBack, m_volume);
	}
}
void Game::renderHPBar(Vec2 const& position, float scale, float maxHP, float HP) {
	Vec2 maxHPEND = Vec2(position.x+scale*maxHP,position.y);
	Vec2 HPEND = Vec2(position.x+scale*HP,position.y);
	DrawHP(position, maxHPEND, scale * 5, Rgba8(255, 0, 0));
	DrawHP(position, HPEND, scale * 5, Rgba8(0, 255, 0));
}
void Game::loadSound() {
	m_BGM = g_theAudio->CreateOrGetSound("Audio/Battle.mp3");
	m_Victory = g_theAudio->CreateOrGetSound("Audio/Vicory.mp3");
	m_Defeat = g_theAudio->CreateOrGetSound("Audio/Defeat.mp3");
	m_Death = g_theAudio->CreateOrGetSound("Audio/Explosion.mp3");
	m_EnemyDeath = g_theAudio->CreateOrGetSound("Audio/Explosion2.mp3");
	m_WaveStart = g_theAudio->CreateOrGetSound("Audio/WaveStart.mp3");
	m_Respawn = g_theAudio->CreateOrGetSound("Audio/Respawn.mp3");
	m_Start = g_theAudio->CreateOrGetSound("Audio/Start.mp3");
	m_Quit = g_theAudio->CreateOrGetSound("Audio/Quit.mp3");
	m_Shoot = g_theAudio->CreateOrGetSound("Audio/Shoot.mp3");
	m_Hurt1 = g_theAudio->CreateOrGetSound("Audio/hurt grunt.wav");
	m_Hurt2 = g_theAudio->CreateOrGetSound("Audio/hurt grunt 1.wav");
	m_Hurt3 = g_theAudio->CreateOrGetSound("Audio/hurt grunt 2.wav");
}
void Game::trySpawnEnemy(){
	if (m_beetleRemain > 0) {
		SpawnBeetle();
	}
	if (m_waspRemain > 0) {
		SpawnWasp();
	}
}
void Game::PeriodicalAsteroid(float deltasecond) {
	m_asteroidTimerRemain-=(20+m_wave)*deltasecond;
	if (m_asteroidTimerRemain <= 0) {
		SpawnAsteroid();
		m_asteroidTimerRemain = m_asteroidSpawnTimer;
	}
}

Game::~Game() {
	if (m_BGMPlayBack != MISSING_SOUND_ID) {
		g_theAudio->StopSound(m_BGMPlayBack);
	}
	if (m_VictoryPlayBack != MISSING_SOUND_ID) {
		g_theAudio->StopSound(m_VictoryPlayBack);
	}
	if (m_DefeatPlayBack != MISSING_SOUND_ID) {
		g_theAudio->StopSound(m_DefeatPlayBack);
	}
	delete g_player;
	delete g_RNG;
	for (int i = 0; i < MAX_BEETLE;i++) {
		delete m_beetleList[i];
	}
	for (int i = 0; i < MAX_WASP; i++) {
		delete m_waspList[i];
	}
}