#include "Engine/Input/AnalogJoystic.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
Vec2 AnalogJoystic::GetPosition()const {
	return m_CorrectedCoord;
}
float AnalogJoystic::GetMagnitude()const {
	return fabsf(m_CorrectedCoord.GetLength());
}
float AnalogJoystic::GetOrientationDegrees()const {
	return m_CorrectedCoord.GetOrientationDegrees();
}

Vec2 AnalogJoystic::GetRawPosition()const {
	return m_RawCoord;
}
float AnalogJoystic::GetInnerDeadZone()const {
	return m_InnerDead;
}
float AnalogJoystic::GetOuterDeadZone()const {
	return m_OuterDead;
}

void AnalogJoystic::Reset() {
	m_CorrectedCoord = Vec2(0, 0);
	m_RawCoord = Vec2(0, 0);
	m_InnerDead = .3f;
	m_OuterDead = .9f;
}
void AnalogJoystic::SetDeadZone(float inner, float outer) {
	m_InnerDead = inner;
	m_OuterDead = outer;
}
void AnalogJoystic::UpdatePosition(float rawX, float rawY) {
	m_RawCoord.x = rawX;
	m_RawCoord.y = rawY;
	float tempX = fabsf(rawX) - 32767*m_InnerDead;
	if (rawX < 0) {
		m_CorrectedCoord.x = GetClamped(-tempX, -(m_OuterDead - m_InnerDead) * 32767, 0);
	}else {
		m_CorrectedCoord.x = GetClamped(tempX, 0, (m_OuterDead - m_InnerDead) * 32767);
	}
	
	float tempY = fabsf(rawY) - 32767*m_InnerDead;
	if (rawY < 0) {
		m_CorrectedCoord.y = GetClamped(-tempY, -(m_OuterDead - m_InnerDead) * 32767, 0);
	}
	else {
		m_CorrectedCoord.y = GetClamped(tempY, 0, (m_OuterDead - m_InnerDead) * 32767);
	}
}