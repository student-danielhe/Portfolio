#include "Engine/Math/CurveLinear1D.hpp"

float CurveLinear1D::Evaluate(float t)
{
    return t*(m_endVal-m_startVal) + m_startVal;
}
