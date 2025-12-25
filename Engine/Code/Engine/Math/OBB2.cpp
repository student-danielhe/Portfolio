#include"Engine/Math/OBB2.hpp"
void OBB2::GetCornerPoints(Vec2* out_fourCornerWorldPositions) const {
	Vec2 JBases = m_iBases.GetRotated90Degrees();
	Vec2 BR = m_center + m_iBases * m_dimension.x - JBases * m_dimension.y;
	Vec2 TR = m_center + m_iBases * m_dimension.x + JBases * m_dimension.y;
	Vec2 TL = m_center - m_iBases * m_dimension.x + JBases * m_dimension.y;
	Vec2 BL = m_center - m_iBases * m_dimension.x - JBases * m_dimension.y;
	out_fourCornerWorldPositions[0] = BR;
	out_fourCornerWorldPositions[1] = TR;
	out_fourCornerWorldPositions[2] = TL;
	out_fourCornerWorldPositions[3] = BL;
}
Vec2 const OBB2::GetLocalPosForWorldPos(Vec2 const& worldPos) const {
	Vec2 displacementToCenter = worldPos - m_center;
	float localX = DotProduct2D(displacementToCenter, m_iBases);
	float localY = DotProduct2D(displacementToCenter, m_iBases.GetRotated90Degrees());
	return Vec2(localX, localY);
}
Vec2 const OBB2::GetWorldPosForLocalPos(Vec2 const& localPos) const {
	return m_center + localPos.x * m_iBases + localPos.y * m_iBases.GetRotated90Degrees();
}
void OBB2::RotateAboutCenter(float rotationDeltaDegrees) {
	m_iBases=m_iBases.GetRotatedDegrees(rotationDeltaDegrees);
}
OBB2::OBB2(Vec2 const& center, Vec2 const& dimension, Vec2 const& iBases)
	:m_center(center)
	,m_dimension(dimension)
	,m_iBases(iBases)
{
}