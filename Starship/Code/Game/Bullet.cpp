#include "Bullet.hpp"
#include<math.h>
#include"Game.hpp"
#include "GameCommon.hpp"
//extern Renderer* g_theRenderer;

Bullet::Bullet(Game* owner, Vec2 const& pos, float ordientationDegree)
	:Entity(owner, pos, ordientationDegree)
{

	m_physicsRadius = BULLET_PHYSICS_RADIUS*m_scale;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS*m_scale;
	//halfway
	m_angularVelocity = 0;
	m_verlocity = Vec2(BULLET_SPEED,0);
	m_verlocity.SetOrientationDegrees(ordientationDegree);

	//head
	m_localVertexArray = new Vertex_PCU[6];
	m_localVertexArray[0].m_position = Vec3(.5 * WORLD_SIZE_X / 400.f, 0, 0).GetRotatedAboutZDegrees(ordientationDegree);
	m_localVertexArray[1].m_position = Vec3(0, .5 * WORLD_SIZE_X / 400.f, 0).GetRotatedAboutZDegrees(ordientationDegree);
	m_localVertexArray[2].m_position = Vec3(0, -.5 * WORLD_SIZE_X / 400.f, 0).GetRotatedAboutZDegrees(ordientationDegree);
	//tail
	m_localVertexArray[3].m_position = Vec3(0, .5 * WORLD_SIZE_X / 400.f, 0).GetRotatedAboutZDegrees(ordientationDegree);
	m_localVertexArray[4].m_position = Vec3(0, -.5 * WORLD_SIZE_X / 400.f, 0).GetRotatedAboutZDegrees(ordientationDegree);
	m_localVertexArray[5].m_position = Vec3(-2 * WORLD_SIZE_X / 400.f, 0, 0).GetRotatedAboutZDegrees(ordientationDegree);
	
	m_localVertexArray[0].m_color = Rgba8(255,255,0);
	m_localVertexArray[1].m_color = Rgba8(255, 0, 0);
	m_localVertexArray[2].m_color = Rgba8(255, 0, 0);
	m_localVertexArray[3].m_color = Rgba8(255, 0, 0);
	m_localVertexArray[4].m_color = Rgba8(255, 0, 0);
	m_localVertexArray[5].m_color = Rgba8(255, 0, 0);
	m_localVertexArray[5].m_color.a = 0;
	for (int i = 0; i < 6; i++) {
		m_localVertexArray[i].m_uvTexCoords = Vec2(0, 0);
	}
}

void Bullet::Update(float deltasecond){
	m_position += m_verlocity*deltasecond;
	if (IsOffScreen()) {
		m_isDead = true;
	}
	if (m_isDead) {
		m_isGarbage = true;
	}
}

void Bullet::Render() {
	Vec3 pos(m_position.x,m_position.y,0);
	Vertex_PCU* arr = new Vertex_PCU[6];
	for (int i = 0; i < 6; i++) {
		arr[i] = Vertex_PCU(m_localVertexArray[i].m_position*m_scale+pos,m_localVertexArray[i].m_color,m_localVertexArray[i].m_uvTexCoords);
	}
	g_theRenderer->DrawVertexArray(6,arr);
}
Bullet::~Bullet() {

}

