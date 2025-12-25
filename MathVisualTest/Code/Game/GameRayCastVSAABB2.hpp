#pragma once
#include "Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include<vector>
class GameRayCastVSAABB2 :public Game {
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void HandleKeyPress(float deltaSecond);
	std::vector<AABB2> m_boxs;
	Vec2 m_arrowTail;
	Vec2 m_arrowHead;
};