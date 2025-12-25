#include "Actor.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Map.hpp"
#include "AIController.hpp"
#include "MarineAI.hpp"
Actor::Actor(ActorDefinition* def, ActorHandle* handle)
:m_definition(def)
,m_handle(handle){
	m_animClock = new Clock(*g_gameClock);
	//load all sound and picture
	m_deathSound = g_theAudio->CreateOrGetSound(GetSoundPath("Death"), true);
	m_hurtSound = g_theAudio->CreateOrGetSound(GetSoundPath("Hurt"), true);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile(m_definition->m_visual->m_spriteSheetPath.data());
	Vec2 cellCount = m_definition->m_visual->m_cellCount;
	m_sprites = new SpriteSheet(*texture,IntVec2((int)cellCount.x,(int)cellCount.y));
	for (std::string weaponName : m_definition->m_weapon->m_name) {
		Weapon* weapon = new Weapon(WeaponDefinition::GetWeaponDefinitionByName(weaponName), m_handle);
		m_weapons.push_back(weapon);
	}
	m_currentHealth = m_definition->m_base->m_health;
	if (m_definition->m_ai->m_aiEnabled) {
		//add controller
	}
	if (m_definition->m_base->m_visible) {
		AddVertsForZCylinder3D(m_vertexes, m_bottomPosition, 
			m_definition->m_collision->m_height, 
			m_definition->m_collision->m_radius);
	}
	// set color
	if (m_definition->m_base->m_faction == Faction_Actor::DEMON) {
		m_color = Rgba8::RED;
	}
	if (m_definition->m_base->m_faction == Faction_Actor::MARINE) {
		m_color = Rgba8::GREEN;
	}
	if (m_definition->m_base->m_faction == Faction_Actor::NEUTRAL) {
		m_color = Rgba8::BLUE;
	}
	if (m_definition->m_collision->m_dieOnCollide) {
		m_color = Rgba8::BLUE;
	}
	InitializeAnimGroup();
	m_dead = m_definition->m_base->m_dieOnSpawn;
	if (m_dead) {
		TryChangeAction("Death");
	}
}
void  Actor::UpdatePhysics(float deltaSeconds) {
	UpdateAnimClock();
	if (m_definition->m_physics->m_simulated&&!m_dead) {
		if (!m_definition->m_physics->m_flying) {
			m_bottomPosition.z = 0.f;
		}
		AddForce((m_definition->m_physics->m_drag/2.f) * -m_velocity.GetNormalized());
		if (!m_dead) {
			m_velocity += m_acceleration*deltaSeconds;
			if (m_definition->m_base->m_name.compare("Meat") == 0&&m_bottomPosition.z>.1f) {
				m_velocity.z -= 1.f * deltaSeconds;
				
			}
			if (m_definition->m_base->m_name.compare("Meat") == 0 && m_bottomPosition.z <= .2f) {
				m_velocity = Vec3();
			}
			//clamp speed
			if (m_definition->m_physics->m_walkSpeed != 0.f) {
				if (!m_running) {
					if (!m_blocking) {
						if (m_velocity.GetLengthSquared() > m_definition->m_physics->m_walkSpeed * m_definition->m_physics->m_walkSpeed) {
							m_velocity = m_velocity.GetNormalized() * m_definition->m_physics->m_walkSpeed;
						}
					}
					else {
						if (m_velocity.GetLengthSquared() > m_definition->m_physics->m_walkSpeed * m_definition->m_physics->m_walkSpeed*.25f) {
							m_velocity = m_velocity.GetNormalized() * m_definition->m_physics->m_walkSpeed*.5f;
						}
					}
					
				}
				else {
					if (m_velocity.GetLengthSquared() > m_definition->m_physics->m_runSpeed * m_definition->m_physics->m_runSpeed) {
						m_velocity = m_velocity.GetNormalized() * m_definition->m_physics->m_runSpeed;
					}
				}
			}
			m_bottomPosition += m_velocity * deltaSeconds;
		}
		m_acceleration = Vec3();
	}
	if(m_dead&& m_deathTimer == nullptr) {
		m_deathTimer = new Timer(m_definition->m_base->m_corpseLifeTime, g_gameClock);
		m_deathTimer->Start();
	}
	if (m_deathTimer != nullptr&&m_deathTimer->HasPeriodElapsed()) {
		m_destroyed = true;
	}
}

void Actor::Damage(int damageNumber) {
	if (m_blocking) {
		damageNumber = RoundDownToInt((float)damageNumber*.2f);
	}
	m_currentHealth -= damageNumber;
	if (m_currentHealth <= 0) {
		m_dead = true;
		TryChangeAction("Death");
		if(m_deathSound!=MISSING_SOUND_ID)
		    g_theAudio->StartSoundAt(m_deathSound, m_bottomPosition);
		//Marine drop meat on death
		if (m_definition->m_base->m_name.compare("Marine") == 0) {
			m_map->SpawnNewActor("Meat", m_bottomPosition);
		}
	}
	else {
		TryChangeAction("Hurt");
		if(m_hurtSound!=MISSING_SOUND_ID)
		    g_theAudio->StartSoundAt(m_hurtSound, m_bottomPosition);
	}
		
}
void Actor::Damage(FloatRange damageNumber) {
	Damage(g_RNG->RollRandomIntInRange((int)damageNumber.m_min, (int)damageNumber.m_max));
}
void Actor::AddForce(Vec3 force) {
	m_acceleration += force;
}
void Actor::AddImpulse(Vec3 impulse) {
	m_velocity += impulse;
}
void Actor::OnCollide(Actor& other) {
	if (!m_dead) {
		if (m_definition->m_base->m_name.compare("Meat") == 0&&m_owner!=other.m_handle) {
			if (other.m_definition->m_base->m_faction == Faction_Actor::DEMON) {
				//in case of player, increase meat ammo
				if (g_player1Controller->m_ownerHandle==other.m_handle) {
					g_player1Controller->m_ammo++;
					m_meat = g_theAudio->CreateOrGetSound("Data/Gold/Fresh Meat.mp3");
					if(!g_theAudio->IsPlaying(m_meatPlayback))
					    m_meatPlayback = g_theAudio->StartSound(m_meat);
					m_dead = true;
				}
				else {
					if (!other.m_AIController->m_inParty) {
						DemonAIController::g_party.push_back(other.m_handle);
						m_dead = true;
						other.m_AIController->m_inParty = true;
						Vec3 animPos = other.m_bottomPosition;
						animPos.z+=other.m_definition->m_camera->m_eyeHeight;
						m_map->SpawnNewActor("Heart", animPos);
					}
				}
			}
		}
		if (other.m_handle != m_owner) {
			//remove marine collateral damage
			if (m_definition->m_base->m_name.compare("PlasmaProjectile") == 0 && other.m_definition->m_base->m_name.compare("Demon") == 0) {
				Vec3 direction = other.m_bottomPosition - m_bottomPosition.GetNormalized();
				other.AddImpulse(direction * m_definition->m_collision->m_impulseOnCollide);

				int damageNum = g_RNG->RollRandomIntInRange((int)m_definition->m_collision->m_damageOnCollide.m_min, (int)m_definition->m_collision->m_damageOnCollide.m_max);
				other.Damage(damageNum);


				if (other.m_AIController && other.m_controller == other.m_AIController && m_owner) {
					other.m_AIController->DamagedBy(m_owner);
					if (other.m_controller == g_player1Controller) {
						DemonAIController::GlobalAggro(m_owner);
					}
				}
			}
			
		}
		if (m_definition->m_collision->m_dieOnCollide) {
			m_dead = true;
			TryChangeAction("Death");
		}
	}
}
void Actor::OnPossessed(Controller* newController) {
	m_controller = newController;
	if(m_AIController)
	    m_AIController->m_ownerHandle = nullptr;
	m_controller->m_ownerHandle = m_handle;
}
void Actor::OnUnpossessed() {
	if(m_controller)
	   m_controller->m_ownerHandle = nullptr;
	m_controller = m_AIController;
	if (m_controller != nullptr) {
		m_controller->m_ownerHandle = m_handle;
	}
}
void Actor::MoveInDirection(Vec3 direction, float speed) {
	if (!m_dead) {
		AddForce(direction * (speed + m_definition->m_physics->m_drag));
	}
}
void Actor::TurnInDirection(float degree, float angularSpeed) {
	if (!m_dead) {
		float angle = GetTurnedTowardDegrees(m_orientation.m_yawDegrees, degree, angularSpeed);
		m_orientation.m_yawDegrees = angle;
	}
}
void Actor::Attack() {
	if(!m_dead)
	m_weapons[m_currentWeaponIndex]->Fire();
}
void Actor::EquipWeapon(int weaponIndex) {
	m_currentWeaponIndex = weaponIndex;
	if (m_currentWeaponIndex > m_weapons.size()) {
		m_currentWeaponIndex = m_currentWeaponIndex % m_weapons.size();
	}
}

void  Actor::Render(PlayerController* player) {
	if (m_definition->m_base->m_visible) {
		if (!(player->GetActor() == this && player->m_mode == CameraMode::POSSESS&& player->m_thirdPersonDist<=.2f)) {
			if (m_definition->m_visual->m_shaderPath.compare("Default")==0) {
				g_theRenderer->BindShader(g_theRenderer->m_defaultShader);
			}
			else {
				g_theRenderer->BindShader(g_theRenderer->m_diffuseShader);
			}
			g_theRenderer->BindTexture(&m_sprites->GetTexture());
			g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
			//always render hit effect
			if (m_definition->m_base->m_dieOnSpawn) {
				g_theRenderer->SetDepthMode(DepthMode::DISABLED);
			}
			else {
				g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
			}
			//Mat44 transform = GetBillboardTransform(m_definition->m_visual->m_billboardType, player->m_worldCamera->GetCameraToWorldTransform(), m_bottomPosition);
			g_theRenderer->SetModelConstants(GetBillboardMatrix(player), Rgba8::WHITE);
			g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
			//m_spriteAnim->UpdateStartIndex(dir->m_animation->m_startFrame);

			std::vector<Vertex_PCU> verts;
			if (GetCurrentDirection(player)) {
				//restore to walk
				SpriteAnimDefinition* current = GetCurrentDirection(player);
				int frameNum = (int)((float)m_animClock->GetTotalSeconds() * current->m_framesPerSecond);
				int animLength = current->m_endSpriteIndex - current->m_startSpriteIndex;
				if (current->m_playbackType == SpriteAnimPlaybackType::ONCE) {
					if (frameNum > animLength) {
						TryChangeAction("Walk");
					}
				}
				//-----------------------------------------------------
				AABB2 UV = GetCurrentDirection(player)->GetSpriteDefAtTime((float)m_animClock->GetTotalSeconds()).GetUV();
				Vec2 size = m_definition->m_visual->m_size;
				Vec2 pivot = m_definition->m_visual->m_pivot;
				AddVertsForQuad3D(verts, Vec3(0.f, (-pivot.x) * size.x, -pivot.y * size.y), Vec3(0.f, (1.f - pivot.x) * size.x, -pivot.y * size.y),
					Vec3(0.f, (1.f - pivot.x) * size.x, (1.f - pivot.y) * size.y), Vec3(0.f, -pivot.x * size.x, (1.f - pivot.y) * size.y), Rgba8::WHITE, UV);
				g_theRenderer->DrawVertexArray(verts);
			}

			if (m_definition->m_base->m_name.compare("Demon") == 0 || m_definition->m_base->m_name.compare("Marine") == 0) {
				RenderHealthBar(player);
			}
		}
	}
}

void Actor::RenderHealthBar(PlayerController* player) {
	if (!m_dead) {
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->BindShader(g_theRenderer->m_defaultShader);
		g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
		g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		//Render green health
		g_theRenderer->SetModelConstants(GetBillboardMatrix(player), Rgba8::GREEN);
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
		AABB2 HPBounds = AABB2(Vec2(-.3f, m_definition->m_camera->m_eyeHeight+.2f), Vec2(.3f, m_definition->m_camera->m_eyeHeight + .25f));
		float percentile = (float)m_currentHealth / (float)m_definition->m_base->m_health;
		std::vector<Vertex_PCU> verts;
		float currentHP = HPBounds.m_maxs.x + (HPBounds.m_mins.x - HPBounds.m_maxs.x) * percentile;
		AddVertsForQuad3D(verts, Vec3(0.f, currentHP, HPBounds.m_mins.y),
			                     Vec3(0.f, HPBounds.m_maxs.x, HPBounds.m_mins.y),
			                     Vec3(0.f, HPBounds.m_maxs.x , HPBounds.m_maxs.y),
			                     Vec3(0.f, currentHP, HPBounds.m_maxs.y), Rgba8::GREEN, AABB2::DEFAULT);
		g_theRenderer->DrawVertexArray(verts);

		verts.clear();

		//Render red health

		g_theRenderer->SetModelConstants(GetBillboardMatrix(player), Rgba8::RED);

		AddVertsForQuad3D(verts, Vec3(0.f, HPBounds.m_mins.x, HPBounds.m_mins.y),
			Vec3(0.f, currentHP, HPBounds.m_mins.y),
			Vec3(0.f, currentHP, HPBounds.m_maxs.y),
			Vec3(0.f, HPBounds.m_mins.x, HPBounds.m_maxs.y), Rgba8::RED, AABB2::DEFAULT);
		g_theRenderer->DrawVertexArray(verts);
		
		if (m_blocking) {
			verts.clear();
			g_theRenderer->SetModelConstants(GetBillboardMatrix(player), Rgba8::WHITE);
			Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Gold/Shield.png");
			g_theRenderer->BindTexture(texture);
			AddVertsForQuad3D(verts, Vec3(0.f, HPBounds.m_maxs.x, HPBounds.m_mins.y - .05f),
				                     Vec3(0.f, HPBounds.m_maxs.x + .15f, HPBounds.m_mins.y-.05f),
				                     Vec3(0.f, HPBounds.m_maxs.x + .15f, HPBounds.m_maxs.y+.05f),
				                     Vec3(0.f, HPBounds.m_maxs.x, HPBounds.m_maxs.y+.05f), Rgba8::WHITE, AABB2::DEFAULT);
			g_theRenderer->DrawVertexArray(verts);
		}
	}


}

Mat44 Actor::GetModelMatrix() {
	Mat44 modelToWorld = Mat44::MakeTranslation3D(m_bottomPosition);
	modelToWorld.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return modelToWorld;
}
Mat44 Actor::GetBillboardMatrix(PlayerController* player) {
	Mat44 modelToWorld = Mat44::MakeTranslation3D(m_bottomPosition);
	Mat44 transform = GetBillboardTransform(m_definition->m_visual->m_billboardType, player->m_worldCamera->GetCameraToWorldTransform(), m_bottomPosition);
	modelToWorld.Append(transform);
	return modelToWorld;
}
bool Actor::CanAggroOnto(ActorHandle* other) {
	Actor* enemy = m_map->GetActorByHandle(*other);
	if (enemy != nullptr) {
		if (!enemy->m_dead) {
			if (OpposingFaction(other)) {
				if (enemy->m_definition->m_base->m_visible) {
					Vec3 targetToMe =  enemy->m_bottomPosition-m_bottomPosition;

					Vec3 fwd;
					Vec3 j;
					Vec3 k;
					m_orientation.GetAsVectors_IFwd_JLeft_KUp(fwd,j,k);
					
					if (IsPointInsideDirectedSector2D(enemy->m_bottomPosition,m_bottomPosition,fwd,m_definition->m_ai->m_sightAngle,m_definition->m_ai->m_sightRadius)) {
						RaycastResult3D res = m_map->RaycastAll(m_bottomPosition, targetToMe.GetNormalized(), m_definition->m_ai->m_sightRadius,this);
						RaycastResult3D actorRes = m_map->RaycastWorldActors(m_bottomPosition, targetToMe.GetNormalized(), m_definition->m_ai->m_sightRadius, this);
						if (actorRes.m_didImpact&&res.m_impactPos == actorRes.m_impactPos) {
							return enemy;
						}
					}
					

				}
			}
		}
	}
	return false;
}
bool Actor::OpposingFaction(ActorHandle* other) {
	Actor* enemy = m_map->GetActorByHandle(*other);
	return(enemy->m_definition->m_base->m_faction == Faction_Actor::DEMON && m_definition->m_base->m_faction == Faction_Actor::MARINE)
		|| ((enemy->m_definition->m_base->m_faction == Faction_Actor::MARINE && m_definition->m_base->m_faction == Faction_Actor::DEMON));
}

SoundID Actor::GetSoundEffect(std::string name) {
	for (Sound* sound : m_definition->m_sound) {
		if(sound->m_sound.compare(name)==0)
		    return g_theAudio->CreateOrGetSound(sound->m_path);
	}
	return 0;
}

Vec3 Actor::GetCameraInActorSpace(PlayerController* player) {
	Vec3 camToDemon = m_bottomPosition-player->m_worldCamera->GetPosition();
	camToDemon.z = 0.f;
	camToDemon = camToDemon.GetNormalized();
	Mat44 worldToModel= GetModelMatrix().GetOrthonormalInverse();
	return worldToModel.TransformVectorQuantity3D(camToDemon);
}


SpriteAnimDefinition* Actor::GetCurrentDirection(PlayerController* player) {
	AnimationGroupDefinition* currentGroup = nullptr;
	for (AnimationGroupDefinition* animGroup : m_animGroups) {
		if (animGroup->m_name.compare(m_currentAction) == 0) {
			currentGroup = animGroup;
		}
	}
	if (currentGroup != nullptr) {
		SpriteAnimDefinition* currentDir = nullptr;
		Vec3 actorDir = GetCameraInActorSpace(player);
		float best = -2.f;
		for (DirAnimation* dir : currentGroup->m_directions) {
			float result = DotProduct3D(*dir->direction, actorDir);
			if ( result > best) {
				best = result;
				currentDir = dir->spriteDef;
			}
		}
		return currentDir;
	}
	return nullptr;
}
void Actor::InitializeAnimGroup() {
	for (AnimationGroup* group : m_definition->m_visual->m_animationGroup) {
		AnimationGroupDefinition* def = new AnimationGroupDefinition();
		m_animGroups.push_back(def);
		def->m_name = group->m_name;
		def->scaleBySpeed = group->m_scaleBySpeed;
		for (Direction* dir : group->m_directions) {
			DirAnimation* dirAnim = new DirAnimation();
			def->m_directions.push_back(dirAnim);
			dirAnim->direction = new Vec3();
			*dirAnim->direction = dir->m_vector.GetNormalized();
			 dirAnim->spriteDef = new SpriteAnimDefinition(*m_sprites,
				dir->m_animation->m_startFrame, dir->m_animation->m_endFrame, 1.f / group->m_secondsPerFrame, group->m_playbackMode);
		}
	}
}
void Actor::TryChangeAction(std::string actionName) {
	for (AnimationGroupDefinition* def : m_animGroups) {
		if (def->m_name.compare(actionName) == 0) {
			m_currentAction = actionName;
			m_animClock->Reset();
		}
	}
}
Actor::~Actor() {
	m_map->m_actors[m_handle->GetIndex()]=nullptr;
	if (m_handle) {
		delete m_handle;
		m_handle = nullptr;
	}
	m_vertexes.clear();
	m_weapons.clear();
	m_owner = nullptr;
	m_map = nullptr;
	if (m_controller) {
		delete m_controller;
		m_controller = nullptr;
	}
	if (m_deathTimer) {
		delete m_deathTimer;
		m_deathTimer = nullptr;
	}
	if (m_sprites) {
		delete m_sprites;
		m_sprites = nullptr;
	}
	m_animGroups.clear();
	if (m_animClock) {
		delete m_animClock;
		m_animClock = nullptr;
	}
}
void Actor::UpdateAnimClock() {
	for (AnimationGroupDefinition* animDef : m_animGroups) {
		if (animDef->m_name.compare(m_currentAction) == 0) {
			if (animDef->scaleBySpeed) {
				m_animClock->SetTimeScale(m_velocity.GetLength() / m_definition->m_physics->m_walkSpeed);
			}
			else {
				m_animClock->SetTimeScale(1.f);
			}
		}
	}
}

std::string Actor::GetSoundPath(std::string name) {
	for (Sound* sound : m_definition->m_sound) {
		if (sound->m_sound.compare(name) == 0) {
			return sound->m_path;
		}
	}
	return"";
}

Vec3 Actor::GetForwardNormal() {
	Vec3 i;
	Vec3 j;
	Vec3 k;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp(i, j, k);
	return i;
}