#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB2.hpp"
class Camera {
public:
    enum Mode {
        eMode_Orthographic,
        eMode_Perspective,
        eMode_Count
    };
    void SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight, float zNear=0.f, float zFar=1.f);
    void SetPerspectiveView(float aspect, float fov, float zNear, float zFar);

    void SetPositionAndOrientation(const Vec3& position, const EulerAngles& orientation);
    void SetPosition(const Vec3& position);
    Vec3 GetPosition()const;
    void SetOrientation(const EulerAngles& orientation);
    EulerAngles GetOrientation()const;

    Mat44 GetCameraToWorldTransform() const;
    Mat44 GetWorldToCameraTransform() const;

    void SetCameraToRenderTransform(const Mat44& m);
    Mat44 GetCameraToRenderTransform()const;

    Mat44 GetRenderToClipTransform()const;

    Vec2 GetOrthoBottomLeft() const;
    Vec2 GetOrthoTopRight() const;
    Vec2 GetDimensions()const;
    void Translate2D(Vec2 const& translate);

    Mat44 GetOrthographicMatrix() const;
    Mat44 GetPerspectiveMatrix() const;
    Mat44 GetProjectionMatrix() const;
    //Camera viewPort, bounded to 0 to 1.
    AABB2 m_viewPort = AABB2(Vec2(0.f, 0.f),Vec2(1.f, 1.f));
protected:
    Mode m_mode = eMode_Orthographic;

    Vec3 m_position;
    EulerAngles m_orientation;

    Vec2 m_orthographicBottomLeft;
    Vec2 m_orthographicTopRight;
    float m_orthographicNear;
    float m_orthographicFar;

    float m_perspectiveAspect;
    float m_perspectiveFOV;
    float m_perspectiveNear;
    float m_perspectiveFar;

    Mat44 m_cameraToRenderTransform;
};