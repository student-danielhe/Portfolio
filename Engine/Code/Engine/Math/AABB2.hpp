#pragma once
#include"Engine/Math/Vec2.hpp"

//---------------------------------------------------------
struct AABB2 {
public:
	Vec2 m_mins;
	Vec2 m_maxs;
public:
	//Construction/destruction
	~AABB2() {}
	AABB2() {}
	AABB2(AABB2 const& copyFrom);
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(Vec2 const& mins, Vec2 const& maxs);

	//Accessors
	bool IsPointInside(Vec2 const& point)const;
	Vec2 const GetCenter()const;
	Vec2 const GetDimensions()const;
	Vec2 const GetNearestPoint(Vec2 const& referencePoint)const;
	Vec2 const GetPointAtUV(Vec2 const& uv)const;
	Vec2 const GetUVForPoint(Vec2 const& point)const;

	//Mutator
	void Translate(Vec2 const& translationToApply);
	void SetCenter(Vec2 const& newCenter);
	void SetDimensions(Vec2 const& point);
	void StretchToIncludePoint(Vec2 const& point);
	bool		operator==(const AABB2& compare) const;

	bool DoesBoxOverlap(AABB2 other) const;

	static const AABB2 DEFAULT;
};