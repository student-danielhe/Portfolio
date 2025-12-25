#pragma once
#include "Engine/Math/Vec3.hpp"
#include "MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Mat44.hpp"
struct OBB3 {
public:
	AABB3 const GetAsLocalAABB3() const;
	Vec3 const GetLocalPosForWorldPos(Vec3 const& worldPos) const;
	Vec3 const GetWorldPosForLocalPos(Vec3 const& localPos) const;
	void RotateAboutCenter(EulerAngles rotationDeltaDegrees);
	OBB3(Vec3* center, Vec3* dimension, EulerAngles* orientation);
	Mat44 const GetTransformMatrix()const;
public:
	Vec3* m_center;
	Vec3* m_halfDimension;
	EulerAngles* m_orientation =nullptr;
};