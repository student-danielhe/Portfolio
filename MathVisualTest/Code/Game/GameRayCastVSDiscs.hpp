#pragma once
#include "Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include<vector>
class GameRayCastVSDiscs :public Game {
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void HandleKeyPress(float deltaSecond);
	std::vector<Vec2> m_discCenters;
	std::vector<float> m_discRadius;
	Vec2 m_arrowTail;
	Vec2 m_arrowHead;
};