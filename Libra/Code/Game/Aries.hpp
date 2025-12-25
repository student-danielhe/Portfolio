#pragma once
#include "Entity.hpp"
class Aries:public Entity {
	friend class Map;
protected:
	explicit Aries(Map* owner, Vec2 const& pos, float ordientationDegree);
	~Aries() {}
	virtual void Update(float deltaSecond) override;

	bool IsPlayerInFront();
	void Idle(float deltaSecond);
	void MoveForward(float deltaSecond);
	Vec2 m_targetPos;
	float m_idleTurnCoolDown = 3.f;
};