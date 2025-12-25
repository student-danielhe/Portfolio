#pragma once

#include "Entity.hpp"

class Asteroid : public Entity {
public:
	void Update(float deltaSecond);
	void Render();
	Asteroid(Game* owner, Vec2 const& pos, float ordientationDegree);
	Asteroid(){}
	~Asteroid(){}
public:
	Vec2* m_edges=nullptr;
};