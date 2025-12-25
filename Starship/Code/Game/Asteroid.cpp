#include "Asteroid.hpp"
#include<math.h>
#include"Game.hpp"
#include "GameCommon.hpp"
//extern Renderer* g_theRenderer;
void Asteroid::Update(float deltaSecond) {
	m_position += m_verlocity*deltaSecond;
	m_orientationDegrees += m_angularVelocity*deltaSecond;
	if (m_health <= 0) {
		Dies();
		m_isGarbage = true;
	}
	if (IsOffScreen()) {
		m_position = m_owner->EnemySpawnLocation(m_cosmeticRadius);
	}
	if (m_isDead) {
		m_isGarbage = true;
	}

}
void Asteroid::Render() {
	Vertex_PCU* arr = new Vertex_PCU[48];
	Rgba8 color(100,100,100);
	Vec2 uv(0,0);
	Vec3 center(0,0,0);
	for(int i=0;i<48;i+=1){
		arr[i].m_color = color;
		arr[i].m_uvTexCoords = uv;
		int angle = i % 3;
		if (angle == 0) {
			arr[i].m_position = center;
		}
		if (angle == 1) {
			arr[i].m_position = Vec3(m_edges[(i-1)/3].x,m_edges[(i-1)/3].y,0).GetRotatedAboutZDegrees(m_orientationDegrees);
		}
		if (angle == 2) {
			if (i ==47) {
				arr[i].m_position = Vec3(m_edges[0].x, m_edges[0].y, 0).GetRotatedAboutZDegrees(m_orientationDegrees);
			}
			else {
				arr[i].m_position = Vec3(m_edges[(i-2) / 3 + 1].x, m_edges[(i-2) / 3 + 1].y, 0).GetRotatedAboutZDegrees(m_orientationDegrees);
			}
			
		}
		arr[i].m_position += Vec3(m_position.x,m_position.y,0);

	}

	g_theRenderer->DrawVertexArray(48, arr);
}
Asteroid::Asteroid(Game* owner, Vec2 const& pos, float ordientationDegree) :Entity(owner, pos, ordientationDegree)
{
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	RandomNumberGenerator* rand=new RandomNumberGenerator();
	m_angularVelocity = rand->RollRandomFloatInRange(-200, 200);
	m_verlocity = Vec2(ASTEROID_SPEED/2, 0);
	m_verlocity.SetOrientationDegrees(ordientationDegree);
	m_health = (int)ASTEROID_MAXHP;
	m_edges = new Vec2[16];
	m_damage = 3;
	for (int i = 0; i < 16;i++) {
		float len =  rand->RollRandomFloatInRange(ASTEROID_PHYSICS_RADIUS, ASTEROID_COSMETIC_RADIUS);
		Vec2* vert=new Vec2();
		m_edges[i] = vert->MakeFromPolarDegrees((float)i*(360.f/16.f), len);
	}
}
