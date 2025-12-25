#pragma once
#include "Vec2.hpp"
struct Capsule2 {
public:
	Vec2 m_start;
	Vec2 m_end;
	float m_radius;
	Capsule2() {}
	explicit Capsule2(Vec2 const& start, Vec2 const& end, float radius);
	void Translate(Vec2 const& trnaslate);
	void SetCenter(Vec2 const& center);
	void RotateAboutCenter(float degree);
};