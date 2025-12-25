#include "Engine/Math/Vec3.hpp"
//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>

//-----------------------------------------------------------------------------------------------
Vec3::Vec3(Vec3 const& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}



//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( Vec3 const& vecToAdd) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z+vecToAdd.z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-(Vec3 const& vecToSubtract) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y,z-vecToSubtract.z);
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y,-z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*(float uniformScale) const
{
	return Vec3(x * uniformScale, y * uniformScale,z*uniformScale);
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*(Vec3 const& vecToMultiply) const
{
	return Vec3(x * vecToMultiply.x, y * vecToMultiply.y,z*vecToMultiply.z);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/(float inverseScale) const
{
	return Vec3(x / inverseScale, y / inverseScale, z/inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=(Vec3 const& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=(Vec3 const& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=(Vec3 const& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*(float uniformScale, const Vec3& vecToScale)
{
	return Vec3(uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale*vecToScale.z);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==(Vec3 const& compare) const
{
	return (x == compare.x && y == compare.y && z == compare.z);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=(Vec3 const& compare) const
{
	return !(x == compare.x && y == compare.y && z == compare.z);
}

//A2
float Vec3::GetLength()const {
	return(sqrtf(x*x+y*y+z*z));
}
float Vec3::GetLengthXY()const {
	return (sqrtf(x * x + y * y));
}
float Vec3::GetLengthSquared()const {
	return x * x + y * y + z * z;
}
float Vec3::GetLengthXYSquared()const {
	return x * x + y * y;
}
float Vec3::GetAngleAboutZRadians()const {
	return atan2f(y, x);
}
float Vec3::Vec3::GetAngleAboutZDegrees()const {
	return Atan2Degrees(y, x);
}
Vec3 const Vec3::GetRotatedAboutZRadians(float deltaRadians)const {
	float newRad = GetAngleAboutZRadians() + deltaRadians;
	float length = GetLengthXY();
	float newX = cosf(newRad) * length;
	float newY = sinf(newRad) * length;
	return Vec3(newX, newY,z);

}
Vec3 const Vec3::GetRotatedAboutZDegrees(float deltaDegrees)const {
	float newDeg = GetAngleAboutZDegrees() + deltaDegrees;
	float length = GetLengthXY();
	float newX = CosDegrees(newDeg) * length;
	float newY = SinDegrees(newDeg) * length;
	return Vec3(newX, newY,z);
}
Vec3 const Vec3::GetClamped(float maxLength)const {
	if (GetLengthSquared() > maxLength * maxLength) {
		return GetNormalized() * maxLength;
	}
	return Vec3(this->x,this->y,this->z);
}
Vec3 const Vec3::GetNormalized()const {
	if (GetLength() == 0.f) {
		return Vec3(0.f, 0.f, 0.f);
	}
	float scale = 1/GetLength();
	return Vec3(x * scale, y * scale, z * scale);
}

const Vec3 Vec3::MakeFromPolarDegree(float yaw, float pitch, float len) {
	Vec3 vector;
	vector.x = CosDegrees(pitch) * CosDegrees(yaw)*len;
	vector.y = CosDegrees(pitch) * SinDegrees(yaw)*len;
	vector.z = -SinDegrees(pitch)*len;
	return vector;
}

void Vec3::SetFromText(char const* text) {
	Strings stringArr = SplitStringOnDelimiter(text, ',');
	x = (float)(atof(stringArr[0].data()));
	y = (float)(atof(stringArr[1].data()));
	z = (float)(atof(stringArr[2].data()));
}

Vec2 const Vec3::GetAsVec2()const {
	return Vec2(x, y);
}