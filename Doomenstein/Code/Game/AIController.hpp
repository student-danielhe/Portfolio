#pragma once
#include "Actor.hpp"
#include "Controller.hpp"
class DemonAIController :public Controller{
public:
	float m_aggroRange = 10.f;
	ActorHandle* m_targetEnemyHandle = nullptr;
	ActorHandle* m_followAllyHandle = nullptr;
	bool m_inParty = false;
	bool m_advancedPathFinding=true;
	DemonAIController(ActorHandle* handle, Map* map);
	void DamagedBy(ActorHandle* handle);
	void Update();
	static std::vector<ActorHandle*> g_party;
	static void UpdateParty();
	static void GlobalAggro(ActorHandle* target);
};