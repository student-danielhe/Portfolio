#include "Engine/Math/Vec2.hpp"
//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Vec3.hpp"
//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}

Vec2::Vec2(const IntVec2& copy)
	: x((float)copy.x)
	, y((float)copy.y)
{
}
Vec2::Vec2(const Vec3& copy)
	: x((float)copy.x)
	, y((float)copy.y) {
}
//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX)
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x+vecToAdd.x, y+vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2(x - vecToSubtract.x, y -vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x*uniformScale, y*uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x*vecToMultiply.x, y*vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x/inverseScale, y/inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( uniformScale*vecToScale.x, uniformScale*vecToScale.y);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return x==compare.x&&y==compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return !(x == compare.x && y == compare.y);
}

//A2
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadian, float length) 
{
	return Vec2((cosf(orientationRadian)*length),sinf(orientationRadian)*length );
}
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegree, float length) {
	float radian = ConvertDegreesToRadians(orientationDegree);
	return Vec2((cosf(radian) * length), (sinf(radian) * length));
}

//Accessors
float Vec2::GetLength()const {
	return sqrtf(x*x + y*y);
}
float Vec2::GetLengthSquared()const {
	return x * x + y * y;
}
float Vec2::GetOrientationRadians()const {
	return atan2f(y, x);
}
float Vec2::GetOrientationDegrees()const {
	return Atan2Degrees(y, x);
}
Vec2 const Vec2::GetRotated90Degrees()const {
	return Vec2(-y,x);
}
Vec2 const Vec2::GetRotatedMinus90Degrees()const {
	return Vec2(y, -x);
}
Vec2 const Vec2::GetRotatedRadians(float deltaRadians)const {
	float newRad=GetOrientationRadians() + deltaRadians;
	float length = GetLength();
	float newX = cosf(newRad)*length;
	float newY = sinf(newRad)*length;
	return Vec2(newX, newY);
}
Vec2 const Vec2::GetRotatedDegrees(float deltaDegrees)const {
	float newDeg = GetOrientationDegrees() + deltaDegrees;
	float length = GetLength();
	float newX = CosDegrees(newDeg)*length;
	float newY = SinDegrees(newDeg)*length;
	return Vec2(newX, newY);
}
Vec2 const Vec2::GetClamped(float maxLength)const {
	if (GetLengthSquared() > maxLength * maxLength) {
		return GetNormalized() * maxLength;
	}
	else return Vec2(x, y);
}
Vec2 const Vec2::GetNormalized()const {
	if (GetLengthSquared() == 0) {
		return Vec2(0.f,0.f);
	}
	float scale = 1.f / GetLength();
	return Vec2(x * scale, y * scale);
}

//Mutators
void Vec2::SetOrientationRadians(float newOrientationRadians) {
	float length = GetLength();
	x = cosf(newOrientationRadians) * length;
	y = sinf(newOrientationRadians) * length;
}
void Vec2::SetOrientationDegrees(float newOrientationDegrees) {
	float length = GetLength();
	x = CosDegrees(newOrientationDegrees) * length;
	y = SinDegrees(newOrientationDegrees) * length;
}
void Vec2::SetPolarRadians(float newPolarRadians, float newLength) {
	x = cosf(newPolarRadians) * newLength;
	y = sinf(newPolarRadians) * newLength;
}
void Vec2::SetPolarDegrees(float newPolarDegrees, float newLength) {
	x = CosDegrees(newPolarDegrees) * newLength;
	y = SinDegrees(newPolarDegrees) * newLength;
}
void Vec2::Rotate90Degrees() {
	float oldX = x;
	x = -y;
	y = oldX;
}
void Vec2::RotateMinus90Degrees() {
	float oldX = x;
	x = y;
	y = -oldX;
}
void Vec2::RotateRadians(float deltaRadians) {
	float newRad = GetOrientationRadians()+deltaRadians;
	float length = GetLength();
	x = cosf(newRad) * length;
	y = sinf(newRad) * length;
}
void Vec2::RotateDegrees(float deltaDegrees) {
	float newDeg = GetOrientationDegrees() + deltaDegrees;
	float length = GetLength();
	x = CosDegrees(newDeg) * length;
	y = SinDegrees(newDeg) * length;
}
void Vec2::ClampLength(float maxLength) {
	if (GetLengthSquared() > maxLength * maxLength) {
		Normalize();
		x *= maxLength;
		y *= maxLength;
	}
}
void Vec2::Normalize() {
	if (GetLengthSquared() == 0) {
		return;
	}
	float scale = 1 / GetLength();
	x *= scale;
	y *= scale;
}
float Vec2::NormalizeAndGetPreviousLength() {
	float length = GetLength();
	float scale = 1 / length;
	x *= scale;
	y *= scale;
	return length;
}
void Vec2::SetLength(float length) {
	float rad = GetOrientationRadians();
	x = cosf(rad) * length;
	y = sinf(rad) * length;
}

Vec2 const Vec2::GetReflected(Vec2 const& surface) const{
	return -DotProduct2D(*this, surface) * surface + DotProduct2D(*this, surface.GetRotated90Degrees()) * surface.GetRotated90Degrees();
}
void Vec2::Reflect(Vec2 const& surface) {
	Vec2 result = -DotProduct2D(*this, surface) * surface + DotProduct2D(*this, surface.GetRotated90Degrees()) * surface.GetRotated90Degrees();
	x = result.x;
	y = result.y;
}

void Vec2::SetFromText(char const* text) {
	Strings stringArr = SplitStringOnDelimiter(text, ',');
	x = (float)(atof(stringArr[0].data()));
	y = (float)(atof(stringArr[1].data()));
}

