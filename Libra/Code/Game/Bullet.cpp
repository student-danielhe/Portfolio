#include "Bullet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include"Game.hpp"
#include "Engine/Math/MathUtils.hpp"

Bullet::Bullet(Map* owner, Vec2 const& pos, float ordientationDegree)
	: Entity(owner, pos, ordientationDegree)
{

	m_physicsRadius = .02f* g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
	m_cosmeticRadius = .05f* g_gameConfigBlackboard.GetValue("GRID_SIZE", 1.f);
	//
	m_localVertexArray = std::vector<Vertex_PCU>(4, Vertex_PCU());
	m_localVertexArray[0].m_position = Vec3(-2*m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[0].m_uvTexCoords = Vec2(0, 0);
	m_localVertexArray[1].m_position = Vec3(2*m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[1].m_uvTexCoords = Vec2(1, 0);
	m_localVertexArray[2].m_position = Vec3(2*m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[2].m_uvTexCoords = Vec2(1, 1);
	m_localVertexArray[3].m_position = Vec3(-2*m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[3].m_uvTexCoords = Vec2(0, 1);
	m_pushes = false;
	m_pushable = false;
	m_pushedByWall = false;
}
void Bullet::Update(float deltaSecond) {
	if (m_health <= 0) {
		m_isGarbage = true;
	}
	Vec2 disp = Vec2(g_gameConfigBlackboard.GetValue("BULLET_SPEED", 1.2f) * deltaSecond, 0);
	disp=disp.GetRotatedDegrees(m_orientationDegrees);
	m_position += disp;
}
