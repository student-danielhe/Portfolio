#include "Debris.hpp"
#include<math.h>
#include"Game.hpp"
#include "GameCommon.hpp"
//extern Renderer* g_theRenderer;
void Debris::Update(float deltaSecond) {
	m_position += m_verlocity * deltaSecond;
	m_orientationDegrees += m_angularVelocity * deltaSecond;
	if (IsOffScreen()) {
		m_isGarbage = true;
	}
	m_transparency--;
	if (m_transparency==0) {
		m_isGarbage = true;
	}

}
void Debris::Render() {
	Vertex_PCU* arr = new Vertex_PCU[24];
	Rgba8 color = m_parentColor;
	color.a = (unsigned char)m_transparency;
	Vec2 uv(0, 0);
	Vec3 center(0, 0, 0);
	for (int i = 0; i < 24; i += 1) {
		arr[i].m_color = color;
		arr[i].m_uvTexCoords = uv;
		int angle = i % 3;
		if (angle == 0) {
			arr[i].m_position = center;
		}
		if (angle == 1) {
			arr[i].m_position = Vec3(m_edges[(i - 1) / 3].x, m_edges[(i - 1) / 3].y, 0).GetRotatedAboutZDegrees(m_orientationDegrees);
		}
		if (angle == 2) {
			if (i == 23) {
				arr[i].m_position = Vec3(m_edges[0].x, m_edges[0].y, 0).GetRotatedAboutZDegrees(m_orientationDegrees);
			}
			else {
				arr[i].m_position = Vec3(m_edges[(i - 2) / 3 + 1].x, m_edges[(i - 2) / 3 + 1].y, 0).GetRotatedAboutZDegrees(m_orientationDegrees);
			}

		}
		arr[i].m_position += Vec3(m_position.x, m_position.y, 0);

	}
	g_theRenderer->DrawVertexArray(24, arr);
}
Debris::Debris(Game* owner, Vec2 const& pos, float ordientationDegree, Vec2 intialVerlocity,Rgba8 Color) :Entity(owner, pos, ordientationDegree)
{
	m_parentColor = Color;
	RandomNumberGenerator* rand = new RandomNumberGenerator();
	m_angularVelocity = rand->RollRandomFloatInRange(-200, 200);
	m_verlocity = Vec2(rand->RollRandomFloatInRange(0 * WORLD_SIZE_X / 400.f,10 * WORLD_SIZE_X / 400.f), 0 * WORLD_SIZE_X / 400.f);
	m_verlocity.SetOrientationDegrees(ordientationDegree);
	m_verlocity += intialVerlocity * WORLD_SIZE_X / 400.f;
	m_edges = new Vec2[8];
	for (int i = 0; i < 8; i++) {
		float len = rand->RollRandomFloatInRange(.3f * WORLD_SIZE_X / 400.f, 1.f * WORLD_SIZE_X / 400.f);
		Vec2* vert = new Vec2();
		m_edges[i] = vert->MakeFromPolarDegrees((float)i * (360.f / 8.f), len);
	}
}