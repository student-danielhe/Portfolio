#pragma once
#include"Entity.hpp"
class Leo :public Entity{
	friend class Map;
protected:
	explicit Leo(Map* owner, Vec2 const& pos, float ordientationDegree);
	~Leo() {}
	virtual void Update(float deltaSecond) override;

	bool IsPlayerInFront();
	bool IsPlayerInRange();
	void Shoot(float deltaSecond);
	void Idle(float deltaSecond);
	void MoveForward(float deltaSecond);
	Vec2 m_targetPos;
	float m_idleTurnCoolDown=3.f;
};