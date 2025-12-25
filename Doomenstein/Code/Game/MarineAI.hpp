#pragma once
#include "Actor.hpp"
#include "Controller.hpp"
class MarineAI :public Controller {
public:
	float m_attackRenge = 6.5f;
	float m_runningDetectionRange = 10.f;
	ActorHandle* m_targetEnemyHandle = nullptr;
	bool m_advancedPathFinding = true;
	MarineAI(ActorHandle* handle, Map* map);
	void DamagedBy(ActorHandle* handle);
	void Update();
};