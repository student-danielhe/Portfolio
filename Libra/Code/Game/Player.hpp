#pragma once
#include "Entity.hpp"

class Player : public Entity {
public:
	virtual void Update(float deltaSecond) override;
	virtual void Render(std::vector<Vertex_PCU>& verts)override;
	void HandleKeyPress(float deltaSecond);
	Player(Map* owner, Vec2 const& pos, float ordientationDegree);
	~Player() {}
	float m_turretOrientationDegrees;
	std::vector<Vertex_PCU> m_turretLocalVertexArray;
	void TurretGlobalVertexArray(std::vector<Vertex_PCU>& result);
	void Shoot(float deltaSeconds);
public:
	float m_targetAngle = .0f;
	//float m_currentFireCooldown = .0f;
};