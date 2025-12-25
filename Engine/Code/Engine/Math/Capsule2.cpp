#include "Engine/Math/Capsule2.hpp"
Capsule2::Capsule2(Vec2 const& start, Vec2 const& end, float radius)
	:m_start(start)
	, m_end(end)
	,m_radius(radius)
{
}
void Capsule2::Translate(Vec2 const& translate) {
	m_start += translate;
	m_end += translate;
}
void Capsule2::SetCenter(Vec2 const& center) {
	Vec2 currentCenter = m_start + (m_end - m_start) / 2;
	m_start += center - currentCenter;
	m_end += center - currentCenter;
}
void Capsule2::RotateAboutCenter(float degree) {
	Vec2 center = m_start + (m_end - m_start) / 2;
	Vec2 startHalf = m_start - center;
	Vec2 endHalf = m_end - center;
	m_start = center + startHalf.GetRotatedDegrees(degree);
	m_end = center + endHalf.GetRotatedDegrees(degree);
}