#pragma once

#include "Entity.hpp"
class Beetle : public Entity {
public:
	void Update(float deltaSecond);
	void Render();
	void InitializeVertex();
	Beetle(Game* owner, Vec2 const& pos, float ordientationDegree);
	Beetle(){}
	~Beetle(){}
public:
	Vec2* m_vertex = nullptr;

private:
	float m_spd = 12;
	Vec2 m_disp;
};