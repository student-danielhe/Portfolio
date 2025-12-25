#include "ActorDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
std::vector<ActorDefinition*> ActorDefinition::s_actorDefinitions = std::vector<ActorDefinition*>(0);
std::vector<ActorDefinition*> ActorDefinition::s_projDefinitions = std::vector<ActorDefinition*>(0);

void ActorDefinition::InitializeDefinition(const char* actorDefPath, const char* projDefPath) {

	XmlDocument actorDoc;
	XmlResult result = actorDoc.LoadFile(actorDefPath);
	UNUSED(result);
	XmlElement* rootElement = actorDoc.RootElement();
	XmlElement* ActorDefinitionElement = rootElement->FirstChildElement("ActorDefinition");
	while (ActorDefinitionElement != nullptr) {
		std::string elementName = ActorDefinitionElement->Name();
		ActorDefinition* actorDefinition = new ActorDefinition();
		actorDefinition->m_base = new Base_Actor();
		actorDefinition->m_collision = new Collision_Actor();
		actorDefinition->m_physics = new Physics_Actor();
		actorDefinition->m_camera = new Camera_Actor();
		actorDefinition->m_ai = new AI_Actor();
		actorDefinition->m_weapon = new Weapon_Actor();
		actorDefinition->m_visual = new Visual();
		actorDefinition->LoadActorDefinition(ActorDefinitionElement);
		ActorDefinition::s_actorDefinitions.push_back(actorDefinition);
		ActorDefinitionElement = ActorDefinitionElement->NextSiblingElement("ActorDefinition");
	}
	XmlDocument projectileDoc;
	result = projectileDoc.LoadFile(projDefPath);
	UNUSED(result);
	rootElement = projectileDoc.RootElement();
	XmlElement* projDefinitionElement = rootElement->FirstChildElement("ActorDefinition");
	while (projDefinitionElement != nullptr) {
		std::string elementName = projDefinitionElement->Name();
		ActorDefinition* projDefinition = new ActorDefinition();
		projDefinition->m_base = new Base_Actor();
		projDefinition->m_collision = new Collision_Actor();
		projDefinition->m_physics = new Physics_Actor();
		projDefinition->m_camera = new Camera_Actor();
		projDefinition->m_ai = new AI_Actor();
		projDefinition->m_weapon = new Weapon_Actor();
		projDefinition->m_visual = new Visual();
		projDefinition->LoadProjectileDefinition(projDefinitionElement);
		ActorDefinition::s_projDefinitions.push_back(projDefinition);
		projDefinitionElement = projDefinitionElement->NextSiblingElement("ActorDefinition");
	}
}
void ActorDefinition::LoadActorDefinition(XmlElement* element) {
	XmlElement* baseElement = element;
	if(baseElement!=nullptr)
	    LoadBaseFromXmlElement(*baseElement);
	XmlElement* collisionElement = element->FirstChildElement("Collision");
	if(collisionElement!=nullptr)
	    LoadCollisionFromXmlElement(*collisionElement);
	XmlElement* physicsElement = element->FirstChildElement("Physics");
	if(physicsElement!=nullptr)
	    LoadPhysicsFromXmlElement(*physicsElement);
	XmlElement* cameraElement = element->FirstChildElement("Camera");
	if(cameraElement!=nullptr)
	    LoadCameraFromXmlElement(*cameraElement);
	XmlElement* aiElement = element->FirstChildElement("AI");
	if (aiElement != nullptr) 
		LoadAIFromXmlElement(*aiElement);
	XmlElement* weaponElement = element->FirstChildElement("Inventory");
	if(weaponElement!=nullptr)
	    LoadWeaponFromXmlElement(weaponElement);
	XmlElement* visualElement = element->FirstChildElement("Visuals");
	if (visualElement != nullptr) 
		LoadVisualFromXmlElement(visualElement);
	XmlElement* soundElement = element->FirstChildElement("Sounds");
	if(soundElement!=nullptr)
	    LoadSoundsFromXmlElement(soundElement);
}
void ActorDefinition::LoadProjectileDefinition(XmlElement* element) {
	XmlElement* baseElement = element;
	if (baseElement != nullptr)
		LoadBaseFromXmlElement(*baseElement);
	XmlElement* collisionElement = element->FirstChildElement("Collision");
	if (collisionElement != nullptr)
		LoadCollisionFromXmlElement(*collisionElement);
	XmlElement* physicsElement = element->FirstChildElement("Physics");
	if (physicsElement != nullptr)
		LoadPhysicsFromXmlElement(*physicsElement);
	XmlElement* visualElement = element->FirstChildElement("Visuals");
	if (visualElement != nullptr)
		LoadVisualFromXmlElement(visualElement);
	XmlElement* soundElement = element->FirstChildElement("Sounds");
	if (soundElement != nullptr)
		LoadSoundsFromXmlElement(soundElement);
}
bool ActorDefinition::LoadBaseFromXmlElement(const XmlElement& element) {
	m_base->m_name = ParseXmlAttribute(element, "name", "");
	m_base->m_canBePossessed = ParseXmlAttribute(element, "canBePossessed", false);
	m_base->m_corpseLifeTime = ParseXmlAttribute(element, "corpseLifetime", 0.f);
	m_base->m_health = ParseXmlAttribute(element, "health", 1);
	m_base->m_visible = ParseXmlAttribute(element, "visible", false);
	m_base->m_dieOnSpawn = ParseXmlAttribute(element, "dieOnSpawn", false);
	std::string faction = ParseXmlAttribute(element, "faction", "Neutral");
	if (faction.compare("Marine") == 0) {
		m_base->m_faction = Faction_Actor::MARINE;
	}
	else if (faction.compare("Demon") == 0) {
		m_base->m_faction = Faction_Actor::DEMON;
	}
	else {
		m_base->m_faction = Faction_Actor::NEUTRAL;
	}
	return true;
}
bool ActorDefinition::LoadCollisionFromXmlElement(const XmlElement& element) {

	m_collision->m_radius = ParseXmlAttribute(element, "radius", 1.f);
	m_collision->m_height = ParseXmlAttribute(element, "height", 1.f);
	m_collision->m_collideWithWorld = ParseXmlAttribute(element, "collidesWithWorld", false);
	m_collision->m_collideWithActor = ParseXmlAttribute(element, "collidesWithActors", false);
	m_collision->m_damageOnCollide = ParseXmlAttribute(element, "damageOnCollide", FloatRange());
	m_collision->m_impulseOnCollide = ParseXmlAttribute(element, "impulseOnCollide", 0.f);
	m_collision->m_dieOnCollide = ParseXmlAttribute(element, "dieOnCollide", false);
	return true;
}
bool ActorDefinition::LoadPhysicsFromXmlElement(const XmlElement& element) {
	m_physics->m_simulated = ParseXmlAttribute(element, "simulated", false);
	m_physics->m_walkSpeed = ParseXmlAttribute(element, "walkSpeed", 0.f);
	m_physics->m_runSpeed = ParseXmlAttribute(element, "runSpeed", 0.f);
	m_physics->m_turnSpeed = ParseXmlAttribute(element, "turnSpeed", 0.f);
	m_physics->m_drag = ParseXmlAttribute(element, "drag", 0.f);
	m_physics->m_flying = ParseXmlAttribute(element, "flying", false);
	return true;
}
bool ActorDefinition::LoadCameraFromXmlElement(const XmlElement& element) {
	m_camera->m_eyeHeight = ParseXmlAttribute(element, "eyeHeight", 0.f);
	m_camera->m_cameraFOVDeg = ParseXmlAttribute(element, "cameraFOV", 0.f);
	return true;
}
bool ActorDefinition::LoadAIFromXmlElement(const XmlElement& element) {
	m_ai->m_aiEnabled = ParseXmlAttribute(element, "aiEnabled", false);
	m_ai->m_sightRadius = ParseXmlAttribute(element, "sightRadius", 0.f);
	m_ai->m_sightAngle = ParseXmlAttribute(element, "sightAngle", 60.f);
	return true;
}
bool ActorDefinition::LoadWeaponFromXmlElement(XmlElement* element) {
	XmlElement* weaponElement = element->FirstChildElement("Weapon");
	while (weaponElement != nullptr) {
		std::string name = ParseXmlAttribute(*weaponElement, "name", "");
		m_weapon->m_name.push_back(name);
		weaponElement = weaponElement->NextSiblingElement("Weapon");
	}
	return true;
}
bool ActorDefinition::LoadVisualFromXmlElement(XmlElement* element) {
	const Vec2 vec(0.f,0.f);
	m_visual->m_size = ParseXmlAttribute(*element, "size", Vec2());
	m_visual->m_pivot = ParseXmlAttribute(*element, "pivot", Vec2());
	std::string billBoardName = ParseXmlAttribute(*element, "billboardType", "None");
	if (billBoardName.compare("None") == 0) {
		m_visual->m_billboardType = BillboardType::NONE;
	}
	else if (billBoardName.compare("WorldUpFacing") == 0) {
		m_visual->m_billboardType = BillboardType::WORLD_UP_FACING;
	}
	else if (billBoardName.compare("WorldUpOpposing") == 0) {
		m_visual->m_billboardType = BillboardType::WORLD_UP_OPPOSING;
	}
	else if (billBoardName.compare("FullFacing") == 0) {
		m_visual->m_billboardType = BillboardType::FULL_FACING;
	}
	else if (billBoardName.compare("FullOpposing") == 0) {
		m_visual->m_billboardType = BillboardType::FULL_OPPPOSING;
	}
	m_visual->m_renderLit = ParseXmlAttribute(*element, "renderLit", false);
	m_visual->m_renderRounded = ParseXmlAttribute(*element, "renderRounded", false);
	m_visual->m_shaderPath = ParseXmlAttribute(*element, "shader", "Default");
	m_visual->m_spriteSheetPath = ParseXmlAttribute(*element, "spriteSheet", "Default");
	m_visual->m_cellCount = ParseXmlAttribute(*element, "cellCount", Vec2(1.f, 1.f));
	XmlElement* animGroupElement = element->FirstChildElement("AnimationGroup");
	while (animGroupElement != nullptr) {
		m_visual->m_animationGroup.push_back(LoadAnimGroupsFromXmlElement(animGroupElement));
		animGroupElement = animGroupElement->NextSiblingElement("AnimationGroup");
	}
	return true;
}

AnimationGroup* ActorDefinition::LoadAnimGroupsFromXmlElement(XmlElement* element) {
	AnimationGroup* animGroup = new AnimationGroup();
	animGroup->m_name = ParseXmlAttribute(*element, "name", "");
	animGroup->m_scaleBySpeed = ParseXmlAttribute(*element, "scaleBySpeed", false);
	animGroup->m_secondsPerFrame = ParseXmlAttribute(*element, "secondsPerFrame", 1.f);
	std::string playBackTXT = ParseXmlAttribute(*element, "playbackMode", "Once");
	if (playBackTXT.compare("Loop") == 0) {
		animGroup->m_playbackMode = SpriteAnimPlaybackType::LOOP;
	}
	else if (playBackTXT.compare("Once") == 0) {
		animGroup->m_playbackMode = SpriteAnimPlaybackType::ONCE;
	}
	else {
		animGroup->m_playbackMode = SpriteAnimPlaybackType::PINGPONG;
	}
	XmlElement* DirectionElement = element->FirstChildElement("Direction");
	while (DirectionElement != nullptr) {
		animGroup->m_directions.push_back(LoadDirectionsFromXmlElement(DirectionElement));
		DirectionElement = DirectionElement->NextSiblingElement("Direction");
	}
	return animGroup;
}
Direction* ActorDefinition::LoadDirectionsFromXmlElement(XmlElement* element) {
	Direction* direction = new Direction();
	direction->m_vector = ParseXmlAttribute(*element, "vector", Vec3(1.f,0.f,0.f));
	XmlElement* animElement = element->FirstChildElement("Animation");
	direction->m_animation = LoadAnimFromXmlElement(*animElement);
	return direction;
}
Animation_Actor* ActorDefinition::LoadAnimFromXmlElement(const XmlElement& element) {
	Animation_Actor* anim = new Animation_Actor();
	anim->m_startFrame = ParseXmlAttribute(element, "startFrame", 0);
	anim->m_endFrame = ParseXmlAttribute(element, "endFrame", 0);
	return anim;
}
bool ActorDefinition::LoadSoundsFromXmlElement(XmlElement* element) {
	XmlElement* soundElement = element->FirstChildElement("Sound");
	while (soundElement != nullptr) {
		Sound* sound = new Sound();
		sound->m_sound = ParseXmlAttribute(*soundElement, "sound", "");
		sound->m_path = ParseXmlAttribute(*soundElement, "name", "");
		m_sound.push_back(sound);
		soundElement = soundElement->NextSiblingElement();
	}
	return true;
}

ActorDefinition* ActorDefinition::GetActorDefinitionByName(std::string name) {
	for (int i = 0; i < s_actorDefinitions.size();i++) {
		ActorDefinition* def = s_actorDefinitions[i];
		if (def&&def->m_base->m_name.compare(name) == 0) {
			return def;
		}
	}
	for (int i = 0; i < s_projDefinitions.size(); i++) {
		ActorDefinition* def = s_projDefinitions[i];
		if (def&&def->m_base->m_name.compare(name) == 0) {
			return def;
		}
	}
	return nullptr;
}