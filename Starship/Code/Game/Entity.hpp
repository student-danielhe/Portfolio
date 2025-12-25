#pragma once
#include "App.hpp"

class Game;

class Entity {
public:
	
	Vec2 m_position;              // : the Entity’s 2D(x, y) Cartesian origin / center location, in world space
	Vec2 m_verlocity;              // : the Entity’s linear 2D(x, y) velocity, in world units per second
	float m_orientationDegrees=0; // : its forward angle, in degrees(counter - clockwise from + x / east)
	float m_angularVelocity=0;    // : the Entity’s signed angular velocity(spin rate), in degrees per second
	float m_physicsRadius=4;      // : the Entity’s(inner, conservative) disc - radius for all physics purposes
	float m_cosmeticRadius=5;     // : the Entity’s(outer, liberal) disc - radius that encloses all of its vertexes
	int m_health = 1;             // (int) : how many “hits” the entity can sustain before dying
	bool m_isDead = false;        // : whether the Entity is “dead” in the game; affects entity and game logic
	bool m_isGarbage=false;       // : whether the Entity should be deleted at the end of Game::Update()
	int m_damage = 5;
	Game* m_owner = nullptr;
public:
	Entity(Game* owner, Vec2 const& pos, float ordientationDegree);
	Entity();
	void Update();
	void Render();
	bool IsAlive();
	bool IsOffScreen();
	bool Collide(Entity* target);
	Vec2 GetFowardNormal();
	void Dies();

};