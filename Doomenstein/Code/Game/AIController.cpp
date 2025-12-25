#include "AIController.hpp"
#include "Map.hpp"
#include "Game.hpp"
std::vector<ActorHandle*> DemonAIController::g_party;
DemonAIController::DemonAIController(ActorHandle* handle, Map* map):
Controller(handle, map){}
void DemonAIController::DamagedBy(ActorHandle* handle) {
	m_targetEnemyHandle = handle;

	if(m_inParty)
	    DemonAIController::GlobalAggro(handle);
}
void DemonAIController::Update() {
	if (m_map->GetActorByHandle(*m_ownerHandle) && !m_map->GetActorByHandle(*m_ownerHandle)->m_dead) {
		Actor* owner = m_map->GetActorByHandle(*m_ownerHandle);
		float speed = owner->m_definition->m_physics->m_walkSpeed;
		if (owner->m_running) {
			speed = owner->m_definition->m_physics->m_runSpeed;
		}
		//Chase enemy
		if (m_targetEnemyHandle != nullptr && m_map->GetActorByHandle(*m_targetEnemyHandle) != nullptr&& !m_map->GetActorByHandle(*m_targetEnemyHandle)->m_dead) {
			
			
			Actor* target = m_map->GetActorByHandle(*m_targetEnemyHandle);
			Vec3 targetToMe = target->m_bottomPosition - owner->m_bottomPosition;
			float deg = Vec2(targetToMe.x, targetToMe.y).GetOrientationDegrees();
			
			//use weapon if in range
			bool attacked = false;
			if (!owner->m_weapons.empty()) {
				for (Weapon* weapon : owner->m_weapons) {
					if (weapon->m_definition->m_meleeRange > 0.f) {
						if (targetToMe.GetLengthSquared() < weapon->m_definition->m_meleeRange * weapon->m_definition->m_meleeRange) {
							weapon->Fire();
							attacked = true;

						}
					}
				}
			}
			//chase enemy
			if (!attacked) {
				owner->TurnInDirection(deg, owner->m_definition->m_physics->m_turnSpeed * (float)g_gameClock->GetDeltaSeconds());
				if (targetToMe.GetLength() >= WeaponDefinition::GetWeaponDefinitionByName("DemonMelee")->m_meleeRange) {
					if (!m_advancedPathFinding) {
						owner->MoveInDirection(targetToMe.GetNormalized(), speed);
					}
					else {
					    Vec3 targetNormal = GetTargetPath(m_targetEnemyHandle);
						if(targetNormal!=Vec3())
							owner->MoveInDirection(targetNormal, speed);
						else
						    owner->MoveInDirection(targetToMe.GetNormalized(), speed);

					}
				}
					
			}
		
		}
		else {
			Actor* target = m_map->GetClosestVisibleEnemyWithinRange(m_ownerHandle);
			
			if (target != nullptr) {
				Vec3 targetToMe = target->m_bottomPosition - owner->m_bottomPosition;
				if (targetToMe.GetLengthSquared() <= m_aggroRange * m_aggroRange) {
					if (!m_inParty) {
						m_targetEnemyHandle = target->m_handle;
					}
					else {
						m_targetEnemyHandle = target->m_handle;
						DemonAIController::GlobalAggro(target->m_handle);
					}
				}
				    
			}
		}

		if (m_targetEnemyHandle == nullptr || m_map->GetActorByHandle(*m_targetEnemyHandle) == nullptr) {
			if (m_followAllyHandle!= nullptr && m_map->GetActorByHandle(*m_followAllyHandle) != nullptr) {
				Actor* target = m_map->GetActorByHandle(*m_followAllyHandle);
				Vec3 targetToMe = target->m_bottomPosition - owner->m_bottomPosition;
				float deg = Vec2(targetToMe.x, targetToMe.y).GetOrientationDegrees();
				owner->TurnInDirection(deg, owner->m_definition->m_physics->m_turnSpeed * (float)g_gameClock->GetDeltaSeconds());
				if (targetToMe.GetLengthSquared() >= 1.f)
					if (!m_advancedPathFinding) {
						owner->MoveInDirection(targetToMe.GetNormalized(), speed);
					}
					else {
						Vec3 targetNormal = GetTargetPath(m_followAllyHandle);
						if (targetNormal != Vec3())
							owner->MoveInDirection(targetNormal, speed);
						else
							owner->MoveInDirection(targetToMe.GetNormalized(), speed);

					}
				//teleport if lost
				if (targetToMe.GetLengthSquared() >= 400.f) {
					owner->m_bottomPosition = target->m_bottomPosition;
				}
			}
		}

	}
}



void DemonAIController::UpdateParty() {
	std::vector<int> indexToErase;
	bool run = g_theGame->m_currentLoadedMap->GetActorByHandle(*g_party[0])->m_running;
	for (int i = 1; i < g_party.size(); i++) {
		if (DemonAIController::g_party[i]!=nullptr&& g_theGame->m_currentLoadedMap->GetActorByHandle(*DemonAIController::g_party[i])!=nullptr) {
			g_theGame->m_currentLoadedMap->GetActorByHandle(*DemonAIController::g_party[i])->m_running = run;
			for (int j = i - 1; j >= 0; j--) {
				if (DemonAIController::g_party[j]!=nullptr&& g_theGame->m_currentLoadedMap->GetActorByHandle(*DemonAIController::g_party[j])!=nullptr) {
					g_theGame->m_currentLoadedMap->GetActorByHandle(*DemonAIController::g_party[i])->m_AIController->m_followAllyHandle = g_party[j];
					break;
				}
			}
		}
		else {
			indexToErase.push_back(i);
		}
	}
	for (int i : indexToErase) {
		DemonAIController::g_party.erase(DemonAIController::g_party.begin() + i);
	}
}

void DemonAIController::GlobalAggro(ActorHandle* target) {
	for (int i = 1; i < g_party.size(); i++) {
		DemonAIController* controller = g_theGame->m_currentLoadedMap->GetActorByHandle(*g_party[i])->m_AIController;
		if (DemonAIController::g_party[i]!=nullptr&& g_theGame->m_currentLoadedMap->GetActorByHandle(*g_party[i]) != nullptr) {
			ActorHandle* enemy = controller->m_targetEnemyHandle;
			if (enemy == nullptr||g_theGame->m_currentLoadedMap->GetActorByHandle(*enemy)==nullptr) {
				controller->m_targetEnemyHandle = target;
			}
		}
	}
}