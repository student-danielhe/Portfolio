#include "Faction.hpp"

Faction::Faction(FactionType type, Map* owner): m_owner(owner), m_factionID(type)
{
    if (m_factionID != FactionType::NEUTRAL) {
        m_UI = new FactionUI(m_factionID);
    }
   

}
