#include "StairInteraction.hpp"
#include "Object.hpp"
#include "Game.hpp"

StairInteraction::StairInteraction(Map* targetMap, IntVec2 targetPos)
{
    m_targetMap = targetMap;
    m_targetPos = targetPos;
}

void StairInteraction::OnInteraction(Object* other)
{
    if (other->m_def == ObjectDefinition::g_objDefinitions[(int)ObjectType::PLAYER]) {
        g_game->ChangeMap(m_targetMap, m_targetPos);
    }
}
