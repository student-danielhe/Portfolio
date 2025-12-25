#include "Engine/Math/AABB2.hpp"
#include "math.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
const AABB2 AABB2::DEFAULT{ Vec2(0.f,0.f), Vec2(1.f,1.f) };
AABB2::AABB2(AABB2 const& copyFrom)
	:m_mins(copyFrom.m_mins)
	,m_maxs(copyFrom.m_maxs)
{}
AABB2::AABB2(float minX, float minY, float maxX, float maxY) 
	:m_mins(Vec2(minX,minY))
	,m_maxs(Vec2(maxX,maxY))
{}
AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
	:m_mins(mins)
	,m_maxs(maxs)
{}

//Accessors
bool AABB2::IsPointInside(Vec2 const& point)const
{
	if (point.x <= m_maxs.x) {
		if (point.x >= m_mins.x) {
			if (point.y <= m_maxs.y) {
				if (point.y >= m_mins.y) {
					return true;
				}
			}
		}
	}
	return false;
}
Vec2 const AABB2::GetCenter()const{
	return (m_mins + m_maxs) / 2;
}
Vec2 const AABB2::GetDimensions()const{
	return m_maxs - m_mins;
}
Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePoint)const{
	float x = referencePoint.x;
	float y = referencePoint.y;
	x = fmaxf(x,m_mins.x);
	x = fminf(x, m_maxs.x);
	y = fmaxf(y, m_mins.y);
	y = fminf(y, m_maxs.y);
	return Vec2(x, y);
}
Vec2 const AABB2::GetPointAtUV(Vec2 const& uv)const {
	float x = RangeMap(uv.x,0,1, m_mins.x, m_maxs.x);
	float y = RangeMap(uv.y, 0, 1, m_mins.y, m_maxs.y);
	return Vec2(x,y);
}
Vec2 const AABB2::GetUVForPoint(Vec2 const& point)const{
	float x = GetFractionWithinRange(point.x, m_mins.x, m_maxs.x);
	float y = GetFractionWithinRange(point.y, m_mins.y, m_maxs.y);
	return Vec2(x, y);
}

//Mutator
void AABB2::Translate(Vec2 const& translationToApply){
	m_mins += translationToApply;
	m_maxs += translationToApply;
}
void AABB2::SetCenter(Vec2 const& newCenter){
	Vec2 oldCenter = GetCenter();
	Vec2 disp = newCenter - oldCenter;
	Translate(disp);
}
void AABB2::SetDimensions(Vec2 const& point){
	Vec2 center = GetCenter();
	m_maxs = center + point / 2;
	m_mins = center - point / 2;
}
void AABB2::StretchToIncludePoint(Vec2 const& point){
	float x = point.x;
	float y = point.y;
	m_maxs.x = fmaxf(x, m_maxs.x);
	m_mins.x = fminf(x, m_mins.x);
	m_maxs.y = fmaxf(y, m_maxs.y);
	m_mins.y = fminf(y, m_mins.y);
}
bool AABB2::operator==(const AABB2& compare) const
{
	return m_mins == compare.m_mins && m_maxs == compare.m_maxs;
}

bool AABB2::DoesBoxOverlap(AABB2 other) const{
	FloatRange xRange(fminf(m_mins.x,m_maxs.x), fmaxf(m_mins.x,m_maxs.x));
	FloatRange yRange(fminf(m_mins.y, m_maxs.y), fmaxf(m_mins.y, m_maxs.y));


	FloatRange otherXRange(fminf(other.m_mins.x, other.m_maxs.x), fmaxf(other.m_mins.x, other.m_maxs.x));
	FloatRange otherYRange(fminf(other.m_mins.y, other.m_maxs.y), fmaxf(other.m_mins.y, other.m_maxs.y));
	return (xRange.IsOverlappingWith(otherXRange) || yRange.IsOverlappingWith(otherYRange)||otherXRange.IsOverlappingWith(xRange)|| otherYRange.IsOverlappingWith(yRange));
}


