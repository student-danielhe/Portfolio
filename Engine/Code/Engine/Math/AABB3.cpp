#include "Engine/Math/AABB3.hpp"
#include "math.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
const AABB3 AABB3::DEFAULT{ Vec3(0.f,0.f,0.f), Vec3(1.f,1.f,1.f) };
AABB3::AABB3(AABB3 const& copyFrom)
	:m_mins(copyFrom.m_mins)
	,m_maxs(copyFrom.m_maxs)
{}
AABB3::AABB3(float minX, float minY,float minZ, float maxX, float maxY,float maxZ) 
	:m_mins(Vec3(minX,minY,minZ))
	,m_maxs(Vec3(maxX,maxY,maxZ))
{}
AABB3::AABB3(Vec3 const& mins, Vec3 const& maxs)
	:m_mins(mins)
	,m_maxs(maxs)
{}

//Accessors
bool AABB3::IsPointInside(Vec3 const& point)const
{
	if (point.x <= m_maxs.x) {
		if (point.x >= m_mins.x) {
			if (point.y <= m_maxs.y) {
				if (point.y >= m_mins.y) {
					if (point.z <= m_maxs.z) {
						if (point.z >= m_mins.z) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
Vec3 const AABB3::GetCenter()const{
	return (m_mins + m_maxs) / 2;
}
Vec3 const AABB3::GetDimensions()const{
	return m_maxs - m_mins;
}
Vec3 const AABB3::GetNearestPoint(Vec3 const& referencePoint)const{
	float x = referencePoint.x;
	float y = referencePoint.y;
	float z = referencePoint.z;
	x = fmaxf(x,m_mins.x);
	x = fminf(x, m_maxs.x);
	y = fmaxf(y, m_mins.y);
	y = fminf(y, m_maxs.y);
	z = fmaxf(z, m_mins.z);
	z = fminf(z, m_maxs.z);
	return Vec3(x, y,z);
}


//Mutator
void AABB3::Translate(Vec3 const& translationToApply){
	m_mins += translationToApply;
	m_maxs += translationToApply;
}
void AABB3::SetCenter(Vec3 const& newCenter){
	Vec3 oldCenter = GetCenter();
	Vec3 disp = newCenter - oldCenter;
	Translate(disp);
}
void AABB3::SetDimensions(Vec3 const& point){
	Vec3 center = GetCenter();
	m_maxs = center + point / 2;
	m_mins = center - point / 2;
}
void AABB3::StretchToIncludePoint(Vec3 const& point){
	float x = point.x;
	float y = point.y;
	float z = point.z;
	m_maxs.x = fmaxf(x, m_maxs.x);
	m_mins.x = fminf(x, m_mins.x);
	m_maxs.y = fmaxf(y, m_maxs.y);
	m_mins.y = fminf(y, m_mins.y);
	m_maxs.z = fmaxf(z, m_maxs.z);
	m_mins.z = fminf(z, m_mins.z);
}
bool AABB3::operator==(const AABB3& compare) const
{
	return m_mins == compare.m_mins && m_maxs == compare.m_maxs;
}

std::vector<Vec3> AABB3::GetCorners()const {
	Vec3 mins = m_mins;
	Vec3 maxs = m_maxs;
	Vec3 x_min_y_min_z_max(mins.x, mins.y, maxs.z);
	Vec3 x_min_y_max_z_min(mins.x, maxs.y, mins.z);
	Vec3 x_min_y_max_z_max(mins.x, maxs.y, maxs.z);
	Vec3 x_max_y_min_z_min(maxs.x, mins.y, mins.z);
	Vec3 x_max_y_min_z_max(maxs.x, mins.y, maxs.z);
	Vec3 x_max_y_max_z_min(maxs.x, maxs.y, mins.z);
	std::vector<Vec3> result;
	result.push_back(m_mins);
	result.push_back(x_min_y_min_z_max);
	result.push_back(x_min_y_max_z_min);
	result.push_back(x_min_y_max_z_max);
	result.push_back(x_max_y_min_z_min);
	result.push_back(x_max_y_min_z_max);
	result.push_back(x_max_y_max_z_min);
	result.push_back(m_maxs);
	return result;
}