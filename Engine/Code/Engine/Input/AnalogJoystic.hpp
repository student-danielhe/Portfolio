#pragma once
#include "Engine/Math/Vec2.hpp"
class AnalogJoystic {
public:
	Vec2 GetPosition()const;
	float GetMagnitude()const;
	float GetOrientationDegrees()const;

	Vec2 GetRawPosition()const;
	float GetInnerDeadZone()const;
	float GetOuterDeadZone()const;

	void Reset();
	void SetDeadZone(float inner, float outer);
	void UpdatePosition(float rawX, float rawY);
protected:
	Vec2 m_RawCoord;
	Vec2 m_CorrectedCoord;
	float m_InnerDead=0.3f;
	float m_OuterDead=0.95f;
};