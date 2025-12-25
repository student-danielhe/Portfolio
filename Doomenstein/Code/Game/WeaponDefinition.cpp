#include "WeaponDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
std::vector<WeaponDefinition*> WeaponDefinition::s_definitions = std::vector<WeaponDefinition*>(0);
void WeaponDefinition::InitializeDefinition(const char* path) {
	XmlDocument weaponDoc;
	XmlResult result = weaponDoc.LoadFile(path);
	UNUSED(result);
	XmlElement* rootElement = weaponDoc.RootElement();
	XmlElement* weapDefinitionElement = rootElement->FirstChildElement();
	while (weapDefinitionElement != nullptr) {
		std::string elementName = weapDefinitionElement->Name();
		WeaponDefinition* weapDefintion = new WeaponDefinition();
		weapDefintion->LoadFromXmlElement(*weapDefinitionElement);
		XmlElement* hudElement = weapDefinitionElement->FirstChildElement("HUD");
		if(hudElement)
		    weapDefintion->LoadHudFromXmlElement(hudElement);
		XmlElement* soundElement = weapDefinitionElement->FirstChildElement("Sounds");
		if(soundElement)
		    weapDefintion->LoadSoundsFromXmlElement(soundElement);
		WeaponDefinition::s_definitions.push_back(weapDefintion);
		weapDefinitionElement = weapDefinitionElement->NextSiblingElement();
	}
}
bool WeaponDefinition::LoadFromXmlElement(const XmlElement& element) {
	m_name = ParseXmlAttribute(element, "name", "");
	m_refireTime = ParseXmlAttribute(element, "refireTime", 0.f);
	m_rayCount = ParseXmlAttribute(element, "rayCount", 0);
	m_rayCone = ParseXmlAttribute(element, "rayCone", 0.f);
	m_rayRange = ParseXmlAttribute(element, "rayRange", 0.f);
	m_rayImpulse = ParseXmlAttribute(element, "rayImpulse", 0.f);
	m_rayDamage = ParseXmlAttribute(element, "rayDamage", FloatRange());
	m_projectileCount = ParseXmlAttribute(element, "projectileCount", 0);
	m_projectileActor = ParseXmlAttribute(element, "projectileActor", "");
	m_projectileCone = ParseXmlAttribute(element, "projectileCone", 0.f);
	m_projectileSpeed = ParseXmlAttribute(element, "projectileSpeed", 0.f);
	m_meleeCount = ParseXmlAttribute(element, "meleeCount", 0);
	m_meleeArc = ParseXmlAttribute(element, "meleeArc", 0.f);
	m_meleeRange = ParseXmlAttribute(element, "meleeRange", 0.f);
	m_meleeDamage = ParseXmlAttribute(element, "meleeDamage", FloatRange());
	m_meleeImpulse = ParseXmlAttribute(element, "meleeImpulse", 0.f);
	return true;
}

bool WeaponDefinition::LoadHudFromXmlElement(XmlElement* element) {
	m_HUD = new HUD_Weapon();
	m_HUD->m_shaderPath = ParseXmlAttribute(*element, "shader", "Default");
	m_HUD->m_TexturePath = ParseXmlAttribute(*element, "baseTexture", "");
	m_HUD->m_reticleTexturePath = ParseXmlAttribute(*element, "reticleTexture", "");
	m_HUD->m_reticleSize = ParseXmlAttribute(*element, "reticleSize", Vec2(1.f,1.f));
	m_HUD->m_spriteSize = ParseXmlAttribute(*element, "spriteSize", Vec2(1.f,1.f));
	m_HUD->m_spritePivot = ParseXmlAttribute(*element, "spritePivot", Vec2(.5f,0.f));
	XmlElement* animElement = element->FirstChildElement("Animation");
	while (animElement != nullptr) {
		m_HUD->m_anims.push_back(LoadAnimFromXmlElement(*animElement));
		animElement = animElement->NextSiblingElement();
	}
	return true;
}
Animation_Wepaon* WeaponDefinition::LoadAnimFromXmlElement(XmlElement const& element) {
	Animation_Wepaon* anim = new Animation_Wepaon();
	anim->m_name = ParseXmlAttribute(element, "name", "Idle");
	anim->m_shaderPath = ParseXmlAttribute(element, "shader", "Default");
	anim->m_spriteSheetPath = ParseXmlAttribute(element, "spriteSheet", "Default");
	anim->m_cellCount = ParseXmlAttribute(element, "cellCount", Vec2(1.f,1.f));
	anim->m_secoundsPerFrame = ParseXmlAttribute(element, "secondsPerFrame", 1.f);
	anim->m_startFrame = ParseXmlAttribute(element, "startFrame", 0);
	anim->m_endFrame = ParseXmlAttribute(element, "endFrame", 0);
	return anim;
}
bool WeaponDefinition::LoadSoundsFromXmlElement(XmlElement* element) {
	XmlElement* soundElement = element->FirstChildElement("Sound");
	while (soundElement != nullptr) {
		Sound_Weapon* sound = new Sound_Weapon();
		sound->m_soundName = ParseXmlAttribute(*soundElement, "sound", "");
		sound->m_soundPath = ParseXmlAttribute(*soundElement, "name", "");
		m_sounds.push_back(sound);
		soundElement = soundElement->NextSiblingElement();
	}
	return true;
}

WeaponDefinition* WeaponDefinition::GetWeaponDefinitionByName(std::string name) {
	for (WeaponDefinition* def : WeaponDefinition::s_definitions) {
		if (def->m_name.compare(name) == 0) {
			return def;
		}
	}
	return nullptr;
}