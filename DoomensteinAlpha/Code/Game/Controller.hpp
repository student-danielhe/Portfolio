#pragma once
#include "ActorHandle.hpp"
class Map;
class Actor;
class Controller {
public:
	ActorHandle* m_ownerHandle = nullptr;
	Map* m_map = nullptr;
	Controller(ActorHandle* handle, Map* map);
	void Possess(ActorHandle* newActor);
	Actor* GetActor()const;
};