#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <string>
#include <vector>
#include "Engine/Core/XmlUtils.hpp"
struct Animation_Wepaon {
	std::string m_name = "Idle";
	std::string m_shaderPath = "Default";
	std::string m_spriteSheetPath = "Default";
	Vec2 m_cellCount = Vec2(1.f, 1.f);
	float m_secoundsPerFrame = 1.f;
	int m_startFrame = 0;
	int m_endFrame = 0;
};
struct HUD_Weapon {
	std::string m_shaderPath = "Default";
	std::string m_TexturePath = "Default";
	std::string m_reticleTexturePath = "Default";
	Vec2 m_reticleSize = Vec2(1.f, 1.f);
	Vec2 m_spriteSize = Vec2(1.f, 1.f);
	Vec2 m_spritePivot = Vec2(.5f, 0.f);
	std::vector<Animation_Wepaon*> m_anims;
};


struct Sound_Weapon {
	std::string m_soundName = "";
	std::string m_soundPath = "";
};
struct WeaponDefinition {
	HUD_Weapon* m_HUD;
	std::vector<Sound_Weapon*> m_sounds;
	std::string m_name = "";
	float m_refireTime = 0.f;
	int m_rayCount = 0;
	float m_rayCone = 0;
	float m_rayRange = 0.f;
	FloatRange m_rayDamage = FloatRange(0.f, 0.f);
	float m_rayImpulse = 0.f;
	int m_projectileCount = 0;
	float m_projectileCone = 0.f;
	float m_projectileSpeed = 0.f;
	std::string m_projectileActor = "";
	int m_meleeCount = 0;
	float m_meleeRange = 0.f;
	float m_meleeArc = 0.f;
	FloatRange m_meleeDamage = FloatRange(0.f, 0.f);
	float m_meleeImpulse = 0.f;

	static std::vector<WeaponDefinition*> s_definitions;

	static void InitializeDefinition(const char* path);
	bool LoadFromXmlElement(const XmlElement& element);
	bool LoadHudFromXmlElement(XmlElement* element);
	Animation_Wepaon* LoadAnimFromXmlElement(XmlElement const& element);
	bool LoadSoundsFromXmlElement(XmlElement* element);
	static WeaponDefinition* GetWeaponDefinitionByName(std::string name);
};