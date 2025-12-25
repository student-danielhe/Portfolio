#pragma once
#include "Actor.hpp"
#include "Controller.hpp"
class MarineAI :public Controller {
public:
	float m_attackRenge = 10.f;
	float m_runningDetectionRange = 5.f;
	ActorHandle* m_targetEnemyHandle = nullptr;
	MarineAI(ActorHandle* handle, Map* map);
	void DamagedBy(ActorHandle* handle);
	void Update();
};