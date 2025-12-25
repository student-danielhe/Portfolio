#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
struct EulerAngles
{
public:
	EulerAngles() = default;
	EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees);
	void GetAsVectors_IFwd_JLeft_KUp(Vec3& out_fwdIbasis, Vec3& out_leftJBasis, Vec3& out_upKBasis);
	Mat44 GetAsMatrix_IFwd_JLeft_KUp()const;
	void SetFromText(char const* text = "0,0,0");
public:
	float m_yawDegrees = 0.f;
	float m_pitchDegrees = 0.f;
	float m_rollDegrees = 0.f;
};