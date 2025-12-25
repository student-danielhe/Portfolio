#include "Engine/Math/IntVec2.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
IntVec2::IntVec2(IntVec2 const& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
{}							// copy constructor (from another vec2)
IntVec2::IntVec2(int initialX, int initialY)
    :x(initialX)
    ,y(initialY)
{}		// explicit constructor (from x, y)

//Accessors
float IntVec2::GetLength()const{
	float fx = static_cast<float>(x);
	float fy = static_cast<float>(y);
	return sqrtf( (fx * fx) + (fy * fy));
}
int IntVec2::GetTaxicabLength()const {
	return abs(x) + abs(y);
}
int IntVec2::GetLengthSquared()const{
	return x * x + y * y;
}
float IntVec2::GetOrientationRadians()const{
	return atan2f( static_cast<float>(y), static_cast<float>(x));
}
float IntVec2::GetOrientationDegrees()const{
	return Atan2Degrees( static_cast<float>(y), static_cast<float>(x));
}
IntVec2 const IntVec2::GetRotated90Degrees()const{
	return IntVec2(-y, x);
}
IntVec2 const IntVec2::GetRotatedMinus90Degrees()const{
	return IntVec2(y, -x);
}


//Mutators
void IntVec2::Rotate90Degrees(){
	int oldX = x;
	x = -y;
	y = oldX;
}
void IntVec2::RotateMinus90Degrees(){
	int oldX = x;
	x = y;
	y = -oldX;
}
void IntVec2::operator=(const IntVec2& copyFrom) {
	x = copyFrom.x;
	y = copyFrom.y;
}

void IntVec2::SetFromText(char const* text) {
	Strings stringArr = SplitStringOnDelimiter(text, ',');
	x = atoi(stringArr[0].data());
	y = atoi(stringArr[1].data());
}

IntVec2 IntVec2::operator+(const IntVec2& add) const
{
    return IntVec2(x+add.x, y+add.y);
}

bool IntVec2::operator==(const IntVec2& compare) const
{
	return x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator!=(const IntVec2& compare) const
{
	return !(x == compare.x && y == compare.y);
}