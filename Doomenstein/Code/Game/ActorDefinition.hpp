#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <string>
#include "WeaponDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <vector>
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
enum class Faction_Actor {
	NEUTRAL,
	MARINE,
	DEMON,
	COUNT,
};
struct Base_Actor {
	std::string m_name = "";
	bool m_visible = false;
	int m_health = 1;
	float m_corpseLifeTime = 0.f;
	Faction_Actor m_faction = Faction_Actor::NEUTRAL;
	bool m_canBePossessed = false;
	bool m_dieOnSpawn = false;
};
struct Collision_Actor {
	float m_radius = 1.f;
	float m_height = 1.f;
	bool m_collideWithWorld = false;
	bool m_collideWithActor = false;
	bool m_dieOnCollide = false;
	FloatRange m_damageOnCollide = FloatRange(0.f, 0.f);
	float m_impulseOnCollide = 0.f;
};
struct Physics_Actor {
	bool m_simulated = false;
	bool m_flying = false;
	float m_walkSpeed = 0.f;
	float m_runSpeed = 0.f;
	float m_drag = 0.f;
	float m_turnSpeed = 0.f;
};
struct Camera_Actor {
	float m_eyeHeight = 0;
	float m_cameraFOVDeg = 60.f;
};
struct AI_Actor {
	bool m_aiEnabled = false;
	float m_sightRadius = 0.f;
	float m_sightAngle = 0.f;
};
struct Weapon_Actor {
	std::vector<std::string> m_name;
};
struct Animation_Actor {
	int m_startFrame = 0;
	int m_endFrame = 0;
};
struct Direction {
	Vec3 m_vector = Vec3(1.f, 0.f, 0.f);
	Animation_Actor* m_animation = nullptr;
};
struct AnimationGroup {
	std::string m_name = "";
	bool m_scaleBySpeed = false;
	float m_secondsPerFrame = 1.f;
	std::vector<Direction*> m_directions;
	SpriteAnimPlaybackType m_playbackMode = SpriteAnimPlaybackType::ONCE;
};
struct Sound {
	std::string m_sound = "";
	std::string m_path = "";
};
struct Visual {
	Vec2 m_size = Vec2(0.f, 0.f);
	Vec2 m_pivot = Vec2(0.5f, 0.5f);
	BillboardType m_billboardType = BillboardType::NONE;
	bool m_renderLit = false;
	bool m_renderRounded = false;
	std::string m_shaderPath = "Default";
	std::string m_spriteSheetPath = "Default";
	Vec2 m_cellCount = Vec2(1.f, 1.f);

	std::vector<AnimationGroup*> m_animationGroup;
	

};


class ActorDefinition {
public:
	Visual* m_visual = nullptr;
	Base_Actor*      m_base     = nullptr;
	Collision_Actor* m_collision= nullptr;
	Physics_Actor*   m_physics  = nullptr;
	Camera_Actor*    m_camera   = nullptr;
	AI_Actor*        m_ai       = nullptr;
	Weapon_Actor*    m_weapon   = nullptr;
	std::vector<Sound*> m_sound;
	static std::vector<ActorDefinition*> s_actorDefinitions;
	static std::vector<ActorDefinition*> s_projDefinitions;
	
	static void InitializeDefinition(const char* actorDefPath, const char* projDefPath);
	void LoadActorDefinition(XmlElement* element);
	void LoadProjectileDefinition(XmlElement* element);
	bool LoadBaseFromXmlElement(const XmlElement& element);
	bool LoadCollisionFromXmlElement(const XmlElement& element);
	bool LoadPhysicsFromXmlElement(const XmlElement& element);
	bool LoadCameraFromXmlElement(const XmlElement& element);
	bool LoadAIFromXmlElement(const XmlElement& element);
	bool LoadWeaponFromXmlElement(XmlElement* element);
	bool LoadVisualFromXmlElement(XmlElement* element);
	AnimationGroup* LoadAnimGroupsFromXmlElement(XmlElement* element);
	Direction* LoadDirectionsFromXmlElement(XmlElement* element);
	Animation_Actor* LoadAnimFromXmlElement(const XmlElement& element);
	bool LoadSoundsFromXmlElement(XmlElement* element);
	static ActorDefinition* GetActorDefinitionByName(std::string name);
};