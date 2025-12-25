#include "Engine/Math/OBB3.hpp"
Vec3 const OBB3::GetLocalPosForWorldPos(Vec3 const& worldPos) const {
	Vec3 i;
	Vec3 j;
	Vec3 k;
	m_orientation->GetAsVectors_IFwd_JLeft_KUp(i, j, k);
	Vec3 displacementToCenter = worldPos - *m_center;
	float localX = DotProduct3D(displacementToCenter, i);
	float localY = DotProduct3D(displacementToCenter, j);
	float localZ = DotProduct3D(displacementToCenter, k);
	return Vec3(localX, localY, localZ);
}

Vec3 const OBB3::GetWorldPosForLocalPos(Vec3 const& localPos) const {
	Vec3 i;
	Vec3 j;
	Vec3 k;
	m_orientation->GetAsVectors_IFwd_JLeft_KUp(i, j, k);
	return *m_center + localPos.x * i + localPos.y * j + localPos.z*k;
}

void OBB3::RotateAboutCenter(EulerAngles rotationDeltaDegrees) {
	m_orientation->m_yawDegrees += rotationDeltaDegrees.m_yawDegrees;
	m_orientation->m_pitchDegrees += rotationDeltaDegrees.m_pitchDegrees;
	m_orientation->m_rollDegrees += rotationDeltaDegrees.m_rollDegrees;
}

OBB3::OBB3(Vec3* center, Vec3* halfDimension, EulerAngles* orientation)
	:m_center(center)
	,m_halfDimension(halfDimension)
	,m_orientation(orientation)
{
}

AABB3 const OBB3::GetAsLocalAABB3()const {
	return AABB3(-*m_halfDimension, *m_halfDimension);
}

Mat44 const OBB3::GetTransformMatrix()const {
	Mat44 transform = Mat44::MakeTranslation3D(*m_center);
	transform.Append(m_orientation->GetAsMatrix_IFwd_JLeft_KUp());
	return transform;
}