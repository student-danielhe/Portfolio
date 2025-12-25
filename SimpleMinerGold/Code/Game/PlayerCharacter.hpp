#pragma once
#include "Entity.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Map.hpp"
#include "Engine/Core/Timer.hpp"
class Player;
class PlayerCharacter : public Entity {
public:

	VertexBuffer* m_vb;
	IndexBuffer* m_ib;
	Timer* m_physicsTimer;
    AABB3 m_body;
	bool m_grounded = false;
	
	Vec3 m_bottomPosition = Vec3();
	EulerAngles m_orientation = EulerAngles();
	Vec3 m_velocity = Vec3();
	Vec3 m_acceleration = Vec3();
	Vec2 m_horizontalVelocity = Vec2();
	float m_verticalVelocity = 0.f;
	bool m_dead = false;

	float m_playerWidth = 0.6f;
	float m_playerHeight = 1.8f;

	float m_runSpeed = 10.f;
	float m_drag = 8.f;
	float m_flyForce = 4.f;
	float m_flyDrag = 4.f;
	float m_runForce = 64.f;

	float m_vertSpeed = 56.f;

	float m_jumpImpulse = 6.f;
	float m_eyeHeight = 1.65f;
	float m_camDist = 4.f;

	Player* m_camera = nullptr;
public:
	PlayerCharacter(Map* owner);
	~PlayerCharacter();

	void Update(float deltaSeconds);
	void Render()const;
	void PhysicsUpdate(float deltaSeconds);

	void HandleKeyPressed(float deltaSeconds);
	void CyclePhysicsMode();

	void AddForce(Vec3 force);
	void AddImpulse(Vec3 impulse);

	void PushOutOfBlocks();

	Vec3 GetForwardNormal();

	bool IsGrounded(float tolerance = .1f);
	//--------------------------------------------------------
	void ClampSpeed();
	void ApplyGravityAndDrag();
	void UpdateBody();
	void SetPosition(Vec3 pos);
	void NoClipUpdate();
//-------------------------------------------------------
	Chunk* GetCurrentChunk();
	IntVec3 GetCoordInChunk();
	float GetGroundHeight();
	std::vector<IntVec3> GetOverlapingBlocks();
	AABB3 GetLocalBody();
};