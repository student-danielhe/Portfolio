#pragma once

#include "Entity.hpp"
class Wasp : public Entity {
public:
	void Update(float deltaSecond);
	void Render();
	void InitializeVertex();
	Wasp(Game* owner, Vec2 const& pos, float ordientationDegree);
	Wasp() {}
	~Wasp() {}
public:
	Vec2* m_vertex = nullptr;

private:
	Vec2 m_accel =Vec2(10,0);
	Vec2 m_disp;
};