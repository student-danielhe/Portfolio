#pragma once
struct Vec3;
struct Vec4;
struct IntVec2;
struct AABB2;
struct LineSegment2;
struct Capsule2;
struct OBB2;
struct OBB3;
#include "Engine/Math/Mat44.hpp"
#include"Engine/Math/Vec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Plane.hpp"
struct RaycastResult2D
{
	// Basic raycast result information (required)
	bool m_didImpact = false;
	float m_impactDist = 0.f;
	Vec2 m_impactPos;
	Vec2 m_impactNormal;
};
struct RaycastResult3D
{
	// Basic raycast result information (required)
	bool m_didImpact = false;
	float m_impactDist = 0.f;
	Vec3 m_impactPos;
	Vec3 m_impactNormal;
};
enum class BillboardType {
	NONE=-1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPPOSING,
	COUNT
};
//angle
float ConvertDegreesToRadians(float degree);
float ConvertRadiansToDegrees(float radiance);
float CosDegrees(float degree);
float SinDegrees(float degree);
float Atan2Degrees(float y,float x);
float GetShortestAngularDispDegrees(float start, float end);
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);

//Basic 2D and 3D
float GetDistance2D(Vec2 const& positionA,Vec2 const& positionB);
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB);
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);

//Geometry
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool DoAABB3sOverlap(AABB3 const& cubeA, AABB3 const& cubeB);
bool DoZCylindersOverlap(Vec3 const& bottomA, float radiusA, FloatRange const& ZRangeA,
	                     Vec3 const& bottomB, float radiusB, FloatRange const& ZRangeB);
bool DoSphereOverlapAABB3(Vec3 const& center, float radius, AABB3 const& cube);
bool DoSphereOverlapZCylinder(Vec3 const& sphereCenter, float sphereRadius, 
	Vec3 const& cylinderBottom, float cylinderRadius, FloatRange const& cylinderZRange);
bool DoAABB3OverlapZCylinder(AABB3 const& cube, Vec3 const& cylinderBottom, float cylinderRadius, FloatRange const& cylinderZRange);
bool DoOBB3OverlapSphere3D(Vec3 const& center, float radius, OBB3 const& cube);
bool DoOBB3OverlapPlane3D(OBB3 const& cube, Plane3 const& plane);
bool DoAABB3OverlapPlane3D(AABB3 const& cube, Plane3 const& plane);
bool DoSphereOverlapPlane3D(Vec3 const& center, float radius, Plane3 plane);


//Tranform
void TransformPosition2D(Vec2& posToTransform,float uniformScale,float rotationDegree, Vec2 const& translation);
void TransformPositionXY3D(Vec3& posToTransform, float scaleXY, float zRotationDegree, Vec2 const& translationXY);
void TransformPosition2D(Vec2& posToTransform, Vec2 const& i, Vec2 const& j, Vec2 const& translation);
void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& i, Vec2 const& j, Vec2 const& translationXY);

//Clmap and lerp
float GetClamped(float val, float min, float max);
float GetClampedZeroToOne(float val);
float Interpolate(float start, float end, float fractionTowardEnd);
float GetFractionWithinRange(float val, float rangeStart, float rangeEnd);
float RangeMap(float inVal, float inStart, float inEnd, float outStart, float outEnd);
float RangeMapClamped(float inVal, float inStart, float inEnd, float outStart, float outEnd);
int RoundDownToInt(float val);


//Dot
float DotProduct2D(Vec2 const& a, Vec2 const& b);
float DotProduct3D(Vec3 const& a, Vec3 const& b);
float DotProduct4D(Vec4 const& a, Vec4 const& b);
float CrossProduct2D(Vec2 const& a, Vec2 const& b);
Vec3  CrossProduct3D(Vec3 const& a, Vec3 const& b);

int GetTaxicabDistance2D(IntVec2 const& A, IntVec2 const& B);
float GetProjectedLength2D(Vec2 const& proj, Vec2 const& onto);
Vec2 const GetProjectedOnto2D(Vec2 const& proj, Vec2 const& onto);
Vec3 const GetProjectedOnto3D(Vec3 const& proj, Vec3 const& onto);

bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& center, float radius);
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& center,float sectorDeg, float sectorApDeg, float radius);
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& center,Vec2 const& sectorNormal, float sectorApDeg, float radius);

bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool IsPointInsideTriangle2D(Vec2 const& point, Vec2 const& vertexA, Vec2 const& vertexB, Vec2 const& vertexC);
bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& obb2);

bool IsPointInsideAABB3D(Vec3 const& point, AABB3 const& cube);
bool IsPointInsideZCylinder3D(Vec3 const& point, Vec3 const& bottom, float radius, FloatRange const& ZRange);
bool IsPointInsideSphere3D(Vec3 const& point, Vec3 const& center, float radius);
bool IsPointInsideOBB3D(Vec3 const& point, OBB3 const& cube);
bool IsPointInFrontPlane3D(Vec3 const& point, Plane3 const& plane);

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b);

bool PushDiscOutOfPoint2D(Vec2& center, float radius, Vec2 const& point);
bool PushDiscOutOfDisc2D(Vec2& mobCenter, float radius, Vec2 const& center, float radius2);
bool PushDiscsOutOfEachOther2D(Vec2& centerA, float radiusA, Vec2& centerB, float radiusB);
bool PushDiscOutOfAABB2D(Vec2& point, float radius, AABB2 const& box);
bool PushAABB3OutOfAABB3(AABB3& mobileBox, AABB3 const& staticBox, bool verticalPush = false);


Vec2 GetNearestPointOnDisc2D(Vec2 const& point, Vec2 const& center, float radius);
Vec2 GetNearestPointOnLineSegment2D(Vec2 const& point, Vec2 const& start, Vec2 const& end);
Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& point, Vec2 const& start, Vec2 const& end);
Vec2 GetNearestPointOnCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
Vec2 GetNearestPointOnTriangle2D(Vec2 const& point, Vec2 const& vertexA, Vec2 const& vertexB, Vec2 const& vertexC);
Vec2 GetNearestPointOnOBB2D(Vec2 const& point, OBB2 const& obb2);

Vec3 GetNearestPointOnSphere3D(Vec3 const& point, Vec3 const& center, float raius);
Vec3 GetNearestPointOnZCylinder3D(Vec3 const& point, Vec3 const& bottom, float radius, FloatRange const& ZRange);
Vec3 GetNearestPointOnOBB3D(Vec3 const& point, OBB3 const& cube);
Vec3 GetNearestPointOnPlane3D(Vec3 const& point, Plane3 const& plane);



RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius);
RaycastResult2D RaycastVsLineSegment2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 lineStart, Vec2 lineEnd);
RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 box);

RaycastResult3D RaycastVsSphere3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 center, float radius);
RaycastResult3D RaycastVsAABB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, AABB3 cube);
RaycastResult3D RaycastVsZCylinder3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 bottom, float radius, FloatRange ZRange);
RaycastResult3D RayCastVSOBB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, OBB3 cube);
RaycastResult3D RayCastVSPlane3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3 plane);

//Beizer---------------------------
float Lerp(float A, float B, float C, float t);
float ComputeCubicBezier1D(float A, float B, float C, float D, float t);
float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t);

//________________________________________________________________________

void BounceDiscOffDisc2D (Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2 staticCenter, float staticRadius, float staticElasticity);
void BounceDiscOffOther2D(Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2& centerB, float radiusB, float elasticityB, Vec2& velocityB);
void BounceDiscOffOBB2D  (Vec2& center, float radius, float elasticity, Vec2& velocity, OBB2 box, float staticElasticity);
void BounceDiscOffCapsule2D (Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2 staticHead, Vec2 staticTail, float staticRadius, float staticElasticity);
void BounceDiscOffPoint  (Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2 point, float staticElasticity = .9f);

float PointDistToPlane(Vec3 const& point, Plane3 const& plane);

//Euler Angle
float NormalizeByte(unsigned char byteValue);
unsigned char DenormalizeByte(float zeroToOne);

Mat44 GetBillboardTransform(BillboardType billboardType,
	Mat44 const& targetTransform, const Vec3& billboardPosition,
	const Vec2& billboardScale = Vec2(1.f, 1.f));



