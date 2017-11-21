#pragma once

#include <array>

#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"

class TextureBase;
class SimpleRenderer;

class Camera3D {
public:


    enum class FrustumPlanes {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,
        NEARPLANE,
        FARPLANE,
        MAX_COUNT,
    };

    using CameraFrustum = std::array<Plane3, static_cast<std::size_t>(FrustumPlanes::MAX_COUNT)>;

    Vector3 m_position = Vector3::ZERO;
    Quaternion m_rotation = Quaternion::GetIdentity();

    float m_rotationYaw = 0.0f;
    float m_rotationPitch = 0.0f;
    float m_rotationRoll = 0.0f;

    float m_fovVerticalDegrees = 60.0f;
    float m_aspectRatio = 16.0f / 9.0f;
    float m_nearDistance = 0.01f;
    float m_farDistance = 1000.0f;

    Vector3 GetForwardXYZ() const;
    Vector3 GetForwardXY() const;
    Vector3 GetForwardXZ() const;
    Vector3 GetLeftXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetLeftXY() const;
    Vector3 GetRightXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetRightXY() const;

    Vector3 GetUpFromLeftXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetUpFromLeftXY() const;
    Vector3 GetUpFromRightXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetUpFromRightXY() const;

    Vector3 GetForwardQuaternionXYZ(const Vector3& worldForward = Vector3::Z_AXIS) const;
    Vector3 GetForwardQuaternionXY(const Vector3& worldForward = Vector3::Z_AXIS) const;
    Vector3 GetForwardQuaternionXZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetLeftQuaternionXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetLeftQuaternionXY() const;
    Vector3 GetRightQuaternionXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetRightQuaternionXY() const;

    Vector3 GetUpFromLeftQuaternionXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetUpFromLeftQuaternionXY() const;
    Vector3 GetUpFromRightQuaternionXYZ(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Vector3 GetUpFromRightQuaternionXY() const;

    Matrix4 GetViewMatrix() const;
    Matrix4 CalcViewMatrix(const Vector3& worldUp = Vector3::Y_AXIS) const;
    Matrix4 CalcViewMatrixQuaternion(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const;

    Matrix4 GetProjectionMatrix() const;
    void SetProjectionMatrix(Matrix4&& mat);
    void SetProjectionMatrix(const Matrix4& mat);

    void SetupView(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance, const Vector3& worldUp = Vector3::Y_AXIS);
    CameraFrustum CalcFrustum(const Vector3& worldUp = Vector3::Y_AXIS) const;

    void SetRenderTarget(TextureBase* target);
    TextureBase* GetRenderTarget() const;

    Vector3 ScreenToWorldCoordinates(SimpleRenderer* renderer, const Vector2& screen_coords);
    Vector2 WorldToScreenCoordinates(SimpleRenderer* renderer, const Vector3& world_coords);


protected:
private:
    Vector3 _worldUp = Vector3::Y_AXIS;
    mutable Matrix4 _projection = Matrix4::GetIdentity();
    mutable Matrix4 _view = Matrix4::GetIdentity();
    TextureBase* _renderTarget = nullptr;
};