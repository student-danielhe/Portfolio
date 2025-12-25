#include "Engine/Math/CurvePiecewise.hpp"

float CurvePiecewise::Evaluate(float t)
{
    float prev = -1.f;
    float next = 1.f;
    float prevVal = 0.f;
    float nextVal = 0.f;
    for (auto pair : m_keyPoints) {
        if (pair.first<t) {
            if (prev < pair.first) {
				prev = pair.first;
				prevVal = pair.second;
            }
                
        }
        if (pair.first == t) {
            return pair.second;
        }
        if (pair.first > t) {
            if (next > pair.first) {
				next = pair.first;
				nextVal = pair.second;
            }
        }
    }
    return (t - prev)/(next - prev) * (nextVal - prevVal) + prevVal;
}
