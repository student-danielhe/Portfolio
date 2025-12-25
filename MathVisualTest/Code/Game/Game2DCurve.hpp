#pragma once
#include "Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/CubicBeizerCurve2D.hpp"
#include "Engine/Math/Easing.hpp"
#include "Engine/Core/Timer.hpp"
#include<vector>
enum class EasingMode {
	SMOOTH_START2,
	SMOOTH_START3,
	SMOOTH_START4,
	SMOOTH_START5,
	SMOOTH_START6,
	SMOOTH_STOP2,
	SMOOTH_STOP3,
	SMOOTH_STOP4,
	SMOOTH_STOP5,
	SMOOTH_STOP6,
	SMOOTH_STEP3,
	SMOOTH_STEP5,
	HESITATE3,
	HESITATE5,
	CUSTOM_FUNKY,
	INDENTITY,
	Count,
};
class Game2DCurve :public Game {
public:
	void StartUp();
	void Update(float deltaSeconds);
	void Render();
	void HandleKeyPress(float deltaSecond);

	void RenderEase();
	void RenderBeizer();
	void RenderSpline();
	void RenderCurve(std::vector<Vertex_PCU>& verts, CubicBeizerCurve2D curve);
	void RandomStart();

	CubicBeizerCurve2D* m_curve = nullptr;
	std::vector<CubicBeizerCurve2D> m_spline;
	std::vector<CubicHermiteCurve> m_splineHermite;
	int m_currentMode = (int)EasingMode::SMOOTH_START2;
	int m_numSplit = 32;
	int m_currentSplineSection = 0;
	Clock* m_clock = nullptr;
	Timer* m_timer =nullptr;
};