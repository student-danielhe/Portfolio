#include "Engine/Renderer/Camera.hpp"
void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight, float zNear, float zFar){
	m_orthographicTopRight = topRight;
	m_orthographicBottomLeft = bottomLeft;
	m_mode = eMode_Orthographic;
	m_orthographicNear = zNear;
	m_orthographicFar = zFar;
}

void Camera::SetPerspectiveView(float aspect, float fov, float zNear, float zFar) {
	m_perspectiveAspect = aspect;
	m_perspectiveFOV = fov;
	m_perspectiveNear = zNear;
	m_perspectiveFar = zFar;
	m_mode = eMode_Perspective;
}
Vec2 Camera::GetOrthoBottomLeft() const {
	return m_orthographicBottomLeft;
}
Vec2 Camera::GetOrthoTopRight() const {
	return m_orthographicTopRight;
}
Vec2 Camera::GetDimensions() const {
	return Vec2(m_orthographicTopRight.x-m_orthographicBottomLeft.x,m_orthographicTopRight.y-m_orthographicBottomLeft.y);
}
void Camera::Translate2D(Vec2 const& translate) {
	m_orthographicBottomLeft += translate;
	m_orthographicTopRight += translate;
}

void Camera::SetPositionAndOrientation(const Vec3& position, const EulerAngles& orientation) {
	m_position = position;
	m_orientation = orientation;
}
void Camera::SetPosition(const Vec3& position) {
	m_position = position;
}
Vec3 Camera::GetPosition()const {
	return m_position;
}
void Camera::SetOrientation(const EulerAngles& orientation) {
	m_orientation.m_yawDegrees = orientation.m_yawDegrees;
	m_orientation.m_pitchDegrees = orientation.m_pitchDegrees;
	m_orientation.m_rollDegrees = orientation.m_rollDegrees;
}
EulerAngles Camera::GetOrientation()const {
	return m_orientation;
}
Mat44 Camera::GetProjectionMatrix() const {
	if (m_mode == eMode_Orthographic) {
		return GetOrthographicMatrix();
	}
	else {
		return GetPerspectiveMatrix();
	}
}

Mat44 Camera::GetCameraToWorldTransform() const {
	Mat44 transform = Mat44::MakeTranslation3D(m_position);
	transform.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return transform;
}
Mat44 Camera::GetWorldToCameraTransform() const {
	return GetCameraToWorldTransform().GetOrthonormalInverse();
}
void Camera::SetCameraToRenderTransform(const Mat44& m) {
	m_cameraToRenderTransform = m;
}
Mat44 Camera::GetCameraToRenderTransform()const {
	return m_cameraToRenderTransform;
}
Mat44 Camera::GetRenderToClipTransform()const {
	return GetProjectionMatrix();
}

Mat44 Camera::GetOrthographicMatrix() const {
	return Mat44::MakeOrthoProjection(m_orthographicBottomLeft.x, m_orthographicTopRight.x, m_orthographicBottomLeft.y, m_orthographicTopRight.y, m_orthographicNear, m_orthographicFar);
}
Mat44 Camera::GetPerspectiveMatrix() const {
	return Mat44::MakePerspectiveProjection(m_perspectiveFOV, m_perspectiveAspect, m_perspectiveNear, m_perspectiveFar);
}
