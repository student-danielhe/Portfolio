#pragma once
#include "App.hpp"
#include"GameCommon.hpp"
#include<vector>
#include "Engine/Core/Vertex_PCU.hpp"
class Map;
enum Faction {
	FACTION_NETRUAL,
	FACTION_GOOD,
	FACTION_EVIL,
	NUM_FACTION
};
enum EntityType {
	ENTITYTYPE_GOOD_PLAYER,
	ENTITYTYPE_EVIL_SCORPIO,
	ENTITYTYPE_EVIL_LEO,
	ENTITYTYPE_EVIL_ARIES,
	ENTITYTYPE_GOOD_BULLET,
	ENTITYTYPE_EVIL_BULLET,
	NUM_ENTITYTYPE
};

class Entity {
	friend class Map;
	public:
		Vec2 m_position;              // : the Entity’s 2D(x, y) Cartesian origin / center location, in world space
		Vec2 m_verlocity;              // : the Entity’s linear 2D(x, y) velocity, in world units per second
		float m_orientationDegrees = 0; // : its forward angle, in degrees(counter - clockwise from + x / east)
		float m_angularVelocity = 0;    // : the Entity’s signed angular velocity(spin rate), in degrees per second
		float m_physicsRadius = 4;      // : the Entity’s(inner, conservative) disc - radius for all physics purposes
		float m_cosmeticRadius = 5;     // : the Entity’s(outer, liberal) disc - radius that encloses all of its vertexes
		int m_health = 1;             // (int) : how many “hits” the entity can sustain before dying
		bool m_isDead = false;        // : whether the Entity is “dead” in the game; affects entity and game logic
		bool m_isGarbage = false;       // : whether the Entity should be deleted at the end of Game::Update()
		Map* m_owner = nullptr;
		std::vector<Vertex_PCU> m_localVertexArray;
		Faction m_faction;
		float m_currentFireCooldown;
		EntityType m_type;
		bool m_pushes = true;
		bool m_pushable = true;
		bool m_pushedByWall = true;
    protected:
		Entity(Map* owner, Vec2 const& pos, float ordientationDegree);
		Entity();
		virtual void Update(float deltaSecond);
		virtual void Render(std::vector<Vertex_PCU>& verts);
		virtual bool IsAlive();
		virtual bool IsOffScreen();
		virtual bool Collide(Entity* target);
		virtual Vec2 GetFowardNormal();
		virtual void Dies();
		void PushOutOfWalls(std::vector<AABB2*> walls);
		void DebugRender()const;
		void GlobalVertexArray(std::vector<Vertex_PCU> &result);
		virtual void RenderBase(std::vector<Vertex_PCU>& verts) { UNUSED(verts); }
		virtual void RenderLaser(std::vector<Vertex_PCU>& verts) { UNUSED(verts); }



};
typedef std::vector<Entity*> EntityList;