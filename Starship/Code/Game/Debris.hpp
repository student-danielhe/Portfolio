#pragma once

#include "Entity.hpp"

class Debris : public Entity {
public:
	void Update(float deltaSecond);
	void Render();
	Debris(Game* owner, Vec2 const& pos, float ordientationDegree, Vec2 initialVerlocity, Rgba8 parentColor);
	Debris() {}
	~Debris(){}
public:
	Vec2* m_edges = nullptr;
	float m_transparency=127;
	Rgba8 m_parentColor;
	
};