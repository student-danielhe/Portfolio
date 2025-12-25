#pragma once
#include "Engine/Math/Curve1D.hpp"

class CurveLinear1D :public Curve1D {
	public:
	float m_startVal = 0.f;
	float m_endVal = 1.f;

	public:
	CurveLinear1D(float startVal, float endVal): m_startVal(startVal), m_endVal(endVal){}

	float Evaluate(float t);
};