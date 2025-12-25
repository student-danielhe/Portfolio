#pragma once
#include "GameCommon.hpp"
#include "CubeDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Timer.hpp"
class Map;
class Cube {
public:
    int m_currentHealth = 10;
    int m_maxHealth = 10;
    int m_manaCost = 10;
    bool m_dead=false;
    IntVec2 m_Position;
    FactionType m_faction = FactionType::NEUTRAL;
    CubeDefinition const& m_def;
    Map* m_owner;

    //Abilities------------------------------------------
    Vec2 m_melee;
    float m_charging = 0.f;;
    bool m_leader = false;;
    bool m_burrowed=false;
	bool m_climbing = false;
	bool m_flying = false;
	bool m_liquid = false;
	int m_retaliate = 0;
	int m_projectile = 0;
	int m_regen = 0;
	int m_growth = 0;
	int m_temporary = 0;

    //Abilities Timers------------------------------------------
    Timer* m_meleeTimer = nullptr;
    Timer* m_chargeTimer = nullptr;
    Timer* m_liquidTimer = nullptr;
    Timer* m_drownTimer = nullptr;
    Timer* m_regenTimer = nullptr;
    Timer* m_growTimer = nullptr;
    Timer* m_temporaryTimer = nullptr;

    Timer* m_specialTimer = nullptr;
    Timer* m_specialTimer2 = nullptr;

    std::vector<std::string> m_description;

    Cube(CubeDefinition const& def,IntVec2 pos, FactionType faction, Map* owner);
    void InitializeSpecialTimer();
    void CreateDescriptions();

    void PhyscisUpdate();
    void UpdateGravity();
	void Render();

    //Gameplay General
    void Damage(int value, Cube* other);
    void Heal(int value, Cube* other);

    //Gameplay Abilities
    void MeleeUpdate();
    void ChargingUpDate();
    void LiquidUpdate();
    void RegenAndGrowUpdate();
    void TemporaryUpdate();

    void OnSpawn();
    void OnDeath();
    void SpecialAbilityUpdate();

    IntVec2 GetFront();

    std::vector<bool> CheckAdjacentBlockLight();
};