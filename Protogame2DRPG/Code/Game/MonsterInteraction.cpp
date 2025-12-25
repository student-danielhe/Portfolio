#include "MonsterInteraction.hpp"
#include "Object.hpp"
#include "Map.hpp"
#include "Engine/Core/Timer.hpp"
MonsterInteraction::MonsterInteraction()
{
    m_movementTimer = new Timer(MOVEMENT_TIME*5.f, g_gameClock);
    m_movementTimer->Start();
}

void MonsterInteraction::OnInteraction(Object* other)
{
    UNUSED(other);
    m_owner->m_isDestroyed=true;
}

void MonsterInteraction::OnTick(float deltaSeconds)
{
    UNUSED(deltaSeconds);

    
    if (m_movementTimer->DecrementPeriodIfElapsed()) {
        
        if (!m_aggro) {
			IntVec2 ownerPosition = m_owner->m_position;
            int dist = m_owner->m_map->m_heatMap->GetHeat(ownerPosition);
            if (dist <= 7) {
                m_aggro = true;
            }
            else {
                m_owner->Move(m_owner->m_map->EnemyWander(ownerPosition, m_monsterIndex, m_step));
            }
        }

        if (m_aggro) {
			IntVec2 ownerPosition = m_owner->m_position;

			int dist = m_owner->m_map->m_heatMap->GetHeat(ownerPosition);
			if (dist >= 10) {
				m_aggro = false;
			}
            else {
				IntVec2 movement = m_owner->m_map->GetPathToPlayer(ownerPosition);

				if (movement != IntVec2(0, 0)) {
					IntVec2 newPos = ownerPosition + movement;
					if (!m_owner->m_map->m_chObjects[(int)(newPos.x + newPos.y * m_owner->m_map->m_dimensions.x)])
						m_owner->Move(newPos);
				}
            }
        }
        
        m_step++;

        
    }
}
