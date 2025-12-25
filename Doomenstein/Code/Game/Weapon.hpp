#pragma once
#include "WeaponDefinition.hpp"
#include "ActorHandle.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/AnimationGroupDefinition.hpp"
#include <string>
#include <map>
class Weapon {
public:
	ActorHandle* m_owner= nullptr;
	Timer* m_timer = nullptr;
	WeaponDefinition* m_definition;
	std::string m_currentAction = "Idle";
	Clock* m_animClock = nullptr;
	std::map<std::string, SpriteAnimDefinition*> m_animationMap;
	Weapon(WeaponDefinition* def, ActorHandle* owner);
	~Weapon();
	void Fire();
	float GetRandomDirectionInCone(float cone);
	void InitializeAnimGroup();
	std::string GetSoundPath(std::string name);
};