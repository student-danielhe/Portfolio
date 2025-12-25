#include"Engine/Math/EulerAngles.hpp"
#include "Engine/Core/StringUtils.hpp"
EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees):
m_yawDegrees(yawDegrees),
m_pitchDegrees(pitchDegrees),
m_rollDegrees(rollDegrees)
{

}
void  EulerAngles::GetAsVectors_IFwd_JLeft_KUp(Vec3& out_fwdIbasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) {
	Mat44 matrix = GetAsMatrix_IFwd_JLeft_KUp();
	out_fwdIbasis =matrix.GetIBasis3D();
	out_leftJBasis = matrix.GetJBasis3D();
	out_upKBasis = matrix.GetKBasis3D();

}
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp()const {
	
	Mat44 result = Mat44();
	result.Append(Mat44::MakeZRotationDegrees(m_yawDegrees));
	result.Append(Mat44::MakeYRotationDegrees(m_pitchDegrees));
	result.Append(Mat44::MakeXRotationDegrees(m_rollDegrees));

	return result;
}

void EulerAngles::SetFromText(char const* text) {
	Strings stringArr = SplitStringOnDelimiter(text, ',');
	m_yawDegrees = (float)(atof(stringArr[0].data()));
	m_pitchDegrees = (float)(atof(stringArr[1].data()));
	m_rollDegrees = (float)(atof(stringArr[2].data()));
}