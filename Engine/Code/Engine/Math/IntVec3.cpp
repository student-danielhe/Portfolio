#include "IntVec3.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
IntVec3::IntVec3(IntVec3 const& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(copyFrom.z)
{
}							// copy constructor (from another vec2)
IntVec3::IntVec3(int initialX, int initialY, int initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}		// explicit constructor (from x, y)


void IntVec3::operator=(const IntVec3& copyFrom) {
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

void IntVec3::SetFromText(char const* text) {
	Strings stringArr = SplitStringOnDelimiter(text, ',');
	x = atoi(stringArr[0].data());
	y = atoi(stringArr[1].data());
	z = atoi(stringArr[2].data());
}

IntVec3 IntVec3::operator+(const IntVec3& add) const
{
	return IntVec3(x + add.x, y + add.y, z + add.z);
}



bool IntVec3::operator<(const IntVec3& cmp) const noexcept
{
    if (z < cmp.z)
    {
	    return true;
    }

	if (z > cmp.z) {
		return false;
	}

	if (y < cmp.y) {
		return true;
	}

	if (y > cmp.y) {
		return false;
	}

	return x < cmp.x;
}

bool IntVec3::operator==(const IntVec3& compare) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}


//-----------------------------------------------------------------------------------------------
bool IntVec3::operator!=(const IntVec3& compare) const
{
	return !(x == compare.x && y == compare.y && z == compare.z);
}