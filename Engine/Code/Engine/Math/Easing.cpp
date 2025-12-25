#include "Easing.hpp"
#include "MathUtils.hpp"
float SmoothStart2(float t) {
	return Lerp(0.f, 0.f, 1.f, t);
}
float SmoothStart3(float t) {
	return ComputeCubicBezier1D(0.f, 0.f, 0.f, 1.f, t);
}
float SmoothStart4(float t) {
	return ComputeCubicBezier1D(0.f, 0.f, 0.f, 1.f, t) * t;
}
float SmoothStart5(float t) {
	return ComputeQuinticBezier1D(0.f, 0.f, 0.f, 0.f, 0.f, 1.f, t);
}
float SmoothStart6(float t) {
	return ComputeQuinticBezier1D(0.f, 0.f, 0.f, 0.f, 0.f, 1.f, t) * t;
}

float SmoothStop2(float t) {
	return Lerp(0.f, 1.f, 1.f, t);
}
float SmoothStop3(float t) {
	return ComputeCubicBezier1D(0.f, 1.f, 1.f, 1.f, t);
}
float SmoothStop4(float t) {
	float x = 1.f - t;
	return 1 - (x * x* x * x);
}
float SmoothStop5(float t) {
	return ComputeQuinticBezier1D(0.f, 1.f, 1.f, 1.f, 1.f, 1.f, t);
}
float SmoothStop6(float t) {
	float x = 1.f - t;
	return 1 - x * x * x * x * x * x;
}

float SmoothStep3(float t) {
	return ComputeCubicBezier1D(0.f, 0.f, 1.f, 1.f, t);
}
float SmoothStep5(float t) {
	return ComputeQuinticBezier1D(0.f, 0.f, 0.f, 1.f, 1.f, 1.f, t);
}

float Hesitate3(float t) {
	return ComputeCubicBezier1D(0.f, 1.f, 0.f, 1.f, t);
}
float Hesitate5(float t) {
	return ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f, 1.f, t);
}

float CustomFunkyEasingFunction(float t) {
	return ComputeQuinticBezier1D(0.f, .5f, -1.f, 2.f, -2.f, 1.f, t);
}