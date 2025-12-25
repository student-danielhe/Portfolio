#include "MarineAI.hpp"
#include "Map.hpp"
MarineAI::MarineAI(ActorHandle* handle, Map* map) :
	Controller(handle, map) {
}
void MarineAI::DamagedBy(ActorHandle* handle) {
	m_targetEnemyHandle = handle;
}

void MarineAI::Update() {
	if (m_map->GetActorByHandle(*m_ownerHandle) && !m_map->GetActorByHandle(*m_ownerHandle)->m_dead) {
		Actor* owner = m_map->GetActorByHandle(*m_ownerHandle);
		//Chase enemy
		if (m_targetEnemyHandle != nullptr && m_map->GetActorByHandle(*m_targetEnemyHandle) != nullptr) {


			Actor* target = m_map->GetActorByHandle(*m_targetEnemyHandle);
			Vec3 targetToMe = target->m_bottomPosition - owner->m_bottomPosition;
			float deg = Vec2(targetToMe.x, targetToMe.y).GetOrientationDegrees();

			//use weapon if in range
			bool attacked = false;
			owner->m_orientation.m_yawDegrees = deg; /*->TurnInDirection(deg, owner->m_definition->m_physics->m_turnSpeed * (float)g_gameClock->GetDeltaSeconds());*/
			if (!owner->m_weapons.empty()) {
				for (Weapon* weapon : owner->m_weapons) {
					if (weapon->m_definition->m_name.compare("Pistol")) {
						if (targetToMe.GetLengthSquared() < m_attackRenge*m_attackRenge) {
							weapon->Fire();
							attacked = true;

						}
					}
				}
			}

			if (!attacked) {
				float speed = owner->m_definition->m_physics->m_walkSpeed;
				if (targetToMe.GetLength() >= m_attackRenge) {
					if (!m_advancedPathFinding) {
						owner->MoveInDirection(targetToMe.GetNormalized(), speed);
					}
					else {
						Vec3 targetNormal = GetTargetPath(m_targetEnemyHandle);
						if (targetNormal != Vec3())
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
				m_targetEnemyHandle = target->m_handle;
			}
			else {
				for (int i = 0; i < m_map->m_actors.size(); i++) {
					target = m_map->m_actors[i];
					if (target!= nullptr&&owner->OpposingFaction(target->m_handle)) {
						if (target->m_running) {
							Vec3 targetToMe = target->m_bottomPosition - owner->m_bottomPosition;
							if (targetToMe.GetLengthSquared() < m_runningDetectionRange * m_runningDetectionRange) {
								m_targetEnemyHandle = target->m_handle;
							}
						}
					}
				}
			}
		}
	}
}