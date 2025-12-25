#pragma once

#include "Entity.hpp"

class PlayerShip : public Entity {
public:

	void Update(float deltaSecond);
	void Render();
	PlayerShip(Game* owner, Vec2 const& pos, float ordientationDegree);
	~PlayerShip(){}
	void RenderTail(float deg, float slow);
	void HandleKeyPress(float deltaSecond);
	void Bounce();
	void KnockedBack(Vec2 const& position);
public:
	Vec3* m_localVertexArray = nullptr;
	bool m_slowTurn = false;
	float m_shootDelay = 120;
	float m_shootRate = 5;
	float m_shootPrep = 0;
	float m_maxHP=20;
	int m_multiShoot=1;
};