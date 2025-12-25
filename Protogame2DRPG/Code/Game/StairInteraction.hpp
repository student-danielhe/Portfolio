#pragma once
#include "Interaction.hpp"
#include "Engine/Math/Vec2.hpp"
class Map;
class StairInteraction : public Interaction {
public:
    Map* m_targetMap = nullptr;
    IntVec2 m_targetPos;

    StairInteraction(Map* targetMap, IntVec2 targetPos);
	virtual void OnInteraction(Object* other);
    virtual void OnTick(float deltaSeconds) {UNUSED(deltaSeconds);};
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
};