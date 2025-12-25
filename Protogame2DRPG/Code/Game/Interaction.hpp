#pragma once
#include "Engine/Core/EngineCommon.hpp"
class Object;

class Interaction {
public:
    Interaction(){}
	Object* m_owner;
	virtual void OnCreate()=0;
	virtual void OnDestroy()=0;
	
	virtual void OnTick(float deltaSeconds) = 0;
	virtual void OnInteraction(Object* other) = 0;
};