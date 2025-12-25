#pragma once
#include "MathUtils.hpp"
#include <vector>
struct CubicHermiteCurve {
	Vec2 m_startPos;
	Vec2 m_endPos;
	Vec2 m_startVelocity;
	Vec2 m_endVelocity;
};
class CubicBeizerCurve2D {
public:
	Vec2 m_startPos;
	Vec2 m_guidePos1;
	Vec2 m_guidePos2;
	Vec2 m_endPos;
	CubicBeizerCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos);
	CubicBeizerCurve2D(CubicHermiteCurve const& fromHermite);
	Vec2 EvaluateAtParametric(float parametricZeroToOne)const;
	float GetApproximateLength(int numSplit = 64)const;
	Vec2 EvaluateAtApproximateDistance(float distAlongCurve, int numSplit = 64) const;
	static std::vector<CubicBeizerCurve2D> MakeCatmullRom(std::vector<Vec2>const& positions, std::vector<CubicHermiteCurve>& outHermite);
};