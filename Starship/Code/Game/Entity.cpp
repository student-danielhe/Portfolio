#include "Entity.hpp"
#include "GameCommon.hpp"
#include"Game.hpp"
Entity::Entity(Game* owner, Vec2 const& pos, float ordientationDegree)
	:m_owner (owner)
	, m_position(pos)
	,m_orientationDegrees(ordientationDegree)
{

}

void Entity::Update() {
	m_position += m_verlocity;
	m_orientationDegrees += m_angularVelocity;
	if (m_health == 0) {
		m_isDead = true;
	}
}
void Entity::Render() {
	//render code
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
	if (displacement.GetLength() < m_physicsRadius + target->m_physicsRadius&&target->IsAlive()) {
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