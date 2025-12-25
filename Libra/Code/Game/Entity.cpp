#include "Entity.hpp"
#include "GameCommon.hpp"
#include"Game.hpp"
Entity::Entity(Map* owner, Vec2 const& pos, float ordientationDegree)
	:m_owner(owner)
	, m_position(pos)
	, m_orientationDegrees(ordientationDegree)
{

}

void Entity::Update(float deltaSecond) {
	m_position += m_verlocity*deltaSecond;
	m_orientationDegrees += m_angularVelocity*deltaSecond;
	if (m_health == 0) {
		m_isDead = true;
	}
}
void Entity::Render(std::vector<Vertex_PCU>& verts) {
	//render code
	//DebugDrawRing(m_position, 20, 3, Rgba8(255, 0, 0));
	std::vector<Vertex_PCU>arr;
	GlobalVertexArray(arr);

	AddVertsForRect(verts, arr[0], arr[1], arr[2], arr[3]);
}
bool Entity::IsAlive() {
	return !m_isDead;
}
bool Entity::IsOffScreen() {
	float x = m_position.x;
	float y = m_position.y;
	if (x < m_cosmeticRadius) {
		return true;
	}
	if (x > WORLD_SIZE_X + m_cosmeticRadius) {
		return true;
	}
	if (y < m_cosmeticRadius) {
		return true;
	}
	if (y > WORLD_SIZE_Y + m_cosmeticRadius) {
		return true;
	}

	return false;
}
bool Entity::Collide(Entity* target) {
	Vec2 displacement = m_position - target->m_position;
	if (displacement.GetLength() < m_physicsRadius + target->m_physicsRadius && target->IsAlive()) {
		return true;
	}
	return false;
}
Vec2 Entity::GetFowardNormal() {
	return m_verlocity.GetNormalized();
}
void Entity::Dies() {
	m_isDead = true;
	//spawn debris
}
void Entity::PushOutOfWalls(std::vector<AABB2*> walls) {
	for (AABB2* box : walls) {
		PushDiscOutOfAABB2D(m_position, m_physicsRadius, *box);
	}
	
}
void Entity::DebugRender()const {
	//inner circle
	DebugDrawRing(m_position, m_physicsRadius, 1, Rgba8(0, 0, 255));
	//outer circle
	DebugDrawRing(m_position, m_cosmeticRadius, 1, Rgba8(255, 0, 255));

	DebugDrawLine(m_position, m_position + Vec2(m_physicsRadius * 2, 0).GetRotatedDegrees(m_orientationDegrees), 1, Rgba8(255, 255, 0));
	DebugDrawLine(m_position, m_position + Vec2(m_physicsRadius * 2, 0).GetRotatedDegrees(m_orientationDegrees+90), 1, Rgba8(255,0, 0));
	//forward vector
	//DebugDrawLine()
}
void Entity::GlobalVertexArray(std::vector<Vertex_PCU> &result) {
	;
	for (Vertex_PCU vertex : m_localVertexArray) {
		Vec2 position(vertex.m_position.x,vertex.m_position.y);
		position=position.GetRotatedDegrees(m_orientationDegrees);
		position += m_position;
		Vertex_PCU globalVertex;
		globalVertex.m_position = Vec3(position.x, position.y, 0);
		globalVertex.m_color = Rgba8(255,255,255);
		globalVertex.m_uvTexCoords = vertex.m_uvTexCoords;
		result.push_back(globalVertex);
	}
}