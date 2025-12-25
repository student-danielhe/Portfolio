#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Timer.hpp"
#include "ObjectDefinition.hpp"
class Interaction;
class Map;
enum class SpriteDirection {
	SOUTH,
	WEST,
	EAST,
	NORTH
};
class Object {
public:
	std::vector<bool> m_switch;
	std::vector<int> m_value;
	Map* m_map=nullptr;
	IntVec2 m_position;
	ObjectDefinition* m_def;
	Interaction* m_interaction=nullptr;
	SpriteDirection m_currentDirection = SpriteDirection::SOUTH;
	bool m_isDestroyed = false;

	Object(ObjectDefinition* def, Map* map, IntVec2 pos, Interaction* interaction = nullptr);

	void Interact(Object* other);
	void Render();
	void Update(float deltaSeconds);
	void Move(IntVec2 newPosition);
	Object* GetInteractingObject();
};
