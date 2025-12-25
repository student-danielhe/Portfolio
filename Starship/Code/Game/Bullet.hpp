#pragma once

#include "Entity.hpp"

class Bullet : public Entity {
public:
	float m_scale = 1.0f;
	void Update(float deltaSecond);
	void Render();
	Bullet(Game* owner, Vec2 const& pos, float ordientationDegree);
	Bullet();
	~Bullet();
public:
	Vertex_PCU* m_localVertexArray = nullptr;
};