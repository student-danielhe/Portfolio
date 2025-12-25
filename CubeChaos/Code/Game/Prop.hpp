#pragma once
#include "Entity.hpp"

class Prop : Entity {
public:
	Prop(Game* owner);
	void Render()const;
	void Update(float deltaSeconds);
	Mat44 GetModelToWorld()const;
	void SetVerts(std::vector<Vertex_PCU> verts);
	void SetAngularVelocity(EulerAngles angular);
	void SetVelocity(Vec3 velocity);
	void SetPosition(Vec3 pos);
	void SetAngle(EulerAngles angle);
	void SetColor(Rgba8 color);
	void SetTexture(Texture* texture);
};