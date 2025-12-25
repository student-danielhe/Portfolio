#include "Leo.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include"Game.hpp"
#include"Map.hpp"
Leo::Leo(Map* owner, Vec2 const& pos, float ordientationDegree)
	: Entity(owner, pos, ordientationDegree)
{
	m_physicsRadius = g_gameConfigBlackboard.GetValue("LEO_PHYSICS_RADIUS", .3f);
	m_cosmeticRadius = g_gameConfigBlackboard.GetValue("LEO_COSMETIC_RADIUS", .5f);
	//
	m_health = g_gameConfigBlackboard.GetValue("LEO_HP", 5);
	m_localVertexArray = std::vector<Vertex_PCU>(4, Vertex_PCU());
	m_localVertexArray[0].m_position = Vec3(-m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[0].m_uvTexCoords = Vec2(0, 0);
	m_localVertexArray[1].m_position = Vec3(m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[1].m_uvTexCoords = Vec2(1, 0);
	m_localVertexArray[2].m_position = Vec3(m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[2].m_uvTexCoords = Vec2(1, 1);
	m_localVertexArray[3].m_position = Vec3(-m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[3].m_uvTexCoords = Vec2(0, 1);
}
//--------------------------
void Leo::Update(float deltaSecond) {
	if (m_health <= 0) {
		m_isGarbage = true;
	}
	if (!m_owner->m_player->m_isDead) {
		if (m_owner->HasLineOfSight(m_position, m_owner->m_player->m_position, g_gameConfigBlackboard.GetValue("LEO_RANGE", 5.f))) {
			m_targetPos = m_owner->m_player->m_position;

		}
		if (m_targetPos != Vec2(0, 0)) {
			if (IsPlayerInFront()) {
				if (IsPlayerInRange()) {
					Shoot(deltaSecond);
				}
				MoveForward(deltaSecond);
			}
			Vec2 vecToPlayer = ((m_targetPos - m_position));
			m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, vecToPlayer.GetOrientationDegrees(), g_gameConfigBlackboard.GetValue("LEO_TURN_SPEED", 30.f) * deltaSecond);
			if (IsPointInsideDisc2D(m_targetPos, m_position, m_physicsRadius)) {
				m_targetPos = Vec2(0, 0);
			}
		}
		else {
			Idle(deltaSecond);
		}
	}
	else {
		m_targetPos = Vec2(0, 0);
		Idle(deltaSecond);
	}
	
	
}
bool Leo::IsPlayerInFront() {
	return IsPointInsideOrientedSector2D(m_targetPos, m_position, m_orientationDegrees, 45, g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f) + g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f));
}
bool Leo::IsPlayerInRange() {
	return IsPointInsideOrientedSector2D(m_owner->m_player->m_position, m_position, m_orientationDegrees, 5, g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f) + g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f));
}
void Leo::Shoot(float deltaSecond) {
	if (m_currentFireCooldown > 0) {
		m_currentFireCooldown -= deltaSecond;
	}
	else {
		m_currentFireCooldown = g_gameConfigBlackboard.GetValue("LEO_FIRE_DELAY", .5f);
		m_owner->SpawnNewEntity(ENTITYTYPE_EVIL_BULLET, FACTION_EVIL, m_position + Vec2(m_physicsRadius, 0).GetRotatedDegrees(m_orientationDegrees), m_orientationDegrees);
	}
}
void Leo::Idle(float deltaSecond) {
	if (m_idleTurnCoolDown > 0) {
		m_idleTurnCoolDown -= deltaSecond;
	}
	else {
		m_idleTurnCoolDown = 3.f;
		m_orientationDegrees = g_RNG->RollRandomFloatInRange(0, 360);
	}
	MoveForward(deltaSecond);

}
void Leo::MoveForward(float deltaSecond) {
	m_position += Vec2(g_gameConfigBlackboard.GetValue("LEO_SPEED", .5f) * deltaSecond, 0).GetRotatedDegrees(m_orientationDegrees);
}

