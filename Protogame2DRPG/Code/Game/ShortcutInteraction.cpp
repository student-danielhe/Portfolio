#include "ShortcutInteraction.hpp"
#include "Object.hpp"

ShortcutInteraction::ShortcutInteraction()
{

}

void ShortcutInteraction::OnInteraction(Object* other)
{
    if (other->m_def->m_type == ObjectType::PLAYER) {
        if (other->m_currentDirection == m_owner->m_currentDirection) {
            m_owner->m_isDestroyed = true;
        }
    }
}
