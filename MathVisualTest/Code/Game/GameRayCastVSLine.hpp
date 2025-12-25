#pragma once
#include "Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include<vector>
class GameRayCastVSLine :public Game {
public:
	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void HandleKeyPress(float deltaSecond);
	std::vector<Vec2> m_LineStarts;
	std::vector<Vec2> m_LineEnds;
	Vec2 m_arrowTail;
	Vec2 m_arrowHead;
};