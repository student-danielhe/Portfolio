#pragma once
#include"Engine/Math/Vec3.hpp"
#include <vector>
//---------------------------------------------------------
struct AABB3 {
public:
	Vec3 m_mins;
	Vec3 m_maxs;
public:
	//Construction/destruction
	~AABB3() {}
	AABB3() {}
	AABB3(AABB3 const& copyFrom);
	explicit AABB3(float minX, float minY,float minZ, float maxX, float maxY,float maxZ);
	explicit AABB3(Vec3 const& mins, Vec3 const& maxs);

	//Accessors
	bool IsPointInside(Vec3 const& point)const;
	Vec3 const GetCenter()const;
	Vec3 const GetDimensions()const;
	Vec3 const GetNearestPoint(Vec3 const& referencePoint)const;

	std::vector<Vec3> GetCorners()const;

	//Mutator
	void Translate(Vec3 const& translationToApply);
	void SetCenter(Vec3 const& newCenter);
	void SetDimensions(Vec3 const& point);
	void StretchToIncludePoint(Vec3 const& point);
	bool		operator==(const AABB3& compare) const;

	static const AABB3 DEFAULT;
};