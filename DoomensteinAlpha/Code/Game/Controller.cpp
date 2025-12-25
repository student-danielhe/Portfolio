#include "Controller.hpp"
#include "Map.hpp"
#include "Actor.hpp"
Controller::Controller(ActorHandle* handle, Map* map)
:m_ownerHandle(handle)
,m_map(map){}

void   Controller::Possess(ActorHandle* newActor) {
	if (GetActor() != nullptr) {
		GetActor()->OnUnpossessed();
	}
	if (m_map->GetActorByHandle(*newActor) != nullptr) {
		m_map->GetActorByHandle(*newActor)->OnPossessed(this);
	}
}
Actor* Controller::GetActor()const {
	if (m_ownerHandle == nullptr) {
		return nullptr;
	}
	return m_map->GetActorByHandle(*m_ownerHandle);
}