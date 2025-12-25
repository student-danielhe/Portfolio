#pragma once
#include "Interaction.hpp"
class EnvObstacleInteraction : public Interaction {
public:
	EnvObstacleInteraction();
	virtual void OnInteraction(Object* other) {UNUSED(other); }
	virtual void OnTick(float deltaSeconds) { UNUSED(deltaSeconds); }
	virtual void OnCreate();
	virtual void OnDestroy() {}
};