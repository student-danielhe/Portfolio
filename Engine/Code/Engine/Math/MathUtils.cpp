#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include <cmath>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Capsule2.hpp"
#include "LineSegment2.hpp"
#include "OBB2.hpp"
#include "OBB3.hpp"
#include "Engine/Core/EngineCommon.hpp"
//using namespace std;
//# define pi           3.14159265358979323846  /* pi */
constexpr float pi = 3.1415926535897932384626433832795f;
float ConvertDegreesToRadians(float degree) {
	return degree * (pi/180.f); 
}
float ConvertRadiansToDegrees(float radians) {
	return radians * (180.f/pi);
}
float CosDegrees(float degree) {
	return cosf(ConvertDegreesToRadians(degree));
}
float SinDegrees(float degree) {
	return sinf(ConvertDegreesToRadians(degree));
}
float Atan2Degrees(float y, float x) {
	return ConvertRadiansToDegrees(atan2(y, x));
}

//Basic 2D and 3D
float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB) {
	float xDist = positionB.x - positionA.x;
	float yDist = positionB.y - positionA.y;
	return sqrtf(xDist * xDist + yDist * yDist);
}
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB) {
	float xDist = positionB.x - positionA.x;
	float yDist = positionB.y - positionA.y;
	return (xDist * xDist + yDist * yDist);
}
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB) {
	float xDist = positionB.x - positionA.x;
	float yDist = positionB.y - positionA.y;
	float zDist = positionB.z - positionA.z;
	return sqrtf(xDist * xDist + yDist * yDist+zDist*zDist);
}
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB) {
	float xDist = positionB.x - positionA.x;
	float yDist = positionB.y - positionA.y;
	float zDist = positionB.z - positionA.z;
	return (xDist * xDist + yDist * yDist + zDist * zDist);
}
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB) {
	float xDist = positionB.x - positionA.x;
	float yDist = positionB.y - positionA.y;
	return sqrtf(xDist * xDist + yDist * yDist);
}
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB) {
	float xDist = positionB.x - positionA.x;
	float yDist = positionB.y - positionA.y;
	return (xDist * xDist + yDist * yDist);
}

//Geometry
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB) {
	
	return GetDistanceSquared2D(centerA, centerB) < (radiusA + radiusB)*(radiusA+radiusB);
}
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB) {
	return GetDistanceSquared3D(centerA, centerB) < (radiusA + radiusB) * (radiusA + radiusB);
}

//Tranform
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegree, Vec2 const& translation) {
	posToTransform *= uniformScale;
	posToTransform.RotateDegrees(rotationDegree);
	posToTransform += translation;
}
void TransformPositionXY3D(Vec3& posToTransform, float scaleXY, float zRotationDegree, Vec2 const& translationXY) {
	posToTransform.x *= scaleXY;
	posToTransform.y *= scaleXY;
	Vec3 result=posToTransform.GetRotatedAboutZDegrees(zRotationDegree);
	result.x += translationXY.x;
	result.y += translationXY.y;
	posToTransform = result;
}

//Clmap and lerp
float GetClamped(float val, float min, float max) {
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}
float GetClampedZeroToOne(float val) {
	if (val < 0.f) {
		return 0.f;
	}
	if (val > 1.f) {
		return 1.f;
	}
	return val;
}
float Interpolate(float start, float end, float fractionTowardEnd) {
	return start * (1 - fractionTowardEnd) + end * fractionTowardEnd;
}
float GetFractionWithinRange(float val, float rangeStart, float rangeEnd) {
	return(val - rangeStart) / (rangeEnd - rangeStart);
}
float RangeMap(float inVal, float inStart, float inEnd, float outStart, float outEnd) {
	float t = GetFractionWithinRange(inVal, inStart, inEnd);
	return Interpolate(outStart, outEnd, t);
}
float RangeMapClamped(float inVal, float inStart, float inEnd, float outStart, float outEnd) {
	float t = GetFractionWithinRange(inVal, inStart, inEnd);
	float toBeClamped= Interpolate(outStart, outEnd, t);
	if(outEnd>outStart){
		return GetClamped(toBeClamped, outStart, outEnd);
	}
	else {
		return GetClamped(toBeClamped, outEnd, outStart);
	}

}
int RoundDownToInt(float val) {
	return static_cast<int>(std::floorf(val));
}
float GetShortestAngularDispDegrees(float start, float end) {
	float disp =end - start;
	while(disp > 180) {
		disp -= 360;
	}
	while(disp < -180) {
		disp += 360;
	}
	return disp;
}
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees) {
	float deg = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	if (abs(deg) < maxDeltaDegrees) {
		return goalDegrees;
	}
	if (deg >= 0.0f) {
		return currentDegrees + maxDeltaDegrees;
	}
	else {
		return currentDegrees - maxDeltaDegrees;
	}
}
float DotProduct2D(Vec2 const& a, Vec2 const& b) {
	return a.x * b.x + a.y * b.y;
}
float DotProduct4D(Vec4 const& a, Vec4 const& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

int GetTaxicabDistance2D(IntVec2 const& A, IntVec2 const& B) {
	return abs(A.x - B.x) + abs(A.y - B.y);
}
float GetProjectedLength2D(Vec2 const& proj, Vec2 const& onto) {
	return DotProduct2D(proj, onto.GetNormalized());
}
Vec2 const GetProjectedOnto2D(Vec2 const& proj, Vec2 const& onto) {
	Vec2 normal = onto.GetNormalized();
	return DotProduct2D(proj, normal) * normal;
}
Vec3 const GetProjectedOnto3D(Vec3 const& proj, Vec3 const& onto) {
	Vec3 normal = onto.GetNormalized();
	return DotProduct3D(proj, normal) * normal;
}

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b) {
	float product = DotProduct2D(a.GetNormalized(), b.GetNormalized());
	product = GetClamped(product, -1, 1);
	return ConvertRadiansToDegrees(acosf(product));
}



bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& center, float radius) {
	return (point - center).GetLengthSquared() < radius * radius;
}
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& center, float sectorDeg, float sectorApDeg, float radius) {
	Vec2 sectorNormal = Vec2(1, 0);
	sectorNormal.SetOrientationDegrees(sectorDeg);
	return IsPointInsideDisc2D(point, center, radius) && GetAngleDegreesBetweenVectors2D((point - center), sectorNormal) < sectorApDeg/2;
}
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& center, Vec2 const& sectorNormal, float sectorApDeg, float radius) {
	return IsPointInsideDisc2D(point, center, radius) && GetAngleDegreesBetweenVectors2D((point - center), sectorNormal) < sectorApDeg/2;
}
Vec2 GetNearestPointOnDisc2D(Vec2 const& point, Vec2 const& center, float radius) {
	if (IsPointInsideDisc2D(point, center, radius)) {
		return point;
	}
	Vec2 normal = point - center;
	normal.Normalize();
	return center+normal * radius;
}

bool PushDiscOutOfPoint2D(Vec2& center, float radius, Vec2 const& point) {
	if (IsPointInsideDisc2D(center, point, radius)) {
		center = point+(center-point).GetNormalized()*radius;
		return true;
	}
	return false;
}
bool PushDiscOutOfDisc2D(Vec2& mobCenter, float radius, Vec2 const& center, float radius2) {
	if (IsPointInsideDisc2D(mobCenter, center, radius+radius2)) {
		mobCenter = center+(mobCenter - center).GetNormalized() * (radius + radius2);
		return true;
	}
	return false;
}
bool PushDiscsOutOfEachOther2D(Vec2& centerA, float radiusA, Vec2& centerB, float radiusB) {
	if (IsPointInsideDisc2D(centerA, centerB, radiusA+radiusB)) {
		Vec2 center = centerB+(centerA - centerB)*.5f;
		float rad = (radiusA + radiusB) *.5f;
		centerA= center+(centerA-center).GetNormalized()*rad;
		centerB = center+(centerB-center).GetNormalized()*rad;
		return true;
	}
	return false;
}
bool PushDiscOutOfAABB2D(Vec2& point, float radius, AABB2 const& box) {
	if (!box.IsPointInside(point)) {
		Vec2 nearest = box.GetNearestPoint(point);
		return PushDiscOutOfPoint2D(point, radius, nearest);
	}
	return false;
}

bool PushAABB3OutOfAABB3(AABB3& mobileBox, AABB3 const& staticBox, bool verticalPush)
{
	if (!DoAABB3sOverlap(mobileBox, staticBox)) {
	    return false;
	}

  if (verticalPush) {
      float dist = 0.f;
	  if (mobileBox.GetCenter().z >= staticBox.GetCenter().z) {
		  //push up
		  dist = staticBox.m_maxs.z - mobileBox.m_mins.z;
		  mobileBox.SetCenter(mobileBox.GetCenter()+Vec3(0.f,0.f, dist));
	  }
	  else {
		  //push down

		  dist =  mobileBox.m_maxs.z - staticBox.m_mins.z;
		  mobileBox.SetCenter(mobileBox.GetCenter() + Vec3(0.f, 0.f, -dist));
	  }
	  
  }
  else {
	  float xDist = 0.f;
	  float yDist = 0.f;
	  

	  if (mobileBox.GetCenter().x >= staticBox.GetCenter().x) {
		  xDist = staticBox.m_maxs.x - mobileBox.m_mins.x;
	  }
	  else {
		  xDist = -(mobileBox.m_maxs.x - staticBox.m_mins.x);
	  }

	  if (mobileBox.GetCenter().y >= staticBox.GetCenter().y) {
		  yDist = staticBox.m_maxs.y - mobileBox.m_mins.y;
	  }
	  else {
		  yDist = -(mobileBox.m_maxs.y - staticBox.m_mins.y);
	  }

	  if (fabsf(xDist) <= fabsf(yDist)) {
	      mobileBox.SetCenter(mobileBox.GetCenter() + Vec3(xDist, 0.f, 0.f));
	  }
	  else {
	      mobileBox.SetCenter(mobileBox.GetCenter() + Vec3(0.f, yDist, 0.f));
	  }
  }

  return true;
 
}

void TransformPosition2D(Vec2& posToTransform, Vec2 const& i, Vec2 const& j, Vec2 const& translation) {
	posToTransform = translation + posToTransform.x* i +posToTransform.y* j;
}
void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& i, Vec2 const& j, Vec2 const& translationXY) {
	Vec2 pos2D = Vec2(posToTransform.x, posToTransform.y);
	pos2D= translationXY + posToTransform.x * i + posToTransform.y * j;
	posToTransform.x = pos2D.x;
	posToTransform.y = pos2D.y;
}

Vec2 GetNearestPointOnLineSegment2D(Vec2 const& point, Vec2 const& start, Vec2 const& end) {
	Vec2 SP = point - start;
	Vec2 EP = point - end;
	Vec2 SE = end - start;
	if (DotProduct2D(SP, SE) <= 0) {
		return start;
	}
	if (DotProduct2D(EP, SE) >= 0) {
		return end;
	}
	return start+GetProjectedOnto2D(SP, SE);
}
Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& point, Vec2 const& start, Vec2 const& end) {
	Vec2 SP = point - start;
	Vec2 SE = end - start;
	return start+GetProjectedOnto2D(SP,SE);
}

bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius) {
	Vec2 NearestOnBone = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	Vec2 distance = point - NearestOnBone;
	return (distance.GetLengthSquared() < radius * radius);
}

Vec2 GetNearestPointOnCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius) {
	if (IsPointInsideCapsule2D(point, boneStart, boneEnd, radius)) {
		return point;
	}
	Vec2 NearestOnBone = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	Vec2 distance = point - NearestOnBone;
	distance.Normalize();
	return NearestOnBone + distance * radius;
}
bool IsPointInsideTriangle2D(Vec2 const& point, Vec2 const& A, Vec2 const& B, Vec2 const& C) {
	Vec2 ABNormal = (B - A).GetRotated90Degrees();
	Vec2 BCNormal = (C - B).GetRotated90Degrees();
	Vec2 CANormal = (A - C).GetRotated90Degrees();
	Vec2 ABMid = A+(B - A) / 2;
	Vec2 BCMid = B+(C - B) / 2;
	Vec2 CAMid = C+(A - C) / 2;
	if (DotProduct2D(point-ABMid, ABNormal) > 0) {
		if (DotProduct2D(point-BCMid, BCNormal) > 0) {
			if (DotProduct2D(point-CAMid, CANormal) > 0) {
				return true;
			}
		}
	}
	return false;
}
Vec2 GetNearestPointOnTriangle2D(Vec2 const& point, Vec2 const& A, Vec2 const& B, Vec2 const& C) {
	Vec2 ABNormal = (B - A).GetRotated90Degrees();
	Vec2 BCNormal = (C - B).GetRotated90Degrees();
	Vec2 CANormal = (A - C).GetRotated90Degrees();
	Vec2 ABMid = A+(B - A) / 2;
	Vec2 BCMid = B+(C - B) / 2;
	Vec2 CAMid = C+(A - C) / 2;
	bool insideAB = DotProduct2D(point - ABMid, ABNormal) > 0;
	bool insideBC = DotProduct2D(point - BCMid, BCNormal) > 0;
	bool insideCA = DotProduct2D(point - CAMid, CANormal) > 0;
	if (insideAB) {
		if (insideBC) {
			if (insideCA) {
				//case1: inside triangle
				return point;
			}
			else {
				//case2: outside CA
				return GetNearestPointOnLineSegment2D(point, C, A);
			}
		}
		else if(insideCA){
			//case3: outside BC
			return GetNearestPointOnLineSegment2D(point,B,C);
		}
		else {
			//case 4: BC,CA cross section
			return C;
		}
	}
	else if(insideBC){
		if (insideCA) {
			//case5: outside AB
			return GetNearestPointOnLineSegment2D(point, A, B);
		}
		else {
			//case 6: CA, AB cross section
			return A;
		}
	}
	else {
		//case 7: AB,BC cross section
		return B;
	}

}

bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& obb2) {
	Vec2 localPoint = obb2.GetLocalPosForWorldPos(point);
	if (abs(localPoint.x) < obb2.m_dimension.x) {
		if (abs(localPoint.y) < obb2.m_dimension.y) {
			return true;
		}
	}
	return false;
}
Vec2 GetNearestPointOnOBB2D(Vec2 const& point, OBB2 const& obb2) {
	Vec2 localPoint = obb2.GetLocalPosForWorldPos(point);
	if (localPoint.x < -obb2.m_dimension.x) {
		localPoint.x = -obb2.m_dimension.x;
	}
	if (localPoint.x > obb2.m_dimension.x) {
		localPoint.x = obb2.m_dimension.x;
	}
	if (localPoint.y < -obb2.m_dimension.y) {
		localPoint.y = -obb2.m_dimension.y;
	}
	if (localPoint.y > obb2.m_dimension.y) {
		localPoint.y = obb2.m_dimension.y;
	}
	return obb2.GetWorldPosForLocalPos(localPoint);

}
RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius) {
	RaycastResult2D result;
	bool didImpact = false;

	Vec2 SC = discCenter - startPos;
	Vec2 j = fwdNormal.GetRotated90Degrees();
	float SCj = DotProduct2D(SC, j);
	float SCi = DotProduct2D(SC, fwdNormal);
	if (SCj >= discRadius||SCj<=-discRadius) {
		didImpact = false;
	}
	else if(SCi>=maxDist+discRadius||SCi<=-discRadius){
		didImpact = false;
	}
	else {
		didImpact = true;
	}
	//------------if not impact, return
	if (!didImpact) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0, 0);
		result.m_impactPos = Vec2(0, 0);
		return result;

	}
	float adjust = sqrtf(discRadius*discRadius-SCj*SCj);
	result.m_impactDist = SCi - adjust;
	result.m_didImpact = true;
	result.m_impactPos = startPos + fwdNormal * result.m_impactDist;
	result.m_impactNormal = (result.m_impactPos - discCenter).GetNormalized();
	return result;

}

float DotProduct3D(Vec3 const& a, Vec3 const& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
float CrossProduct2D(Vec2 const& a, Vec2 const& b) {
	return a.x * b.y - a.y * b.x;
}
Vec3  CrossProduct3D(Vec3 const& a, Vec3 const& b) {
	return Vec3(
	    a.y*b.z-a.z*b.y,
		a.z*b.x-a.x*b.z,
		a.x*b.y-a.y*b.x
	);
}

float NormalizeByte(unsigned char byteValue) {
	if (byteValue == 255) {
		return 1.f;
	}
	return (float)(byteValue) / 256.f;
}

unsigned char DenormalizeByte(float zeroToOne) {
	if (zeroToOne == 1.f) {
		return 255;
	}
	return (unsigned char)round((float)zeroToOne*256.f);
}
RaycastResult2D RaycastVsLineSegment2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 lineStart, Vec2 lineEnd) {
	RaycastResult2D result;
	
	Vec2 j = fwdNormal.GetRotated90Degrees();
	Vec2 RS = lineStart - startPos;
	Vec2 RE = lineEnd - startPos;

	Vec2 RSj = GetProjectedOnto2D(RS, j);
	Vec2 REj = GetProjectedOnto2D(RE, j);
	Vec2 RSi = GetProjectedOnto2D(RS, fwdNormal);
	Vec2 REi = GetProjectedOnto2D(RE, fwdNormal);

	if (DotProduct2D(RSj,REj)>= 0.f) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0, 0);
		result.m_impactPos = Vec2(0, 0);
		return result;
	}
	//
	Vec2 EndPos = startPos + fwdNormal * maxDist;
	Vec2 lineNormal = (lineEnd - lineStart).GetNormalized();
	Vec2 lineJ = lineNormal.GetRotated90Degrees();
	Vec2 SR = startPos - lineStart;
	Vec2 SM = EndPos - lineStart;
	Vec2 SRj = GetProjectedOnto2D(SR, lineJ);
	Vec2 SMj = GetProjectedOnto2D(SM, lineJ);
	if (DotProduct2D(SRj, SMj) >= 0.f) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0, 0);
		result.m_impactPos = Vec2(0, 0);
		return result;
	}
	/*float RSiLen = GetProjectedLength2D(RS,fwdNormal);
	float REiLen = GetProjectedLength2D(RE, fwdNormal);
	if (RSi.GetLength()>= maxDist && REi.GetLength() >= maxDist) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0, 0);
		result.m_impactPos = Vec2(0, 0);
		return result;
	}else if(RSiLen <=0.f && REiLen <= 0.f){
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0, 0);
		result.m_impactPos = Vec2(0, 0);
		return result;
	}*/
	else {
		//portion of impact
		float t = RSj.GetLength() / (RSj.GetLength() + REj.GetLength());
		result.m_impactDist = RSi.GetLength() + t*(REi.GetLength() - RSi.GetLength());
		if (result.m_impactDist <= 0 || result.m_impactDist >= maxDist) {
			result.m_didImpact = false;
			result.m_impactDist = 0.f;
			result.m_impactNormal = Vec2(0, 0);
			result.m_impactPos = Vec2(0, 0);
			return result;
		}
		result.m_didImpact = true;
		result.m_impactPos = startPos + fwdNormal * result.m_impactDist;
		Vec2 perp = (lineEnd - lineStart).GetRotated90Degrees();
		if (DotProduct2D(perp, fwdNormal) < 0.f) {
			result.m_impactNormal = perp.GetNormalized();
		}
		else {
			result.m_impactNormal = - perp.GetNormalized();
		}
		return result;
	}	
}

RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 box) {
	RaycastResult2D result;
	if (box.IsPointInside(startPos)) {
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactNormal = -fwdNormal;
		result.m_impactPos = startPos;
		return result;
	}
	Vec2 endPosition = startPos + (fwdNormal * maxDist);

	AABB2 rayBoxAABB2(startPos, startPos);
	rayBoxAABB2.StretchToIncludePoint(endPosition);
	//reject clearly miss cases
	if (!box.DoesBoxOverlap(rayBoxAABB2)) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0.f, 0.f);
		result.m_impactPos = Vec2(0.f, 0.f);
		return result;
	}
	
	float xTraveled = fwdNormal.x * maxDist;
	float yTraveled = fwdNormal.y * maxDist;

	if (xTraveled == 0.f) {
		xTraveled = .0000000000001f;
	}

	if (yTraveled == 0.f) {
		yTraveled = .0000000000001f;
	}

	float minXIntersect = box.m_mins.x - startPos.x;
	float maxXIntersect = box.m_maxs.x - startPos.x;
	float minYIntersect = box.m_mins.y - startPos.y;
	float maxYIntersect = box.m_maxs.y - startPos.y;

	float minXTime = minXIntersect / xTraveled;
	float maxXTime = maxXIntersect / xTraveled;
	float minYTime = minYIntersect / yTraveled;
	float maxYTime = maxYIntersect / yTraveled;
	

	//Find enter and exit time--------------------------------------------
	float enterXTime = minXTime;
	float  exitXTime = maxXTime;
	float enterYTime = minYTime;
	float  exitYTime = maxYTime;


	if (minXTime > maxXTime) {
		enterXTime = maxXTime;
		 exitXTime = minXTime;
	}
	if (minYTime > maxYTime) {
		enterYTime = maxYTime;
		 exitYTime = minYTime;
	}
	//Adjust values-------------------------------------------
	//perpendicular
	if (xTraveled == 0) {
		enterXTime = 0;
		exitXTime = 1;
	}
	if (yTraveled == 0) {
		enterYTime = 0;
		exitYTime = 1;
	}
	//clamp
	if (enterXTime < 0)
		enterXTime = 0;
	if (enterYTime < 0)
		enterYTime = 0;
	if (exitXTime > 1)
		exitXTime = 1;
	if (exitYTime > 1)
		exitYTime = 1;
	//miss at angle
	if (enterXTime >= exitYTime || enterYTime >= exitXTime) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec2(0.f, 0.f);
		result.m_impactPos =    Vec2(0.f, 0.f);
		return result;
	}
	//Find enter time and which side it entered----------------------------------------------------
	float enterBoxTime = enterXTime;
	
	if (enterYTime > enterXTime) {
		enterBoxTime = enterYTime;
		if (yTraveled > 0.f) {
			result.m_impactNormal = Vec2(0.f, -1.f);
		}
		else {
			result.m_impactNormal = Vec2(0.f, 1.f);
		}
	}
	else {
		if (xTraveled > 0.f) {
			result.m_impactNormal = Vec2(-1.f, 0.f);
		}
		else {
			result.m_impactNormal = Vec2(1.f, 0.f);
		}
	}

	result.m_didImpact = true;
	result.m_impactDist = maxDist*enterBoxTime;
	result.m_impactPos = startPos + fwdNormal * result.m_impactDist;
	
	return result;
}

Mat44 GetBillboardTransform(BillboardType billboardType, Mat44 const& targetTransform, const Vec3& billboardPosition, const Vec2& billboardScale) {
	Mat44 result;
	switch (billboardType) {
	  case BillboardType::FULL_FACING:
	  	  result=Mat44::MakeLookAtTransform(billboardPosition, targetTransform.GetTranslation3D());
	  	  break;
	  case BillboardType::FULL_OPPPOSING:
	  	  result=Mat44::MakeFullOpposing(targetTransform);
	  	  break;
	  case BillboardType::WORLD_UP_FACING:
	  	  result=Mat44::MakeWorldUpFacing(billboardPosition, targetTransform.GetTranslation3D());
	  	  break;
	  case BillboardType::WORLD_UP_OPPOSING:
	  	  Vec3 IBasis = targetTransform.GetIBasis3D().GetNormalized();
	  	  IBasis.z = 0.f;
	  	  Vec3 KBasis = Vec3(0.f, 0.f, 1.f);
	  	  Vec3 JBasis = CrossProduct3D(KBasis, IBasis);
	  	  result=Mat44(IBasis, JBasis, KBasis, Vec3(0.f, 0.f, 0.f));
	  	  break;
	}
	result.AppendScaleNonUniform3D(Vec3(1.f, billboardScale.x, billboardScale.y));
	return result;
}

bool IsPointInsideAABB3D(Vec3 const& point, AABB3 const& cube) {
	return cube.IsPointInside(point);
}
bool IsPointInsideZCylinder3D(Vec3 const& point, Vec3 const& bottom, float radius, FloatRange const& ZRange) {
	bool horrizontalAlign = IsPointInsideDisc2D(Vec2(point.x, point.y), Vec2(bottom.x, bottom.y),radius);
	bool verticalAlign = ZRange.IsOnRange(point.z);
	return horrizontalAlign && verticalAlign;
}
bool IsPointInsideSphere3D(Vec3 const& point, Vec3 const& center, float radius) {
	return radius>GetDistanceSquared3D(center, point);
}

bool DoAABB3sOverlap(AABB3 const& cubeA, AABB3 const& cubeB) {
	FloatRange xRange(cubeA.m_mins.x, cubeA.m_maxs.x);
	FloatRange yRange(cubeA.m_mins.y, cubeA.m_maxs.y);
	FloatRange zRange(cubeA.m_mins.z, cubeA.m_maxs.z);

	FloatRange otherXRange(cubeB.m_mins.x, cubeB.m_maxs.x);
	FloatRange otherYRange(cubeB.m_mins.y, cubeB.m_maxs.y);
	FloatRange otherZRange(cubeB.m_mins.z, cubeB.m_maxs.z);

	bool xOverlap = xRange.IsOverlappingWith(otherXRange) || otherXRange.IsOverlappingWith(xRange);
	bool yOverlap = yRange.IsOverlappingWith(otherYRange) || otherYRange.IsOverlappingWith(yRange);
	bool zOverlap = zRange.IsOverlappingWith(otherZRange) || otherZRange.IsOverlappingWith(zRange);
	return xOverlap && yOverlap && zOverlap;
}

bool DoZCylindersOverlap(Vec3 const& bottomA, float radiusA, FloatRange const& ZRangeA,
	Vec3 const& bottomB, float radiusB, FloatRange const& ZRangeB) {
	return DoDiscsOverlap(Vec2(bottomA.x, bottomA.y), radiusA, Vec2(bottomB.x, bottomB.y), radiusB)&&
		    (ZRangeA.IsOverlappingWith(ZRangeB)||ZRangeB.IsOverlappingWith(ZRangeA));
}
bool DoSphereOverlapAABB3(Vec3 const& center, float radius, AABB3 const& cube) {
	Vec3 nearestPoint = cube.GetNearestPoint(center);
	return GetDistanceSquared3D(center, nearestPoint) < radius * radius;
}
bool DoSphereOverlapZCylinder(Vec3 const& sphereCenter, float sphereRadius,
	Vec3 const& cylinderBottom, float cylinderRadius, FloatRange const& cylinderZRange) {
	Vec3 nearestPoint = GetNearestPointOnZCylinder3D(sphereCenter, cylinderBottom, cylinderRadius, cylinderZRange);
	return GetDistanceSquared3D(sphereCenter, nearestPoint) < sphereRadius * sphereRadius;
}
bool DoAABB3OverlapZCylinder(AABB3 const& cube, Vec3 const& cylinderBottom, float cylinderRadius, FloatRange const& cylinderZRange) {
	FloatRange cubeZrange(cube.m_mins.z, cube.m_maxs.z);
	bool ZOverlap = cylinderZRange.IsOverlappingWith(cubeZrange) || cubeZrange.IsOverlappingWith(cylinderZRange);
	if (!ZOverlap) {
		return false;
	}
	else {
		AABB2 cubeHorizontal(Vec2(cube.m_mins.x, cube.m_mins.y), Vec2(cube.m_maxs.x, cube.m_maxs.y));
		Vec2 CylinderCenter(cylinderBottom.x, cylinderBottom.y);
		return GetDistanceSquared2D(CylinderCenter, cubeHorizontal.GetNearestPoint(CylinderCenter)) < cylinderRadius * cylinderRadius;
	}
}

Vec3 GetNearestPointOnSphere3D(Vec3 const& point, Vec3 const& center, float radius) {
	if (IsPointInsideSphere3D(point, center, radius)) {
		return point;
	}
	else return center + (point - center).GetNormalized() * radius;
}
Vec3 GetNearestPointOnZCylinder3D(Vec3 const& point, Vec3 const& bottom, float radius, FloatRange const& ZRange) {
	if (IsPointInsideZCylinder3D(point, bottom, radius, ZRange)) {
		return point;
	}
	else {
		float resultZ;
		Vec2 resultXY;
		if (point.z < ZRange.m_min) {
			resultZ = ZRange.m_min;
		}
		else if (point.z > ZRange.m_max) {
			resultZ = ZRange.m_max;
		}
		else {
			resultZ = point.z;
		}
		resultXY = GetNearestPointOnDisc2D(Vec2(point.x, point.y), Vec2(bottom.x, bottom.y), radius);
		return Vec3(resultXY.x, resultXY.y, resultZ);
	}
}

RaycastResult3D RaycastVsSphere3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 center, float radius) {
	RaycastResult3D result;
	if (IsPointInsideSphere3D(startPos, center, radius)) {
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactNormal = -fwdNormal;
		result.m_impactPos = startPos;
		return result;
	}
	Vec3 SC = center - startPos;
	Vec3 j = CrossProduct3D(fwdNormal, Vec3(0.f, 0.f, 1.f)).GetNormalized();
	Vec3 k = CrossProduct3D(fwdNormal, j).GetNormalized();
	float SCi = DotProduct3D(SC, fwdNormal);
	float SCj = DotProduct3D(SC, j);
	float SCk = DotProduct3D(SC, k);

	if (SCj*SCj+SCk*SCk >= radius * radius) {;
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	else if (SCi >= maxDist + radius || SCi <= -radius) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	float adjust = sqrtf(radius * radius - SCj * SCj - SCk*SCk);
	result.m_impactDist = SCi - adjust;
	result.m_didImpact = true;
	result.m_impactPos = startPos + fwdNormal * result.m_impactDist;
	result.m_impactNormal = (result.m_impactPos - center).GetNormalized();
	return result;
	
}
RaycastResult3D RaycastVsAABB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, AABB3 cube) {
	RaycastResult3D result;
	if (cube.IsPointInside(startPos)) {
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactNormal = -fwdNormal;
		result.m_impactPos = startPos;
		return result;
	}
	Vec3 endPosition = startPos + (fwdNormal * maxDist);

	AABB3 rayBoxAABB3(startPos, startPos);
	rayBoxAABB3.StretchToIncludePoint(endPosition);
	//reject clearly miss cases
	if (!DoAABB3sOverlap(cube,rayBoxAABB3)) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}

	float xTraveled = fwdNormal.x * maxDist;
	float yTraveled = fwdNormal.y * maxDist;
	float zTraveled = fwdNormal.z * maxDist;
	// divide by 0 prevention
	if (xTraveled == 0.f) {
		xTraveled = .0000000000001f;
	}

	if (yTraveled == 0.f) {
		yTraveled = .0000000000001f;
	}

	if (zTraveled == 0.f) {
		zTraveled = .0000000000001f;
	}
	float minXIntersect = cube.m_mins.x - startPos.x;
	float maxXIntersect = cube.m_maxs.x - startPos.x;
	float minYIntersect = cube.m_mins.y - startPos.y;
	float maxYIntersect = cube.m_maxs.y - startPos.y;
	float minZIntersect = cube.m_mins.z - startPos.z;
	float maxZIntersect = cube.m_maxs.z - startPos.z;

	float minXTime = minXIntersect / xTraveled;
	float maxXTime = maxXIntersect / xTraveled;
	float minYTime = minYIntersect / yTraveled;
	float maxYTime = maxYIntersect / yTraveled;
	float minZTime = minZIntersect / zTraveled;
	float maxZTime = maxZIntersect / zTraveled;


	//Find enter and exit time--------------------------------------------
	float enterXTime = fminf(minXTime, maxXTime);
	float  exitXTime = fmaxf(minXTime, maxXTime);
	float enterYTime = fminf(minYTime, maxYTime);
	float  exitYTime = fmaxf(minYTime, maxYTime);
	float enterZTime = fminf(minZTime, maxZTime);
	float  exitZTime = fmaxf(minZTime, maxZTime);

	//Adjust values-------------------------------------------
	//clamp
	if (enterXTime < 0.f)
		enterXTime = 0.f;
	if (enterYTime < 0.f)
		enterYTime = 0.f;
	if (enterZTime < 0.f)
		enterZTime = 0.f;

	if (exitXTime > 1.f)
		exitXTime = 1.f;
	if (exitYTime > 1.f)
		exitYTime = 1.f;
	if (exitZTime > 1.f)
		exitZTime = 1.f;

	float enterTime = fmaxf(enterXTime,enterYTime);
	enterTime = fmaxf(enterTime, enterZTime);
	float exitTime = fminf(exitXTime, exitYTime);
	exitTime = fminf(exitTime, exitZTime);
	//miss at angle
	if (enterTime>=exitTime) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	//Find enter time and which side it entered----------------------------------------------------
	if (enterTime == enterXTime) {
		if (fwdNormal.x > 0) {
			result.m_impactNormal = Vec3(-1.f, 0.f, 0.f);
		}
		else {
			result.m_impactNormal = Vec3(1.f, 0.f, 0.f);
		}
	}
	else if (enterTime == enterYTime) {
		if (fwdNormal.y > 0) {
			result.m_impactNormal = Vec3(0.f, -1.f, 0.f);
		}
		else {
			result.m_impactNormal = Vec3(0.f, 1.f, 0.f);
		}
	}
	else if (enterTime == enterZTime) {
		if (fwdNormal.z > 0) {
			result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
		}
		else {
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
		}
	}

	result.m_didImpact = true;
	result.m_impactDist = maxDist * enterTime;
	result.m_impactPos = startPos + fwdNormal * result.m_impactDist;

	return result;
}
RaycastResult3D RaycastVsZCylinder3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 bottom, float radius, FloatRange ZRange) {
	RaycastResult3D result;
	if (IsPointInsideZCylinder3D(startPos,bottom,radius,ZRange)) {
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactNormal = -fwdNormal;
		result.m_impactPos = startPos;
		return result;
	}
	//vertical
	if (fwdNormal.z>=0.f&&startPos.z>=ZRange.m_max) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	else if (fwdNormal.z<=0.f&&startPos.z<=ZRange.m_min) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	float zTraveled = fwdNormal.z * maxDist;
	float minZIntersect = ZRange.m_min - startPos.z;
	float maxZIntersect = ZRange.m_max - startPos.z;

	if (zTraveled == 0.f) {
		zTraveled = .0000000000001f;
	}

	float minZTime = minZIntersect / zTraveled;
	float maxZTime = maxZIntersect / zTraveled;

	float enterZTime = fminf(minZTime, maxZTime);
	float  exitZTime = fmaxf(minZTime, maxZTime);

	if (enterZTime < 0.f)
		enterZTime = 0.f;
	if (exitZTime > 1.f)
		exitZTime = 1.f;
	//horizontal
	Vec2 horinzontalNormal = Vec2(fwdNormal.x, fwdNormal.y).GetNormalized();
	float horrinzontalDist = maxDist*(Vec2(fwdNormal.x, fwdNormal.y).GetLength() / 1.f);
	RaycastResult2D horizontalResult = RaycastVsDisc2D(Vec2(startPos.x, startPos.y), horinzontalNormal, horrinzontalDist, Vec2(bottom.x, bottom.y), radius);
	if (!horizontalResult.m_didImpact) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	float enterCycleTime = horizontalResult.m_impactDist / horrinzontalDist;
	Vec2 centerToImpact = Vec2(bottom.x, bottom.y) - horizontalResult.m_impactPos;
	float CII = DotProduct2D(centerToImpact, horinzontalNormal);
	float exitCycleTime = (horizontalResult.m_impactDist + 2.f * CII) / horrinzontalDist;
	if (exitCycleTime > 1.f)
		exitCycleTime = 1.f;
	if (enterZTime >= exitCycleTime || enterCycleTime >= exitZTime) {
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
	result.m_didImpact = true;
	result.m_impactDist = (maxDist * fmaxf(enterCycleTime, enterZTime));
	result.m_impactPos = startPos + fwdNormal * result.m_impactDist;
	//case: enter from side
	if (enterCycleTime > enterZTime) {
		result.m_impactNormal = Vec3(horizontalResult.m_impactNormal.x, horizontalResult.m_impactNormal.y, 0.f);
	}
	else {
		if (fwdNormal.z > 0.f)
			result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
		else
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
	}
	return result;
}

float Lerp(float A, float B, float C, float t) {
	float AB = A+(B - A) * t;
	float BC = B+(C - B) * t;
	return BC * t + (1.f - t) * AB;;
}

float ComputeCubicBezier1D(float A, float B, float C, float D, float t) {
	float AC = Lerp(A, B, C, t);
	float BD = Lerp(B, C, D, t);
	return (BD * t) + (1.f-t) * AC;
}
float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t) {
	float AD = ComputeCubicBezier1D(A, B, C, D, t);
	float BE = ComputeCubicBezier1D(B, C, D, E, t);
	float CF = ComputeCubicBezier1D(C, D, E, F, t);
	return Lerp(AD, BE, CF, t);
}
void BounceDiscOffDisc2D(Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2 staticCenter, float staticRadius, float staticElasticity) {
	bool pushed = PushDiscOutOfDisc2D(center, radius, staticCenter, staticRadius);
	if (pushed) {
		Vec2 closest = GetNearestPointOnDisc2D(center, staticCenter, staticRadius);
		BounceDiscOffPoint(center, radius, elasticity, velocity, closest, staticElasticity);
	}
}
void BounceDiscOffOther2D(Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2& centerB, float radiusB, float elasticityB, Vec2& velocityB) {
	UNUSED(elasticityB);
	if (DoDiscsOverlap(center, radiusB, centerB, radiusB)) {
		bool pushed = PushDiscsOutOfEachOther2D(center, radius, centerB, radiusB);
		if (pushed) {
			Vec2 sumVelocityA = velocity - velocityB;
			Vec2 BToA = centerB - center;
			if (DotProduct2D(sumVelocityA, BToA) > 0.f) {
				Vec2 ExchangedA = GetProjectedOnto2D(velocity, (centerB - center));
				Vec2 ExcahngedB = GetProjectedOnto2D(velocityB, (center - centerB));
				velocity -= ExchangedA;
				velocity += ExcahngedB * elasticity;
				velocityB -= ExcahngedB;
				velocityB += ExchangedA * elasticity;
			}
		}
	}
	
}
void BounceDiscOffOBB2D(Vec2& center, float radius, float elasticity, Vec2& velocity, OBB2 box, float staticElasticity) {
	if (!IsPointInsideOBB2D(center, box)) {
		Vec2 nearest = GetNearestPointOnOBB2D(center, box);
		bool pushed = PushDiscOutOfPoint2D(center, radius, nearest);
		if (pushed) {
			BounceDiscOffPoint(center, radius, elasticity, velocity, nearest, staticElasticity);
		}
	}
	
}
void BounceDiscOffCapsule2D(Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2 staticHead, Vec2 staticTail, float staticRadius, float staticElasticity) {
//	if (!IsPointInsideCapsule2D(center, staticHead, staticTail, staticRadius)) {
		Vec2 nearest = GetNearestPointOnCapsule2D(center, staticHead, staticTail, staticRadius);
		bool pushed = PushDiscOutOfPoint2D(center, radius, nearest);
		if (pushed) {
			BounceDiscOffPoint(center, radius, elasticity, velocity, nearest, staticElasticity);
	    }
	//}
}
void BounceDiscOffPoint(Vec2& center, float radius, float elasticity, Vec2& velocity, Vec2 point, float staticElasticity) {
	if ((center - point).GetLengthSquared() <= radius * radius) {
		if(DotProduct2D(velocity,(point-center))>0.f)
		   velocity -=  GetProjectedOnto2D(velocity, (center - point))*(1.f+elasticity*staticElasticity);
	}
}

float PointDistToPlane(Vec3 const& point, Plane3 const& plane) {
	float projectLen = DotProduct3D(point, plane.m_normal);
	return projectLen - plane.m_dist;
}

bool IsPointInsideOBB3D(Vec3 const& point, OBB3 const& cube) {
	Vec3 pointInLocal = cube.GetLocalPosForWorldPos(point);
	AABB3 localCube = cube.GetAsLocalAABB3();
	localCube.m_mins += *cube.m_center;
	localCube.m_maxs += *cube.m_center;
	return localCube.IsPointInside(pointInLocal);
}

bool IsPointInFrontPlane3D(Vec3 const& point, Plane3 const& plane) {
	return PointDistToPlane(point, plane) > 0.f;
}

Vec3 GetNearestPointOnOBB3D(Vec3 const& point, OBB3 const& cube) {
	Vec3 pointInLocal = cube.GetLocalPosForWorldPos(point);
	AABB3 localCube = cube.GetAsLocalAABB3();
	Vec3 result = localCube.GetNearestPoint(pointInLocal);
	return cube.GetWorldPosForLocalPos(result);
}

Vec3 GetNearestPointOnPlane3D(Vec3 const& point, Plane3 const& plane) {
	Vec3 normal = plane.m_normal;
	return point - (normal * PointDistToPlane(point, plane));
}

RaycastResult3D RayCastVSOBB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, OBB3 cube) {
	AABB3 localCube = cube.GetAsLocalAABB3();
	//localCube.m_mins += *cube.m_center;
	//localCube.m_maxs += *cube.m_center;
	Vec3 localStart = cube.GetTransformMatrix().GetOrthonormalInverse().TransformPosition3D(startPos);
	Vec3 localFWD = cube.m_orientation->GetAsMatrix_IFwd_JLeft_KUp().GetOrthonormalInverse().TransformVectorQuantity3D(fwdNormal);
	RaycastResult3D localResult = RaycastVsAABB3D(localStart, localFWD, maxDist, localCube);
	localResult.m_impactPos = cube.GetTransformMatrix().TransformPosition3D(localResult.m_impactPos);
	localResult.m_impactNormal = cube.GetTransformMatrix().TransformVectorQuantity3D(localResult.m_impactNormal);
	return localResult;
}

RaycastResult3D RayCastVSPlane3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3 plane) {
	Vec3 projection = GetProjectedOnto3D((fwdNormal * maxDist), plane.m_normal);
	if (IsPointInFrontPlane3D(startPos,plane)!=IsPointInFrontPlane3D(startPos+fwdNormal*maxDist,plane)) {
		Vec3 localFWDVector = GetProjectedOnto3D(fwdNormal, plane.m_normal);
		float time = fabsf(PointDistToPlane(startPos, plane) / localFWDVector.GetLength());
		RaycastResult3D result;
		result.m_didImpact = true;
		result.m_impactDist = fabsf(time*fwdNormal.GetLength());
		result.m_impactNormal = IsPointInFrontPlane3D(startPos,plane)? plane.m_normal: -plane.m_normal;
		result.m_impactPos = startPos+fwdNormal*time;
		return result;
	}
	else {
		RaycastResult3D result;
		result.m_didImpact = false;
		result.m_impactDist = 0.f;
		result.m_impactNormal = Vec3();
		result.m_impactPos = Vec3();
		return result;
	}
}

bool DoOBB3OverlapSphere3D(Vec3 const& center, float radius, OBB3 const& cube) {
	Vec3 nearest = GetNearestPointOnOBB3D(center, cube);
	return (nearest - center).GetLengthSquared() < radius * radius;
}

bool DoOBB3OverlapPlane3D(OBB3 const& cube, Plane3 const& plane) {
	AABB3 localCube = cube.GetAsLocalAABB3();

	std::vector<Vec3> corners = localCube.GetCorners();

	bool inFront = false;
	bool inBack = false;

	for (Vec3 corner : corners) {
		if (IsPointInFrontPlane3D(cube.GetTransformMatrix().TransformPosition3D(corner),plane)){
			inFront = true;
		}
		else {
			inBack = true;
		}
	}

	return inFront && inBack;
}

bool DoAABB3OverlapPlane3D(AABB3 const& cube, Plane3 const& plane) {
	std::vector<Vec3> corners = cube.GetCorners();

	bool inFront = false;
	bool inBack = false;

	for (Vec3 corner : corners) {
		if (IsPointInFrontPlane3D(corner, plane)) {
			inFront = true;
		}
		else {
			inBack = true;
		}
	}

	return inFront && inBack;
}

bool DoSphereOverlapPlane3D(Vec3 const& center, float radius, Plane3 plane) {
	return fabsf(PointDistToPlane(center, plane)) < radius;
}