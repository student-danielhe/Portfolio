#include "Player.hpp"
#include "Game.hpp"
#include "Engine/Math/MathUtils.hpp"

constexpr float HUDSizeX = 2000.f;
float HUDSizeY = 2000.f;
constexpr float HUDHeight = .125f;
void PlayerController::UpdateInput(float deltaSeconds) {
	Vec3 i;Vec3 j; Vec3 k;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp(i, j, k);
	g_theAudio->UpdateListener(m_playerIndex, m_position, i, k);
	m_angularVelocity.m_pitchDegrees = 0.f;
	m_angularVelocity.m_rollDegrees = 0.f;
	m_angularVelocity.m_yawDegrees = 0.f;
	m_velocity = Vec3(0.f, 0.f, 0.f);
	float speed = 1.f;
	float RunSpeed = 15.f;
	if (g_theInput->GetCursorMode()==CursorMode::FPS) {
		//rotation------------------------------------------
		Vec2 mouseDelta = g_theInput->GetCursorClientDelta();
		//key press check
		
		float valX = 0;
		float valY = 0;
		if (m_playerIndex == 0&&!m_aimMeat) {
			valX = mouseDelta.x;
			valY = mouseDelta.y;
		}

		float finalValX = 5 * valX;
		if (m_mode == CameraMode::POSSESS) {
			if (m_map->GetActorByHandle(*m_ownerHandle) != nullptr) {
				Actor* possess = m_map->GetActorByHandle(*m_ownerHandle);
				finalValX = fmaxf(finalValX, possess->m_definition->m_physics->m_turnSpeed);
			}
		}
		else {
			finalValX = fminf(finalValX, 180.f);
		}
		m_orientation.m_yawDegrees -= 5* valX * deltaSeconds;
		m_orientation.m_pitchDegrees += 5 * valY * deltaSeconds;
		m_orientation.m_pitchDegrees = fmaxf(m_orientation.m_pitchDegrees, -85.f);
		m_orientation.m_pitchDegrees = fminf(m_orientation.m_pitchDegrees, 85.f);

		
		//movement-----------------------------------
		if (g_theInput->WasKeyJustPressed('W') && m_playerIndex == 0) {
			m_velocity.x = speed;
		}
		else if (g_theInput->WasKeyJustPressed('S') && m_playerIndex == 0) {
			m_velocity.x = -speed;
		}

		if (g_theInput->WasKeyJustPressed('A') && m_playerIndex == 0) {
			m_velocity.y = speed;
		}
		else if (g_theInput->WasKeyJustPressed('D') && m_playerIndex == 0) {
			m_velocity.y = -speed;
		}


	
		UpdateWeapon();
		

		if (m_mode == CameraMode::POSSESS) {
			Actor* actor = m_map->GetActorByHandle(*m_ownerHandle);
			if (actor) {
				actor->m_running = g_theInput->WasKeyJustPressed(KEYCODE_SHIFT)&&!actor->m_blocking;
				if (actor->m_running) {
					m_velocity *= RunSpeed / speed;
				}
			}
		}
		

		m_velocity = m_orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(m_velocity);
		
		m_position += m_velocity * deltaSeconds;


	}
	if (g_theInput->WasKeyJustPressed('Q')) {
		m_thirdPersonDist -= deltaSeconds;
		if (m_thirdPersonDist < 0.f) {
			m_thirdPersonDist = 0.f;
		}
	}
	else if (g_theInput->WasKeyJustPressed('E')) {
		m_thirdPersonDist += deltaSeconds;
		if (m_thirdPersonDist > 3.f) {
			m_thirdPersonDist = 3.f;
		}
	}
	if (m_mode == CameraMode::POSSESS) {
		Actor* possess = m_map->GetActorByHandle(*m_ownerHandle);
		if (possess) {
			if (m_velocity.GetLengthSquared() > 0.f) {
				possess->MoveInDirection(m_velocity.GetNormalized(), m_velocity.GetLength());
			}
			possess->TurnInDirection(m_orientation.m_yawDegrees, possess->m_definition->m_physics->m_turnSpeed * deltaSeconds);
		}
		else {
			m_map->SpawnPlayer();
			if (m_map->m_splitScreen) {
				if (m_playerIndex == 0) {
					PlayerPossess(m_map->m_player1Handle);
				}
				else {
					PlayerPossess(m_map->m_player2Handle);
				}
			}
			else {
				PlayerPossess(m_map->m_player1Handle);
			}
		}
	}
}

void PlayerController::UpdateWeapon() {
	if (m_aimMeat) {
		Vec2 mouseDelta = g_theInput->GetCursorClientDelta();
		//key press check
		if(m_cursorDeltasOver20Frames.size()==20)
		     m_cursorDeltasOver20Frames.erase(m_cursorDeltasOver20Frames.begin());
		m_cursorDeltasOver20Frames.push_back(new Vec2());
		for (Vec2* vec : m_cursorDeltasOver20Frames) {
			*vec =*vec+ mouseDelta;
		}
	}
	if (m_map->GetActorByHandle(*m_ownerHandle) != nullptr) {
		Actor* possess = m_map->GetActorByHandle(*m_ownerHandle);
		if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
			if (m_aimMeat) {
				float valX = m_cursorDeltasOver20Frames[0]->x;
				float valY = m_cursorDeltasOver20Frames[0]->y;
				if (valY < -10.f) {
					m_ammo--;
					Vec3 velocity;
      				velocity.y = -valX * .001f;
					velocity.x = -valY * .001f;
					velocity.z = -valY * .001f;
					Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
					velocity = rotation.TransformVectorQuantity3D(velocity);
					ActorHandle* handle =m_map->SpawnNewActor("Meat", m_position+possess->GetForwardNormal()*.5f, velocity);
					m_map->GetActorByHandle(*handle)->m_owner = m_ownerHandle;
				}
				m_aimMeat = false;
			}
		}
		if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
			if (possess->m_weapons[possess->m_currentWeaponIndex]->m_definition->m_name.compare("Meat") == 0&&!g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
				if (m_ammo > 0) {
					m_aimMeat = true;
					m_thirdPersonDist = 0.f;
				}
			}
			else {
				possess->m_weapons[possess->m_currentWeaponIndex]->Fire();
			}
			possess->m_blocking = false;
		}
		else {
			if (possess->m_weapons[possess->m_currentWeaponIndex]->m_definition->m_name.compare("Meat") == 0) {
				if (g_theInput->WasKeyJustReleased(KEYCODE_RIGHT_MOUSE)) {
					if (m_ammo > 0) {
						m_ammo--;
						possess->m_currentHealth = possess->m_definition->m_base->m_health;
						SoundID heal = g_theAudio->CreateOrGetSound("Data/Gold/Heal.ogg");
						g_theAudio->StartSound(heal);
						m_map->SpawnNewActor("Heal", possess->m_bottomPosition + possess->GetForwardNormal() * .2f);
					}
				}
			}
			else if (possess->m_weapons[possess->m_currentWeaponIndex]->m_definition->m_name.compare("DemonMelee") == 0) {
				possess->m_blocking = g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE);
			}
		}

		//Weapon swaping--------------------------------------------------------------------
		if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT)) {
			possess->m_currentWeaponIndex--;
			if (possess->m_currentWeaponIndex < 0) {
				possess->m_currentWeaponIndex = (int)possess->m_weapons.size() - 1;
			}
		}

		if (g_theInput->WasKeyJustReleased(KEYCODE_RIGHT)) {
			possess->m_currentWeaponIndex++;
			if (possess->m_currentWeaponIndex >= possess->m_weapons.size()) {
				possess->m_currentWeaponIndex = 0;
			}
		}
		if (g_theInput->WasKeyJustReleased('1')) {
			possess->EquipWeapon(0);
		}
		if (g_theInput->WasKeyJustReleased('2')) {
			possess->EquipWeapon(1);
		}
	}
}

PlayerController::PlayerController(ActorHandle* handle, Map* map)
	:Controller(handle,map){
	m_position = Vec3(0.f, 0.f, 2.f);
	m_cursorDeltasOver20Frames.reserve(20 * sizeof(Vec2*));
	m_FPSTimer = new Timer(3.f);
}

void PlayerController::InitializeCameras() {

	m_worldCamera = new Camera();
	m_HUDCamera = new Camera();
	m_HUDCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(HUDSizeX, HUDSizeY));
	
}

Vec3 PlayerController::GetFwdVector() {
	return m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D();
}
Vec3 PlayerController::GetOrientation() {
	return Vec3(
	    m_orientation.m_yawDegrees,
		m_orientation.m_pitchDegrees,
		m_orientation.m_rollDegrees);
}
void PlayerController::UpdateCamera() {

	//apply movement
	if (m_mode == CameraMode::POSSESS) {
		if (m_ownerHandle&&m_map->GetActorByHandle(*m_ownerHandle) != nullptr) {
				Actor* possess = m_map->GetActorByHandle(*m_ownerHandle);
				m_position = possess->m_bottomPosition;
				m_position.z += possess->m_definition->m_camera->m_eyeHeight;
				m_position+=Vec3(-m_thirdPersonDist, 0.f, 0.2f).GetRotatedAboutZDegrees(possess->m_orientation.m_yawDegrees);
				m_orientation.m_yawDegrees = possess->m_orientation.m_yawDegrees;
				m_worldCamera->SetPerspectiveView(m_aspect, possess->m_definition->m_camera->m_cameraFOVDeg, .1f, 100.f);
		}
	}
	else {
		m_worldCamera->SetPerspectiveView(m_aspect, 60.f, .1f, 100.f);
	}
	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);
	m_worldCamera->SetPosition(m_position);
	m_worldCamera->SetOrientation(m_orientation);
	Mat44 CamToRender(Vec4(0.f, -1.f, 0.f, 0.f),
		Vec4(0.f, 0.f, 1.f, 0.f),
		Vec4(1.f, 0.f, 0.f, 0.f),
		Vec4(0.f, 0.f, 0.f, 1.f));
	CamToRender.Transpose();
	m_worldCamera->SetCameraToRenderTransform(CamToRender);
}

void PlayerController::PlayerPossess(ActorHandle* newActor) {
	Possess(newActor);
	Actor* actor = m_map->GetActorByHandle(*newActor);
	m_position.x = actor->m_bottomPosition.x;
	m_position.y = actor->m_bottomPosition.y;
	m_orientation = actor->m_orientation;
	m_mode = CameraMode::POSSESS;
}

void PlayerController::RenderUI(Weapon* weapon) {

	g_theRenderer->BeginCamera(*m_HUDCamera);
	if (weapon->m_timer->HasPeriodElapsed()) {
		weapon->m_currentAction = "Idle";
	}
	if (weapon->m_definition->m_HUD->m_shaderPath.compare("Default") == 0) {
		g_theRenderer->BindShader(g_theRenderer->m_defaultShader);
	}
	else {
		g_theRenderer->BindShader(g_theRenderer->m_diffuseShader);
	}

	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	RenderWeapon(weapon);
	RenderHUD(weapon);
	RenderStats(weapon);
	g_theRenderer->EndCamera(*m_HUDCamera);

}
void PlayerController::RenderHUD(Weapon* weapon) {
	//HUD
	std::vector<Vertex_PCU> verts;
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile(weapon->m_definition->m_HUD->m_TexturePath.data());
	g_theRenderer->BindTexture(texture);
	AddVertsForRect(verts, Vec2(HUDSizeX, 0.f), 
		                   Vec2(HUDSizeX, HUDSizeY*HUDHeight), 
		                   Vec2(0.f,      HUDSizeY*HUDHeight), 
		                   Vec2(0.f,      0.f), Rgba8::WHITE, AABB2::DEFAULT);
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();
	//reticle
	if (!m_map->GetActorByHandle(*m_ownerHandle)->m_dead) {
		g_theRenderer->BindTexture(g_theRenderer->CreateOrGetTextureFromFile(weapon->m_definition->m_HUD->m_reticleTexturePath.data()));
		Vec2 rectSize = weapon->m_definition->m_HUD->m_reticleSize;
		AddVertsForRect(verts, Vec2(HUDSizeX * .5f + rectSize.x*.5f, HUDSizeY * .5f - rectSize.y*.5f), 
			                   Vec2(HUDSizeX * .5f + rectSize.x*.5f, HUDSizeY * .5f + rectSize.y*.5f),
			                   Vec2(HUDSizeX * .5f - rectSize.x*.5f, HUDSizeY * .5f + rectSize.y*.5f), 
			                   Vec2(HUDSizeX * .5f - rectSize.x*.5f, HUDSizeY * .5f - rectSize.y*.5f), Rgba8::WHITE, AABB2::DEFAULT);
		g_theRenderer->DrawVertexArray(verts);
	}
}
void PlayerController::RenderWeapon(Weapon* weapon) {
	//no weapon rendering for gold
	if (!m_map->m_gold) {
		if (!m_map->GetActorByHandle(*m_ownerHandle)->m_dead) {
			std::string spritePath = "";
			auto result = weapon->m_animationMap.find(weapon->m_currentAction);
			if (result != weapon->m_animationMap.end()) {
				SpriteAnimDefinition* def = result->second;
				g_theRenderer->BindTexture(&def->m_spriteSheet.GetTexture());
				AABB2 UV = def->GetSpriteDefAtTime((float)weapon->m_animClock->GetTotalSeconds()).GetUV();
				std::vector<Vertex_PCU> verts;
				Vec2 pivot = weapon->m_definition->m_HUD->m_spritePivot;
				Vec2 size = weapon->m_definition->m_HUD->m_spriteSize;
				AddVertsForRect(verts, Vec2(HUDSizeX * pivot.x + size.x * .5f, HUDSizeY * (pivot.y + HUDHeight)),
					Vec2(HUDSizeX * pivot.x + size.x * .5f, HUDSizeY * (pivot.y + HUDHeight) + size.y * 2.f),
					Vec2(HUDSizeX * pivot.x - size.x * .5f, HUDSizeY * (pivot.y + HUDHeight) + size.y * 2.f),
					Vec2(HUDSizeX * pivot.x - size.x * .5f, HUDSizeY * (pivot.y + HUDHeight)), Rgba8::WHITE, UV);
				g_theRenderer->DrawVertexArray(verts);
			}
		}
	}
	
}
void PlayerController::RenderStats(Weapon* weapon) {
	float cellHeight = HUDSizeY*HUDHeight/3.f;
	float boxLow = HUDSizeY * HUDHeight * .2f;
	float boxHight = HUDSizeY * HUDHeight* 1.f;
	AABB2 HealthSquare = AABB2(Vec2(0.f* HUDSizeX, boxLow), Vec2( .6f*HUDSizeX, .45f*boxHight));
	AABB2 AmmoSquare  = AABB2(Vec2(0.f * HUDSizeX, boxLow), Vec2(.35f * HUDSizeX, .45f * boxHight));
	AABB2 WeaponSquare = AABB2(Vec2(0.46f * HUDSizeX, 0.f), Vec2(.54f * HUDSizeX, .12f*HUDSizeX));
	Actor* owner = m_map->GetActorByHandle(*m_ownerHandle);
	if (owner) {
		std::string health = std::to_string(owner->m_currentHealth);


		BitmapFont* squirrel = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
		g_theRenderer->BindTexture(&squirrel->GetTexture());
		std::vector<Vertex_PCU> verts;
		squirrel->AddVertsForTextInBox2D(verts, health,HealthSquare, cellHeight);
		squirrel->AddVertsForTextInBox2D(verts, std::to_string(m_ammo), AmmoSquare, cellHeight);
		//squirrel->AddVertsForTextInBox2D(verts, kill, KillSquare, cellHeight);
		//squirrel->AddVertsForTextInBox2D(verts, death, DeathSquare, cellHeight);
		g_theRenderer->DrawVertexArray(verts);

		verts.clear();
		if (m_map->m_gold) {
			if (!m_map->GetActorByHandle(*m_ownerHandle)->m_dead) {
				std::string spritePath = "";
				auto result = weapon->m_animationMap.find(weapon->m_currentAction);
				if (result != weapon->m_animationMap.end()) {
					
					SpriteAnimDefinition* def = result->second;
					g_theRenderer->BindTexture(&def->m_spriteSheet.GetTexture());
				}
				Rgba8 color = Rgba8::WHITE;
				if (m_ammo == 0 && weapon->m_definition->m_name.compare("Meat") == 0) {
					color = Rgba8(100, 100, 100);
				}
				AddVertsForRect(verts, Vec2(WeaponSquare.m_maxs.x,WeaponSquare.m_mins.y), WeaponSquare.m_maxs, 
					                   Vec2(WeaponSquare.m_mins.x,WeaponSquare.m_maxs.y), WeaponSquare.m_mins, color, AABB2::DEFAULT);
				g_theRenderer->DrawVertexArray(verts);
			}
		}
	}
	
}