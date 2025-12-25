#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Mat44.hpp"
#include "ActorHandle.hpp"
#include "ActorDefinition.hpp"
#include "Weapon.hpp"
#include "Controller.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/AnimationGroupDefinition.hpp"
#include "Engine/Renderer/Shader.hpp"

class Map;
class DemonAIController;
class MarineAI;
class Actor {
public:
	SoundID m_hurtSound = 0;
	SoundID m_deathSound = 0;
	SoundID m_meat = 0;
	SoundPlaybackID m_meatPlayback =0;
	ActorHandle* m_handle = nullptr;
	ActorDefinition* m_definition = nullptr;
	Vec3 m_bottomPosition = Vec3(0.f, 0.f, 0.f);
	EulerAngles m_orientation = EulerAngles(0.f,0.f,0.f);
	Vec3 m_velocity = Vec3(0.f, 0.f, 0.f);
	Vec3 m_acceleration = Vec3(0.f, 0.f, 0.f);
	std::vector<Vertex_PCU> m_vertexes;
	std::vector<Weapon*> m_weapons;
	int m_currentWeaponIndex = 0;
	ActorHandle* m_owner = nullptr;
	bool m_dead = false;
	bool m_destroyed = false;
	bool m_running = false;
	int m_currentHealth = 0;
	Map* m_map = nullptr;
	bool m_blocking = false;


	//controller
	Controller* m_controller = nullptr;
	DemonAIController* m_AIController = nullptr;
	MarineAI* m_MarineAI = nullptr;
	//AI controller
	Timer* m_deathTimer = nullptr;
	Rgba8 m_color = Rgba8::WHITE;
	SpriteSheet* m_sprites = nullptr;
	std::string m_currentAction = "Walk";
	std::vector<AnimationGroupDefinition*> m_animGroups;
	Clock* m_animClock = nullptr;

	Actor(ActorDefinition* def, ActorHandle* handle);
	~Actor();
	void InitializeAnimGroup();
	void UpdatePhysics(float deltaSeconds);
	void Damage(int damageNumber);
	void Damage(FloatRange damageNumber);
	void AddForce(Vec3 force);
	void AddImpulse(Vec3 impulse);
	void OnCollide(Actor& other);
	void OnPossessed(Controller* newController);
	void OnUnpossessed();
	void MoveInDirection(Vec3 direction,float speed);
	void TurnInDirection(float degree, float angularSpeed);
	void Attack();
	void EquipWeapon(int weaponIndex);
	void Render(PlayerController* player);
	bool CanAggroOnto(ActorHandle* other);
	Mat44 GetModelMatrix();
	Mat44 GetBillboardMatrix(PlayerController* player);
	bool OpposingFaction(ActorHandle* other);
	SoundID GetSoundEffect(std::string name);
	Vec3 GetCameraInActorSpace(PlayerController* player);
	SpriteAnimDefinition* GetCurrentDirection(PlayerController* player);
	void TryChangeAction(std::string actionName);
	void UpdateAnimClock();
	std::string GetSoundPath(std::string name);
	Vec3 GetForwardNormal();
	void RenderHealthBar(PlayerController* player);
};