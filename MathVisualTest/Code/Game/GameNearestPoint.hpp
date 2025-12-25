#pragma once
#include "Game.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Core/VertexUtils.hpp"
class GameNearestPoint :public Game {
public:
	void Render();

	void StartUp();
	void Update(float deltaSceonds);

public:
	//"PLayer"
	Vec2 m_player=Vec2(0, 0);
	//Disc
	Vec2 m_discCenter;
	float m_discRadius=0.f;
	//Triangle
	Vec2 m_triA;
	Vec2 m_triB;
	Vec2 m_triC;
	//line seg
	Vec2 m_segStart;
	Vec2 m_segEnd;
	//line infinite
	Vec2 m_infStart;
	Vec2 m_infEnd;
	//Capsule
	Vec2 m_boneStart;
	Vec2 m_boneEnd;
	float m_capsuleRad=0.f;
	//AABB2
	AABB2 *m_AABB2=nullptr;
	//OBB2
	OBB2* m_OBB2 = nullptr;
};