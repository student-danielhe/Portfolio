#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
const FloatRange FloatRange:: ZERO{ 0.0f,0.0f };
const FloatRange FloatRange::ONE{ 1.0f,1.0f };
const FloatRange FloatRange::ZERO_TO_ONE{ 0.0f,1.0f };

FloatRange::FloatRange(float min, float max)
	: m_min(min)
	, m_max(max)
{}
FloatRange::FloatRange(const Vec2& copy)
   : m_min(copy.x)
   , m_max(copy.y)
{}
bool FloatRange:: operator==(FloatRange const& compare) const {
	return m_min == compare.m_min && m_max == compare.m_max;
}
bool FloatRange:: operator!=(FloatRange const& compare) const {
	return !(m_min == compare.m_min && m_max == compare.m_max);
}
void FloatRange:: operator=(FloatRange const& copyFrom) {
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

bool FloatRange:: IsOnRange(float compare) const {
	return compare <= m_max && compare >= m_min;
}
bool FloatRange:: IsOverlappingWith(FloatRange const& compare) const{
	return IsOnRange(compare.m_min) || IsOnRange(compare.m_max);
}
void FloatRange::SetFromText(char const* text) {
 	Strings stringArr = SplitStringOnDelimiter(text, '~');
	m_min = (float)(atof(stringArr[0].data()));
	m_max = (float)(atof(stringArr[1].data()));
}