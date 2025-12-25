#include "Engine/Math/IntRange.hpp"
const IntRange IntRange:: ZERO{ 0,0 };
const IntRange IntRange::ONE{ 1,1 };
const IntRange IntRange::ZERO_TO_ONE{ 0,1 };

IntRange::IntRange(int min, int max)
	: m_min(min)
	, m_max(max)
{
}
bool IntRange:: operator==(IntRange const& compare) const {
	return m_min == compare.m_min && m_max == compare.m_max;
}
bool IntRange:: operator!=(IntRange const& compare) const {
	return !(m_min == compare.m_min && m_max == compare.m_max);
}
void IntRange:: operator=(IntRange const& copyFrom) {
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

bool IntRange:: IsOnRange(int compare) const {
	return compare <= m_max && compare >= m_min;
}
bool IntRange:: IsOverlappingWith(IntRange const& compare) const{
	return IsOnRange(compare.m_min) || IsOnRange(compare.m_max);
}