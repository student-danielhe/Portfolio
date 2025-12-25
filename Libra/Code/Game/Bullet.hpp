#pragma once
#include "Entity.hpp"
#include "Engine/Math/AABB2.hpp"
class Bullet:public Entity {
	friend class Map;
protected:
	explicit Bullet(Map* owner, Vec2 const& pos, float ordientationDegree);
	~Bullet() {}
	virtual void Update(float deltaSecond) override;

};