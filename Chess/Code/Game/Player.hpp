#pragma once
#include "Entity.hpp"
#include "Engine/Renderer/Camera.hpp"
class Player : Entity {
public:
	Player(Game* owner);
	void Update(float deltaSecond);
	void Render()const;
	Mat44 GetTransform();
	Vec3 GetPosition();
	Vec3 GetFwdVector();
	Vec3 GetOrientation();
};
