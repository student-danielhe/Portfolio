#pragma once
#include "Engine/Math/Vec2.hpp"
#include "MathUtils.hpp"
struct OBB2 {
    public:
	    void GetCornerPoints(Vec2* out_fourCornerWorldPositions) const;
	    Vec2 const GetLocalPosForWorldPos(Vec2 const& worldPos) const;
	    Vec2 const GetWorldPosForLocalPos(Vec2 const& localPos) const;
	    void RotateAboutCenter(float rotationDeltaDegrees);
		OBB2(Vec2 const& center,Vec2 const& dimension, Vec2 const& iBases);
    public:
		Vec2 m_center;
		Vec2 m_dimension;
		Vec2 m_iBases;
};