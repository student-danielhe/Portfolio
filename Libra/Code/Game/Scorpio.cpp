#include"Scorpio.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include"Game.hpp"
#include "Map.hpp"
Scorpio::Scorpio(Map* owner, Vec2 const& pos, float ordientationDegree)
	: Entity(owner, pos, ordientationDegree)
{
	m_physicsRadius = g_gameConfigBlackboard.GetValue("SCORPIO_PHYSICS_RADIUS", .3f);
	m_cosmeticRadius = g_gameConfigBlackboard.GetValue("SCORPIO_COSMETIC_RADIUS", .5f);
	m_health = g_gameConfigBlackboard.GetValue("SCORPIO_HP", 5);
	m_localVertexArray = std::vector<Vertex_PCU>(4, Vertex_PCU());
	m_localVertexArray[0].m_position = Vec3(-m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[0].m_uvTexCoords = Vec2(0, 0);
	m_localVertexArray[1].m_position = Vec3(m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[1].m_uvTexCoords = Vec2(1, 0);
	m_localVertexArray[2].m_position = Vec3(m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[2].m_uvTexCoords = Vec2(1, 1);
	m_localVertexArray[3].m_position = Vec3(-m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[3].m_uvTexCoords = Vec2(0, 1);
	m_pushes = true;
	m_pushable = false;
	m_pushedByWall = false;
}
//----------------------
void Scorpio::Update(float deltaSecond) {
	if (m_health <= 0) {
		m_isGarbage = true;
	}
	if (!m_owner->m_player->m_isDead) {
		if (m_owner->HasLineOfSight(m_position, m_owner->m_player->m_position, g_gameConfigBlackboard.GetValue("SCORPIO_RANGE", 10.f))) {
			if (IsPlayerInRange()) {
				Shoot(deltaSecond);
			}
			Vec2 vecToPlayer = ((m_owner->m_player->m_position - m_position));
			m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, vecToPlayer.GetOrientationDegrees(), g_gameConfigBlackboard.GetValue("SCORPIO_TURN_SPEED", 30.f) * deltaSecond);
		}
		else {
			m_orientationDegrees += g_gameConfigBlackboard.GetValue("SCORPIO_TURN_SPEED", 30.f) *deltaSecond;
		}
	}
	else {
		m_orientationDegrees += g_gameConfigBlackboard.GetValue("SCORPIO_TURN_SPEED", 30.f) * deltaSecond;
	}
	

}
bool Scorpio::IsPlayerInRange() {
	return IsPointInsideOrientedSector2D(m_owner->m_player->m_position, m_position, m_orientationDegrees, 5, g_gameConfigBlackboard.GetValue("SCREEN_SIZE_X", 16.f) + g_gameConfigBlackboard.GetValue("SCREEN_SIZE_Y", 8.f));
}
void Scorpio::Shoot(float deltaSecond) {
	if (m_currentFireCooldown > 0) {
		m_currentFireCooldown -= deltaSecond;
	}
	else {
		m_currentFireCooldown = g_gameConfigBlackboard.GetValue("SCORPIO_FIRE_DELAY", .5f);
		m_owner->SpawnNewEntity(ENTITYTYPE_EVIL_BULLET, FACTION_EVIL, m_position + Vec2(m_physicsRadius, 0).GetRotatedDegrees(m_orientationDegrees), m_orientationDegrees);
	}
}
void Scorpio::RenderBase(std::vector<Vertex_PCU>& verts) {
	
	Vertex_PCU BL = m_localVertexArray[0];
	BL.m_position.x += m_position.x;
	BL.m_position.y += m_position.y;
	Vertex_PCU BR = m_localVertexArray[1];
	BR.m_position.x += m_position.x;
	BR.m_position.y += m_position.y;
	Vertex_PCU TR = m_localVertexArray[2];
	TR.m_position.x += m_position.x;
	TR.m_position.y += m_position.y;
	Vertex_PCU TL = m_localVertexArray[3];
	TL.m_position.x += m_position.x;
	TL.m_position.y += m_position.y;
	AddVertsForRect(verts, BL, BR, TR, TL);


}
void Scorpio::RenderLaser(std::vector<Vertex_PCU>& verts) {
	UNUSED(verts);
	Vec2 forwardNormal = Vec2::MakeFromPolarDegrees(m_orientationDegrees);
	RaycastResult2D result = m_owner->RaycastVsTiles(m_position, m_position + forwardNormal* g_gameConfigBlackboard.GetValue("SCORPIO_RANGE", 10.f));
	DebugDrawLine(m_position+forwardNormal*m_cosmeticRadius, result.m_impactPos, .02f, Rgba8(255, 0, 0));
}
