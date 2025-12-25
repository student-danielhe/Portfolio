#include "Engine/Math/Mat44.hpp"
#include"Engine/Math/MathUtils.hpp"
Mat44::Mat44() {
	for (int i = 0; i < 16; i++) {
		m_values[i] = .0f;
	}
	m_values[Ix] = 1.f;
	m_values[Jy] = 1.f;
	m_values[Kz] = 1.f;
	m_values[Tw] = 1.f;
}
Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D) {
	for (int i = 0; i < 16; i++) {
		m_values[i] = .0f;
	}
	m_values[Ix] = 1.f;
	m_values[Jy] = 1.f;
	m_values[Kz] = 1.f;
	m_values[Tw] = 1.f;
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
}
Mat44::Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D) {
	for (int i = 0; i < 16; i++) {
		m_values[i] = .0f;
	}
	m_values[Ix] = 1.f;
	m_values[Jy] = 1.f;
	m_values[Kz] = 1.f;
	m_values[Tw] = 1.f;
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;
}
Mat44::Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D) {
	for (int i = 0; i < 16; i++) {
		m_values[i] = .0f;
	}
	m_values[Ix] = 1.f;
	m_values[Jy] = 1.f;
	m_values[Kz] = 1.f;
	m_values[Tw] = 1.f;
	m_values[Ix] = iBasis4D.x;
	m_values[Iy] = iBasis4D.y;
	m_values[Iz] = iBasis4D.z;
	m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x;
	m_values[Jy] = jBasis4D.y;
	m_values[Jz] = jBasis4D.z;
	m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x;
	m_values[Ky] = kBasis4D.y;
	m_values[Kz] = kBasis4D.z;
	m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translation4D.x;
	m_values[Ty] = translation4D.y;
	m_values[Tz] = translation4D.z;
	m_values[Tw] = translation4D.w;
}
Mat44::Mat44(float const* ValuesBasisMajor) {
	for (int i = 0; i < 16; i++) {
		m_values[i] = ValuesBasisMajor[i];
	}
}
//----------------------------------------------
Mat44 const Mat44::MakeTranslation2D(Vec2 const& translationXY) {
	Mat44 newMatrix;
	newMatrix.m_values[Tx] = translationXY.x;
	newMatrix.m_values[Ty] = translationXY.y;
	return newMatrix;
}
Mat44 const Mat44::MakeTranslation3D(Vec3 const& translationXYZ) {
	Mat44 newMatrix;
	newMatrix.m_values[Tx] = translationXYZ.x;
	newMatrix.m_values[Ty] = translationXYZ.y;
	newMatrix.m_values[Tz] = translationXYZ.z;
	return newMatrix;
}
Mat44 const Mat44::MakeUniformScale2D(float uniformScaleXY) {
	Mat44 newMatrix;
	newMatrix.m_values[Ix] *= uniformScaleXY;
	newMatrix.m_values[Jy] *= uniformScaleXY;
	return newMatrix;

}
Mat44 const Mat44::MakeUniformScale3D(float uniformScaleXYZ) {
	Mat44 newMatrix;
	newMatrix.m_values[Ix] *= uniformScaleXYZ;
	newMatrix.m_values[Jy] *= uniformScaleXYZ;
	newMatrix.m_values[Kz] *= uniformScaleXYZ;
	return newMatrix;
}
Mat44 const Mat44::MakeNonUniformScale2D(Vec2 const& nonUniformScaleXY) {
	Mat44 newMatrix;
	newMatrix.m_values[Ix] *= nonUniformScaleXY.x;
	newMatrix.m_values[Jy] *= nonUniformScaleXY.y;
	return newMatrix;
}
Mat44 const Mat44::MakeNonUniformScale3D(Vec3 const& nonUniformScaleXYZ) {
	Mat44 newMatrix;
	newMatrix.m_values[Ix] *= nonUniformScaleXYZ.x;
	newMatrix.m_values[Jy] *= nonUniformScaleXYZ.y;
	newMatrix.m_values[Kz] *= nonUniformScaleXYZ.z;
	return newMatrix;
}
Mat44 const Mat44::MakeZRotationDegrees(float rotationDegreesAboutZ) {
	Mat44 newMatrix;
	newMatrix.m_values[Ix] = CosDegrees(rotationDegreesAboutZ);
	newMatrix.m_values[Iy] = SinDegrees(rotationDegreesAboutZ);
	newMatrix.m_values[Jx] = -SinDegrees(rotationDegreesAboutZ);
	newMatrix.m_values[Jy] = CosDegrees(rotationDegreesAboutZ);
	return newMatrix;
}
Mat44 const Mat44::MakeYRotationDegrees(float rotationDegreesAboutY) {
	Mat44 newMatrix;
	newMatrix.m_values[Kz] = CosDegrees(rotationDegreesAboutY);
	newMatrix.m_values[Kx] = SinDegrees(rotationDegreesAboutY);
	newMatrix.m_values[Iz] = -SinDegrees(rotationDegreesAboutY);
	newMatrix.m_values[Ix] = CosDegrees(rotationDegreesAboutY);
	return newMatrix;
}
Mat44 const Mat44::MakeXRotationDegrees(float rotationDegreesAboutX) {
	Mat44 newMatrix;
	newMatrix.m_values[Jy] = CosDegrees(rotationDegreesAboutX);
	newMatrix.m_values[Jz] = SinDegrees(rotationDegreesAboutX);
	newMatrix.m_values[Ky] = -SinDegrees(rotationDegreesAboutX);
	newMatrix.m_values[Kz] = CosDegrees(rotationDegreesAboutX);
	return newMatrix;
}
//-----------------------------
Vec2 const Mat44::TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const{

	Vec4 Xs(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]);
	Vec4 Ys(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]);
	Vec4 position(vectorQuantityXY.x, vectorQuantityXY.y, 0, 0);
	return Vec2(DotProduct4D(position, Xs), DotProduct4D(position, Ys));
}
Vec3 const Mat44::TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const{

	Vec4 Xs(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]);
	Vec4 Ys(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]);
	Vec4 Zs(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]);
	Vec4 position(vectorQuantityXYZ.x, vectorQuantityXYZ.y, vectorQuantityXYZ.z, 0);
	return Vec3(DotProduct4D(position, Xs), DotProduct4D(position, Ys), DotProduct4D(position, Zs));
}
Vec2 const Mat44::TransformPosition2D(Vec2 const& positionXY) const{
	Vec4 Xs(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]);
	Vec4 Ys(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]);
	Vec4 position (positionXY.x,positionXY.y,0,1);
	return Vec2(DotProduct4D(position,Xs),DotProduct4D(position,Ys));
}
Vec3 const Mat44::TransformPosition3D(Vec3 const& positionXYZ) const{
	Vec4 Xs(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]);
	Vec4 Ys(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]);
	Vec4 Zs(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]);
	Vec4 position (positionXYZ.x,positionXYZ.y,positionXYZ.z,1);
	return Vec3(DotProduct4D(position, Xs), DotProduct4D(position, Ys), DotProduct4D(position, Zs));
}
Vec4 const Mat44::TransformHomogeneous3D(Vec4 const& homogeneousPoint3D) const{
	Vec4 Xs(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]);
	Vec4 Ys(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]);
	Vec4 Zs(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]);
	Vec4 Ws(m_values[Iw], m_values[Jw], m_values[Kw], m_values[Tw]);
	Vec4 position = homogeneousPoint3D;
	position.x = DotProduct4D(homogeneousPoint3D,Xs);
	position.y = DotProduct4D(homogeneousPoint3D,Ys);
	position.z = DotProduct4D(homogeneousPoint3D,Zs);
	position.w = DotProduct4D(homogeneousPoint3D,Ws);
	return position;
}
//--------------------------
float* Mat44::GetAsFloatArray() {
	return m_values;
}
float const* Mat44::GetAsFloatArray()const {
	return m_values;
}
Vec2 const Mat44::GetIBasis2D()const {
	return Vec2(m_values[Ix], m_values[Iy]);
}
Vec2 const Mat44::GetJBasis2D()const {
	return Vec2(m_values[Jx], m_values[Jy]);
}
Vec2 const Mat44::GetTranslation2D()const {
	return Vec2(m_values[Tx], m_values[Ty]);
}
Vec3 const Mat44::GetIBasis3D()const {
	return Vec3(m_values[Ix],m_values[Iy],m_values[Iz]);
}
Vec3 const Mat44::GetJBasis3D()const {
	return Vec3(m_values[Jx],m_values[Jy],m_values[Jz]);
}
Vec3 const Mat44::GetKBasis3D()const {
	return Vec3(m_values[Kx],m_values[Ky],m_values[Kz]);
}
Vec3 const Mat44::GetTranslation3D()const {
	return Vec3(m_values[Tx],m_values[Ty],m_values[Tz]);
}
Vec4 const Mat44::GetIBasis4D()const {
	return Vec4(m_values[Ix],m_values[Iy],m_values[Iz],m_values[Iw]);
}
Vec4 const Mat44::GetJBasis4D()const {
	return Vec4(m_values[Jx],m_values[Jy],m_values[Jz],m_values[Jw]);
}
Vec4 const Mat44::GetKBasis4D()const {
	return Vec4(m_values[Kx],m_values[Ky],m_values[Kz],m_values[Kw]);
}
Vec4 const Mat44::GetTranslation4D()const {
	return Vec4(m_values[Tx],m_values[Ty],m_values[Tz],m_values[Tw]);
}

void Mat44::SetTranslation2D(Vec2 const& translationXY) {
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = .0f;
	m_values[Tw] = 1.f;
}
void Mat44::SetTranslation3D(Vec3 const& translationXYZ) {
	m_values[Tx] = translationXYZ.x;
	m_values[Ty] = translationXYZ.y;
	m_values[Tz] = translationXYZ.z;
	m_values[Tw] = 1.f;
}
void Mat44::SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D) {
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = .0f;
	m_values[Iw] = .0f;
	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = .0f;
	m_values[Jw] = .0f;
}
void Mat44::SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY) {
	SetIJ2D(iBasis2D, jBasis2D);
	SetTranslation2D(translationXY);
}
void Mat44::SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D) {
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = .0f;
	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = .0f;
	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = .0f;
}
void Mat44::SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 translationXYZ) {
	SetIJK3D(iBasis3D, jBasis3D, kBasis3D);
	SetTranslation3D(translationXYZ);
}
void Mat44::SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 translationXYZW) {
	m_values[Ix] = iBasis4D.x;
	m_values[Iy] = iBasis4D.y;
	m_values[Iz] = iBasis4D.z;
	m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x;
	m_values[Jy] = jBasis4D.y;
	m_values[Jz] = jBasis4D.z;
	m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x;
	m_values[Ky] = kBasis4D.y;
	m_values[Kz] = kBasis4D.z;
	m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translationXYZW.x;
	m_values[Ty] = translationXYZW.y;
	m_values[Tz] = translationXYZW.z;
	m_values[Tw] = translationXYZW.w;
}

void Mat44::Append(Mat44 const& appendThis) {
	Vec4 IBasis=appendThis.GetIBasis4D();
	Vec4 JBasis=appendThis.GetJBasis4D();
	Vec4 KBasis=appendThis.GetKBasis4D();
	Vec4 TBasis=appendThis.GetTranslation4D();
	Vec4 Xs(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]);
	Vec4 Ys(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]);
	Vec4 Zs(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]);
	Vec4 Ws(m_values[Iw], m_values[Jw], m_values[Kw], m_values[Tw]);
	m_values[Ix] = DotProduct4D(IBasis, Xs);
	m_values[Jx] = DotProduct4D(JBasis, Xs);
	m_values[Kx] = DotProduct4D(KBasis, Xs);
	m_values[Tx] = DotProduct4D(TBasis, Xs);
	m_values[Iy] = DotProduct4D(IBasis, Ys);
	m_values[Jy] = DotProduct4D(JBasis, Ys);
	m_values[Ky] = DotProduct4D(KBasis, Ys);
	m_values[Ty] = DotProduct4D(TBasis, Ys);
	m_values[Iz] = DotProduct4D(IBasis, Zs);
	m_values[Jz] = DotProduct4D(JBasis, Zs);
	m_values[Kz] = DotProduct4D(KBasis, Zs);
	m_values[Tz] = DotProduct4D(TBasis, Zs);
	m_values[Iw] = DotProduct4D(IBasis, Ws);
	m_values[Jw] = DotProduct4D(JBasis, Ws);
	m_values[Kw] = DotProduct4D(KBasis, Ws);
	m_values[Tw] = DotProduct4D(TBasis, Ws);
}
void Mat44::AppendZRotation(float degreeRotionAboutZ) {
	Mat44 appendThis = Mat44::MakeZRotationDegrees(degreeRotionAboutZ);
	Append(appendThis);
}
void Mat44::AppendYRotation(float degreeRotionAboutY) {
	Mat44 appendThis = Mat44::MakeYRotationDegrees(degreeRotionAboutY);
	Append(appendThis);
}
void Mat44::AppendXRotation(float degreeRotionAboutX) {
	Mat44 appendThis = Mat44::MakeXRotationDegrees(degreeRotionAboutX);
	Append(appendThis);
}
void Mat44::AppendTranslation2D(Vec2 const& translationXY) {
	Mat44 appendThis = Mat44::MakeTranslation2D(translationXY);
	Append(appendThis);
}
void Mat44::AppendTranslation3D(Vec3 const& translationXYZ) {
	Mat44 appendThis = Mat44::MakeTranslation3D(translationXYZ);
	Append(appendThis);
}
void Mat44::AppendScaleUniform2D(float uniformScaleXY) {
	Mat44 appendThis = Mat44::MakeUniformScale2D(uniformScaleXY);
	Append(appendThis);
}
void Mat44::AppendScaleUniform3D(float uniformScaleXYZ) {
	Mat44 appendThis = Mat44::MakeUniformScale3D(uniformScaleXYZ);
	Append(appendThis);
}
void Mat44::AppendScaleNonUniform2D(Vec2 const& nonUniformaScaleXY) {
	Mat44 appendThis = Mat44::MakeNonUniformScale2D(nonUniformaScaleXY);
	Append(appendThis);
}
void Mat44::AppendScaleNonUniform3D(Vec3 const& nonUniformaScaleXYZ) {
	Mat44 appendThis = Mat44::MakeNonUniformScale3D(nonUniformaScaleXYZ);
	Append(appendThis);
}

Mat44 const Mat44::MakeOrthoProjection(float left, float right, float bottom, float up, float zNear, float zFar) {
	float xScale = 2/(right-left);
	float yScale = 2/(up-bottom);
	float zScale = 1/(zFar-zNear);
	float transX = -(right+left)/(right-left);
	float transY = -(up+bottom)/(up-bottom);
	float transZ = 0-(zNear*zScale);

	Vec3 IBasis(xScale, 0, 0);
	Vec3 JBasis(0, yScale, 0);
	Vec3 KBasis(0, 0, zScale);
	Vec3 Translation(transX, transY, transZ);
	return Mat44(IBasis, JBasis, KBasis, Translation);
}
Mat44 const Mat44::MakePerspectiveProjection(float fovYdegree, float aspect, float zNear, float zFar) {
	Mat44 perspective;

	float c = CosDegrees(fovYdegree * .5f);
	float s = SinDegrees(fovYdegree * .5f);
	float scaleY = c / s;
	float scaleX = scaleY / aspect;
	float scaleZ = zFar / (zFar - zNear);
	float translateZ = (zNear * zFar) / (zNear - zFar);

	perspective.m_values[Ix] = scaleX;
	perspective.m_values[Jy] = scaleY;
	perspective.m_values[Kz] = scaleZ;
	perspective.m_values[Kw] = 1.f;
	perspective.m_values[Tz] = translateZ;
	perspective.m_values[Tw] = 0.f;

	return perspective;
}
Mat44 const Mat44::GetOrthonormalInverse()const {
	
	Vec3 trans(-m_values[Tx], -m_values[Ty], -m_values[Tz]);
	Mat44 TInverse = MakeTranslation3D(trans);
	float RVal[16];
	for (int i = Ix; i < Tx; i++) {
		RVal[i] = m_values[i];
	}
	RVal[Tx] = 0;
	RVal[Ty] = 0;
	RVal[Tz] = 0;
	RVal[Tw] = 1;
	Mat44 RInverse(RVal);
	RInverse.Transpose();
	RInverse.Append(TInverse);
	return RInverse;
}
void Mat44::Transpose() {
	float newVals[16];
	newVals[Ix] = m_values[Ix];
	newVals[Iy] = m_values[Jx];
	newVals[Iz] = m_values[Kx];
	newVals[Iw] = m_values[Tx];
	newVals[Jx] = m_values[Iy];
	newVals[Jy] = m_values[Jy];
	newVals[Jz] = m_values[Ky];
	newVals[Jw] = m_values[Ty];
	newVals[Kx] = m_values[Iz];
	newVals[Ky] = m_values[Jz];
	newVals[Kz] = m_values[Kz];
	newVals[Kw] = m_values[Tz];
	newVals[Tx] = m_values[Iw];
	newVals[Ty] = m_values[Jw];
	newVals[Tz] = m_values[Kw];
	newVals[Tw] = m_values[Tw];
	for (int i = 0; i < 16; i++) {
		m_values[i] = newVals[i];
	}
}
void Mat44::Orthonormalize_IFwd_JLeft_KUp() {
	Vec4 IBasis(m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw]);
	Vec4 JBasis(m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw]);
	Vec4 KBasis(m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw]);
	Vec4 Trans(m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw]);

	IBasis = IBasis.GetNormalized();
	Vec4 Ki = DotProduct4D(KBasis, IBasis) * IBasis;
	KBasis = KBasis - Ki;
	KBasis = KBasis.GetNormalized();
	Vec4 Ji = DotProduct4D(JBasis, IBasis) * IBasis;
	Vec4 Jk = DotProduct4D(JBasis, KBasis) * KBasis;
	JBasis = JBasis - Ji - Jk;
	JBasis = JBasis.GetNormalized();
	float* newVal=Mat44(IBasis, JBasis, KBasis, Trans).m_values;
	for (int i = 0; i < 16; i++) {
		m_values[i] = newVal[i];
	}
}


Mat44 const Mat44::MakeLookAtTransform(Vec3 start, Vec3 end) {
	Vec3 IBasis = (end - start).GetNormalized();
	if (IBasis == Vec3(0.f, 0.f, 1.f)) {
		IBasis.GetRotatedAboutZDegrees(.01f);
	}
	Vec3 JBasis = CrossProduct3D(Vec3(0.f, 0.f, 1.f), IBasis);
	
	Vec3 KBasis;
	if (DotProduct3D(IBasis, JBasis) < 1.f&& DotProduct3D(IBasis, JBasis) > -1.f) {
		JBasis = JBasis.GetNormalized();
		KBasis = CrossProduct3D(IBasis, JBasis);
	}
	else {
		KBasis = CrossProduct3D(IBasis, Vec3(1.f, 0.f, 0.f));
		JBasis = CrossProduct3D(KBasis, IBasis);
	}
	return Mat44(IBasis,JBasis,KBasis,Vec3(0.f,0.f,0.f));
}

Mat44 const Mat44::MakeWorldUpFacing(Vec3 start, Vec3 end) {
	Vec3 IBasis = (start-end).GetNormalized();
	IBasis.z = 0.f;
	Vec3 KBasis = Vec3(0.f, 0.f, 1.f);
	Vec3 JBasis = CrossProduct3D(KBasis, IBasis);
	return Mat44(IBasis, JBasis, KBasis, Vec3(0.f, 0.f, 0.f));
}

Mat44 const Mat44::MakeFullOpposing(Mat44 transform) {
	Vec3 IBasis = -transform.GetIBasis3D().GetNormalized();
	Vec3 KBasis = transform.GetKBasis3D().GetNormalized();
	Vec3 JBasis = transform.GetJBasis3D().GetNormalized();
	return Mat44(IBasis, JBasis, KBasis, Vec3(0.f, 0.f, 0.f));
}