#pragma once
#include "Engine/Math/Curve1D.hpp"
#include <map>

class CurvePiecewise :public Curve1D {
    public:
    std::map<float, float> m_keyPoints;
    CurvePiecewise(std::map<float, float> keyPoints): m_keyPoints(keyPoints){}
    float Evaluate(float t);
};