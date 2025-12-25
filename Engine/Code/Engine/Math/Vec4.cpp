#include "Engine/Math/Vec4.hpp"
//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
//-----------------------------------------------------------------------------------------------
Vec4::Vec4(Vec4 const& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4(float initialX, float initialY, float initialZ,float initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}
//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator + (Vec4 const& vecToAdd) const
{
	return Vec4(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z,w+vecToAdd.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-(Vec4 const& vecToSubtract) const
{
	return Vec4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z,w-vecToSubtract.w);
}


//------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-() const
{
	return Vec4(-x, -y, -z,-w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*(float uniformScale) const
{
	return Vec4(x * uniformScale, y * uniformScale, z * uniformScale,w*uniformScale);
}


//------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*(Vec4 const& vecToMultiply) const
{
	return Vec4(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z,w*vecToMultiply.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/(float inverseScale) const
{
	return Vec4(x / inverseScale, y / inverseScale, z / inverseScale, w/inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=(Vec4 const& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=(Vec4 const& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=(Vec4 const& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
const Vec4 operator*(float uniformScale, const Vec4& vecToScale)
{
	return Vec4(uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z,uniformScale*vecToScale.w);
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==(Vec4 const& compare) const
{
	return (x == compare.x && y == compare.y && z == compare.z&&w==compare.w);
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=(Vec4 const& compare) const
{
	return !(x == compare.x && y == compare.y && z == compare.z&&w==compare.w);
}

//A2
float Vec4::GetLength()const {
	return(sqrtf(x * x + y * y + z * z+ w * w));
}
float Vec4::GetLengthSquared()const {
	return x * x + y * y + z * z+w*w;
}

Vec4 const Vec4::GetClamped(float maxLength)const {
	if (GetLengthSquared() > maxLength * maxLength) {
		return GetNormalized() * maxLength;
	}
	return Vec4(x,y,z,w);
}
Vec4 const Vec4::GetNormalized()const {
	float scale = 1 / GetLength();
	return Vec4(x * scale, y * scale, z * scale,w*scale);
}