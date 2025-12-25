#include "EnvObstacleInteraction.hpp"
#include "Map.hpp"
#include "Object.hpp"
#include "Tile.hpp"

void EnvObstacleInteraction::OnCreate() {
	Map* map = m_owner->m_map;
	map->m_tiles[(int)(m_owner->m_position.x + m_owner->m_position.y * map->m_dimensions.x)].m_blockPath = true;
}