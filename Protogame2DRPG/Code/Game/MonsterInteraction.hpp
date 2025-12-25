#pragma once
#include "Interaction.hpp"
class Timer;
class MonsterInteraction : public Interaction {
public:
    MonsterInteraction();
    Timer* m_movementTimer = nullptr;
	bool m_aggro = false;
	int m_monsterIndex =0;
	int m_step = 0;
	virtual void OnInteraction(Object* other);
	virtual void OnTick(float deltaSeconds);
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
};