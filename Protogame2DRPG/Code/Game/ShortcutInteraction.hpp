#pragma once
#include "Interaction.hpp"
class ShortcutInteraction : public Interaction {
public:
	ShortcutInteraction();
	virtual void OnInteraction(Object* other);
	virtual void OnTick(float deltaSeconds) {UNUSED(deltaSeconds);}
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
};