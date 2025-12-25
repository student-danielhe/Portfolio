#include"Wasp.hpp"
#include<math.h>
#include"Game.hpp"
#include "GameCommon.hpp"
#include "PlayerShip.hpp"

void Wasp::Update(float deltaSecond) {

	m_disp = m_owner->g_player->m_position - m_position;
	m_orientationDegrees = m_disp.GetOrientationDegrees();
	m_accel.SetOrientationDegrees(m_orientationDegrees);
	m_verlocity += m_accel*deltaSecond;
	m_verlocity.GetClamped(40);
	if (IsOffScreen()) {
		m_verlocity = Vec2(10, 0);
		m_verlocity.SetOrientationDegrees(m_orientationDegrees);
	}

	m_position += m_verlocity * deltaSecond;
	if (m_health <= 0) {
		Dies();
	}
	if (m_isDead) {
		m_isGarbage = true;
	}
}
void Wasp::Render() {
	Vertex_PCU* body = new Vertex_PCU[9];
	for (int i = 0; i < 9; i++) {
		body[i].m_uvTexCoords = Vec2(0, 0);
		body[i].m_color = Rgba8(255, 255, 0);
		Vec2 temp = m_position + m_vertex[i].GetRotatedDegrees(m_orientationDegrees);
		body[i].m_position = Vec3(temp.x, temp.y, 0);
	}
	g_theRenderer->DrawVertexArray(9, body);
}
Wasp::Wasp(Game* owner, Vec2 const& pos, float ordientationDegree) :Entity(owner, pos, ordientationDegree)
{
	m_cosmeticRadius = 6.0f;
	m_physicsRadius = 5.0f;
	m_health = (int)WASP_MAXHP;
	m_disp = m_owner->g_player->m_position - m_position;
	m_orientationDegrees = m_disp.GetOrientationDegrees();
	m_verlocity = Vec2(0, 0);
	m_verlocity.SetOrientationDegrees(m_orientationDegrees);
	m_vertex = new Vec2[9];
	InitializeVertex();
}

void Wasp::InitializeVertex() {

	m_vertex[0] = Vec2(-1 * WORLD_SIZE_X / 400.f, 3 * WORLD_SIZE_X / 400.f);
	m_vertex[1] = Vec2(-1 * WORLD_SIZE_X / 400.f, 1 * WORLD_SIZE_X / 400.f);
	m_vertex[2] = Vec2(2 * WORLD_SIZE_X / 400.f, 0 * WORLD_SIZE_X / 400.f);
	m_vertex[3] = Vec2(-1 * WORLD_SIZE_X / 400.f, -3 * WORLD_SIZE_X / 400.f);
	m_vertex[4] = Vec2(-1 * WORLD_SIZE_X / 400.f, -1 * WORLD_SIZE_X / 400.f);
	m_vertex[5] = Vec2(2 * WORLD_SIZE_X / 400.f, 0 * WORLD_SIZE_X / 400.f);
	m_vertex[6] = Vec2(-2 * WORLD_SIZE_X / 400.f, .5 * WORLD_SIZE_X / 400.f);
	m_vertex[7] = Vec2(-2 * WORLD_SIZE_X / 400.f, -.5 * WORLD_SIZE_X / 400.f);
	m_vertex[8] = Vec2(4 * WORLD_SIZE_X / 400.f, 0 * WORLD_SIZE_X / 400.f);
}