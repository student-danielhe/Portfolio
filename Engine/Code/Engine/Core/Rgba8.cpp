#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
const Rgba8 Rgba8::WHITE{255,255,255,255};
const Rgba8 Rgba8::GRAY{ 100,100,100,255 };
const Rgba8 Rgba8::BLACK{0,0,0,255};
const Rgba8 Rgba8::RED  {255,0,0,255};
const Rgba8 Rgba8::GREEN{0,255,0,255};
const Rgba8 Rgba8::BLUE {0,0,255,255};
//-----------------------------------------------------------------------------------------------
Rgba8::Rgba8(const Rgba8& copy)
	: r(copy.r)
	, g(copy.g)
	, b(copy.b)
	, a(copy.a)
{
}


//-----------------------------------------------------------------------------------------------
Rgba8::Rgba8(unsigned char r, unsigned char	g, unsigned char b, unsigned char a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{
}
void Rgba8::SetFromText(char const* text) {
	Strings stringArr = SplitStringOnDelimiter(text, ',');
	r = static_cast<unsigned char>(atof(stringArr[0].data()));
	g = static_cast<unsigned char>(atof(stringArr[1].data()));
	b = static_cast<unsigned char>(atof(stringArr[2].data()));
	if (stringArr.size()>=4) {
		a = static_cast<unsigned char>(atof(stringArr[3].data()));
	}
	else {
		a = 255;
	}
	
}
Rgba8 Rgba8::InterpolateColor(Rgba8 start, Rgba8 end, float fraction) {
	float red = static_cast<float>(start.r) + fraction * (static_cast<float>(end.r) - static_cast<float>(start.r));
	float green = static_cast<float>(start.g) + fraction * (static_cast<float>(end.g) - static_cast<float>(start.g));
	float blue = static_cast<float>(start.b) + fraction * (static_cast<float>(end.b) - static_cast<float>(start.b));
	float a = static_cast<float>(start.a) + fraction * (static_cast<float>(end.a) - static_cast<float>(start.a));
	Rgba8 result(static_cast<unsigned char>(RoundDownToInt(red)), static_cast<unsigned char>(RoundDownToInt(green)), static_cast<unsigned char>(RoundDownToInt(blue)));
	result.a = static_cast<unsigned char>(RoundDownToInt(a));
	return result;
}
void Rgba8::GetAsFloat(float* colorAsFloats)const {
	colorAsFloats[0] = r / 255.f;
	colorAsFloats[1] = g / 255.f;
	colorAsFloats[2] = b / 255.f;
	colorAsFloats[3] = a / 255.f;
}
bool Rgba8::operator==(const Rgba8& compare) const
{
	return r == compare.r && g == compare.g && b==compare.b && a==compare.a;
}