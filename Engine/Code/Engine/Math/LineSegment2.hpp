#pragma once
#include "Vec2.hpp"
struct LineSegment2 {
public:
	Vec2 m_start;
	Vec2 m_end;


	LineSegment2() {}
	explicit LineSegment2(Vec2 const& start, Vec2 const& end);
	void Translate(Vec2 const& translate);
	void SetCenter(Vec2 const& center);
	void RotateAboutCenter(float degree);

};