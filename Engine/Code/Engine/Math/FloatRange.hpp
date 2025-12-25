#pragma once
#include "Vec2.hpp"
struct FloatRange {
public:
	float m_min=0.0f;
	float m_max=0.0f;
	FloatRange(){}
	explicit FloatRange(float min, float max);
	FloatRange(const Vec2& copy);
	bool		operator==(FloatRange const& compare) const;		
	bool		operator!=(FloatRange const& compare) const;		
	void		operator=(FloatRange const& copyFrom);
	bool IsOnRange(float compare) const;
	bool IsOverlappingWith(FloatRange const& compare) const;
	static const FloatRange ZERO;
	static const FloatRange ONE;
	static const FloatRange ZERO_TO_ONE;
	void SetFromText(char const* text = "0,0");
};