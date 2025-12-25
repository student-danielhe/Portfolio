#pragma once
//-----------------------------------------
struct Rgba8
{
public:
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    unsigned char a = 255;
public:
    ~Rgba8() {}												// destructor (do nothing)
    Rgba8() {}												// default constructor (do nothing)
    Rgba8(const Rgba8& copyFrom);							// copy constructor (from another vec2)
    explicit Rgba8(unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
    void SetFromText(char const* text);
    static Rgba8 InterpolateColor(Rgba8 start, Rgba8 end, float fraction);
    void GetAsFloat(float* colorAsFloats)const;
    bool		operator==(const Rgba8& compare) const;

    static const Rgba8 WHITE; 
    static const Rgba8 GRAY;
    static const Rgba8 BLACK; 
    static const Rgba8 RED; 
    static const Rgba8 GREEN; 
    static const Rgba8 BLUE; 
};
