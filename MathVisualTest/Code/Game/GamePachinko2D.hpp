#pragma once
#include "Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include<vector>
struct DiscBumper {
	Vec2 m_center;
	float m_radius = 0.f;
	float m_bounciness = 0.f;
};
struct MobileDisc{
	Vec2 m_center;
	float m_radius = 0.f;
	Vec2 m_velocity;
	Rgba8 m_color;
};
struct CapsuleBumper {
	Vec2 m_head;
	Vec2 m_tail;
	float m_radius = 0.f;
	float m_bounciness = 0.f;
};
struct OBBBumper {
	OBB2 m_box = OBB2(Vec2(),Vec2(),Vec2());
	float m_bounciness = 0.f;
};
class GamePachinko2D :public Game {
public:
	void StartUp();
	void Update(float deltaSecond);
	void FixedUpdate(float deltaSeconds);
	void Render();
	void HandleKeyPress(float deltaSecond);
	Vec2 m_arrowTail;
	Vec2 m_arrowHead;
	std::vector<MobileDisc> m_dynamicDisc;
	std::vector<DiscBumper> m_staticDisc;
	std::vector<OBBBumper> m_staticBox;
	std::vector<CapsuleBumper> m_staticCapsule;
	float m_globalBounciness = .9f;
	float m_fixedDeltaSeconds = .005f;
	Timer* m_physicsTimer = nullptr;
	Clock* m_gameClock = nullptr;
	bool m_bottomWall = true;
	bool m_usingPhysicsUpdate = true;
private:
	void InitializeBumpers();
	void RenderDiscBumpers();
	void RenderOBBBumpers();
	void RenderCapsuleBumpers();
	void RenderMobileDisc();
	void CollideWithMobileDisc(MobileDisc& disc);
	void CollideWithDisc(MobileDisc& disc);
	void CollideWithBox(MobileDisc& disc);
	void CollideWithCapsule(MobileDisc& disc);
	void CollideWithWorld(MobileDisc& disc);
};