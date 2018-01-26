#include "Engine/Renderer/Camera3D.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/TextureBase.hpp"

Vector3 Camera3D::GetForwardXYZ() const {
    float cos_yaw = MathUtils::CosDegrees(m_rotationYaw);
    float cos_pitch = MathUtils::CosDegrees(m_rotationPitch);

    float sin_yaw = MathUtils::SinDegrees(m_rotationYaw);
    float sin_pitch = MathUtils::SinDegrees(m_rotationPitch);

    return Vector3(-sin_yaw * cos_pitch, sin_pitch, cos_yaw * cos_pitch);
}

Vector3 Camera3D::GetForwardXY() const {
    return Vector3(MathUtils::CosDegrees(m_rotationYaw), MathUtils::SinDegrees(m_rotationYaw), 0.0f);
}

Vector3 Camera3D::GetForwardXZ() const {
    return Vector3(-MathUtils::SinDegrees(m_rotationYaw), 0.0f, MathUtils::CosDegrees(m_rotationYaw));
}

Vector3 Camera3D::GetLeftXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    Vector3 forward = GetForwardXYZ();
    Vector3 world_up = worldUp;
    return CrossProduct(forward, world_up);
}

Vector3 Camera3D::GetLeftXY() const {
    Vector3 forward = GetForwardXY();
    return Vector3(-forward.y, forward.x, 0.0f); //(-y, x) for left perpendicular
}

Vector3 Camera3D::GetRightXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    return -GetLeftXYZ(worldUp);
}
Vector3 Camera3D::GetRightXY() const {
    return -GetLeftXY();
}
Vector3 Camera3D::GetUpFromLeftXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    Vector3 forward = GetForwardXYZ();
    Vector3 left = GetLeftXYZ(worldUp);
    return CrossProduct(left, forward);
}

Vector3 Camera3D::GetUpFromLeftXY() const {
    Vector3 forward = GetForwardXY();
    Vector3 left = GetLeftXY();
    return CrossProduct(left, forward);
}

Vector3 Camera3D::GetUpFromRightXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    Vector3 forward = GetForwardXYZ();
    Vector3 right = GetRightXYZ(worldUp);
    return CrossProduct(right, forward);
}

Vector3 Camera3D::GetUpFromRightXY() const {
    Vector3 forward = GetForwardXY();
    Vector3 right = GetRightXY();
    return CrossProduct(right, forward);
}

Vector3 Camera3D::GetForwardQuaternionXYZ(const Vector3& worldForward /*= Vector3::Z_AXIS*/) const {
    return Vector3(m_rotation * worldForward);
}

Vector3 Camera3D::GetForwardQuaternionXY(const Vector3& worldForward /*= Vector3::Z_AXIS*/) const {
    return Vector3(m_rotation * worldForward);
}

Vector3 Camera3D::GetForwardQuaternionXZ(const Vector3& worldUp /*= Vector3::Z_AXIS*/) const {
    return Vector3(m_rotation * -worldUp);
}

Vector3 Camera3D::GetLeftQuaternionXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    Vector3 forward = GetForwardQuaternionXYZ();
    Vector3 world_up = worldUp;
    return CrossProduct(forward, world_up);
}

Vector3 Camera3D::GetLeftQuaternionXY() const {
    Vector3 forward = GetForwardQuaternionXYZ();
    return Vector3(-forward.y, forward.x, 0.0f); //(-y, x) for left perpendicular
}

Vector3 Camera3D::GetRightQuaternionXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    return -GetLeftQuaternionXYZ(worldUp);
}

Vector3 Camera3D::GetRightQuaternionXY() const {
    return -GetLeftQuaternionXY();
}

Vector3 Camera3D::GetUpFromLeftQuaternionXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    Vector3 forward = GetForwardQuaternionXYZ();
    Vector3 left = GetLeftQuaternionXYZ(worldUp);
    return CrossProduct(forward, left);
}

Vector3 Camera3D::GetUpFromLeftQuaternionXY() const {
    Vector3 forward = GetForwardQuaternionXY();
    Vector3 left = GetLeftQuaternionXY();
    return CrossProduct(left, forward);
}

Vector3 Camera3D::GetUpFromRightQuaternionXYZ(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {
    Vector3 forward = GetForwardQuaternionXYZ();
    Vector3 right = GetRightQuaternionXYZ(worldUp);
    return CrossProduct(forward, right);
}

Vector3 Camera3D::GetUpFromRightQuaternionXY() const {
    Vector3 forward = GetForwardQuaternionXY();
    Vector3 right = GetRightQuaternionXY();
    return CrossProduct(right, forward);
}

Matrix4 Camera3D::GetViewMatrix() const {
    return _view;
}

Matrix4 Camera3D::CalcViewMatrix(const Vector3& /*worldUp*/ /*= Vector3::Y_AXIS*/) const {
    Matrix4 I = Matrix4::GetIdentity();
    Matrix4 Rx = Matrix4::Create3DXRotationDegreesMatrix(m_rotationPitch);
    Matrix4 Ry = Matrix4::Create3DYRotationDegreesMatrix(m_rotationYaw);
    Matrix4 Rz = Matrix4::Create3DZRotationDegreesMatrix(m_rotationRoll);
    Matrix4 T = Matrix4::CreateTranslationMatrix(m_position);
    _view = I * Rz * Rx * Ry * T;
    return _view;
    //Matrix4 C_inv = Matrix4::CalculateInverse(C);
    //return C_inv;
    //Matrix4 result = I * Matrix4(m_rotation) * T;
    //return Matrix4(GetLeftXYZ(), GetUpFromLeftXYZ(worldUp), GetForwardXYZ(), m_position);
}

Matrix4 Camera3D::CalcViewMatrixQuaternion(const Vector3& /*worldUp*/ /*= Vector3::Y_AXIS*/) const {
    Matrix4 I = Matrix4::GetIdentity();
    Matrix4 R = Matrix4(Quaternion::CreateFromEulerAnglesDegrees(m_rotationYaw, m_rotationPitch, m_rotationRoll));
    Matrix4 T = Matrix4::CreateTranslationMatrix(m_position);
    _view = I * R * T;
    return _view;
    //Matrix4 C_inv = Matrix4::CalculateInverse(C);
    //return C_inv;
    //Matrix4 result = I * Matrix4(m_rotation) * T;
    //return Matrix4(GetLeftXYZ(), GetUpFromLeftXYZ(worldUp), GetForwardXYZ(), m_position);
}

Matrix4 Camera3D::GetProjectionMatrix() const {
    return _projection;
}

Camera3D::CameraFrustum Camera3D::CalcFrustum(const Vector3& worldUp /*= Vector3::Y_AXIS*/) const {

    //Referenced from http://www.lighthouse3d.com/tutorials/view-frustum-culling/view-frustums-shape/
    //and http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/

    float fovRadians = MathUtils::ConvertDegreesToRadians(m_fovVerticalDegrees);
    float fovHalf = fovRadians * 0.5f;
    float tanFovHalf = std::tan(fovHalf);

    float nearHeight = 2.0f * tanFovHalf * m_nearDistance;
    float nearWidth = nearHeight * m_aspectRatio;
    Vector2 nearHalfExtents(nearWidth * 0.5f, nearHeight * 0.5f);

    float farHeight = 2.0f * tanFovHalf * m_farDistance;
    float farWidth = farHeight * m_aspectRatio;
    Vector2 farHalfExtents(farWidth * 0.5f, farHeight * 0.5f);

    Vector3 forward = GetForwardXYZ();
    Vector3 backward = -forward;
    Vector3 up = GetUpFromLeftXYZ(worldUp);
    Vector3 down = -up;
    Vector3 left = GetLeftXYZ(worldUp);
    Vector3 right = -left;
    float cam_distance_from_origin = m_position.CalcLength();

    CameraFrustum frustum;

    std::size_t currentPlane = static_cast<std::size_t>(FrustumPlanes::NEARPLANE);
    frustum[currentPlane].dist = m_nearDistance;
    frustum[currentPlane].normal = forward;

    currentPlane = static_cast<std::size_t>(FrustumPlanes::FARPLANE);
    frustum[currentPlane].dist = m_farDistance;
    frustum[currentPlane].normal = backward;

    currentPlane = static_cast<std::size_t>(FrustumPlanes::RIGHT);
    frustum[currentPlane].dist = cam_distance_from_origin;
    Vector3 rightEdge(((m_position + forward * m_nearDistance) + right * nearHalfExtents.x) - m_position);
    rightEdge.Normalize();
    frustum[currentPlane].normal = CrossProduct(up, rightEdge);

    currentPlane = static_cast<std::size_t>(FrustumPlanes::LEFT);
    frustum[currentPlane].dist = cam_distance_from_origin;
    Vector3 leftEdge(((m_position + forward * m_nearDistance) + left * nearHalfExtents.x) - m_position);
    leftEdge.Normalize();
    frustum[currentPlane].normal = CrossProduct(down, leftEdge);

    currentPlane = static_cast<std::size_t>(FrustumPlanes::TOP);
    frustum[currentPlane].dist = cam_distance_from_origin;
    Vector3 topEdge(((m_position + forward * m_nearDistance) + up * nearHalfExtents.y) - m_position);
    topEdge.Normalize();
    frustum[currentPlane].normal = CrossProduct(down, topEdge);

    currentPlane = static_cast<std::size_t>(FrustumPlanes::BOTTOM);
    frustum[currentPlane].dist = cam_distance_from_origin;
    Vector3 bottomEdge(((m_position + forward * m_nearDistance) + down * nearHalfExtents.y) - m_position);
    bottomEdge.Normalize();
    frustum[currentPlane].normal = CrossProduct(up, bottomEdge);

    return std::move(frustum);
}

void Camera3D::SetRenderTarget(TextureBase* target, TextureBase* depthstencil /* = nullptr*/) {
    ASSERT_OR_DIE(target && target->IsValid() && target->IsRenderTarget(), "Camera3D::SetRenderTarget: target not valid nor a render target.");
    _renderTarget = target;
    _renderDepthStencil = depthstencil;
}

TextureBase* Camera3D::GetRenderTarget() const {
    return _renderTarget;
}

Vector3 Camera3D::ScreenToWorldCoordinates(SimpleRenderer* renderer, const Vector2& screen_coords) {

    //You need the inverse of the camera_to_screen matrix.
    //The 3D pipeline overall looks like this:
    //  model->world->camera->viewport->screen
    //The world_to_camera matrix is the usual rotation / translation matrix you construct for a 3D camera system.Inverting it is trivial : https://stackoverflow.com/questions/695043/how-does-one-convert-world-coordinates-to-camera-coordinates
    //The camera->viewport matrix is your projection matrix; inverting that is trivial if you know how to construct it in the first place : http://www.gamedev.net/topic/478055-perspective-projection-matrix/
    //The viewport_to_screen matrix is generally a simple transformation that maps the coordinate space[-1, -1] : [1, 1] into [0, 0]:[ScreenWidth, ScreenHeight].
    //  You don't usually need to generate this matrix yourself as it's taken care of by the graphics hardware automatically.
    //  The order of operations of the matrix is to scale by half the screen size, then to translate by half the screen size.
    //  Inverting those operations is basic algebra.
    //Multiply those inverted matrices together in the opposite order to get your screen->world matrix.
    //  You can now multiply by the vector(ScreenX, ScreenY, Near, 1) to get the world position of the mouse cursor on the near clipping plane in world space.
    //If you need to do object picking at some point, you can take that point and the camera's position to generate a ray to do ray-collision test to find the object the cursor is pointing at.
    
    auto screen_extents = Vector2(renderer->_rhi_output->GetDimensions());
    auto screen_half_extents = screen_extents * 0.50f;
    auto screen_ndc_x = MathUtils::RangeMap(screen_coords.x, 0.0f, screen_extents.x, -1.0f, 1.0f);
    auto screen_ndc_y = MathUtils::RangeMap(screen_coords.y, 0.0f, screen_extents.y, 1.0f, -1.0f);
    Vector4 ndc = Vector4(screen_ndc_x, screen_ndc_y, 0.0f, 1.0f);
    //auto inv_S = Matrix4::CreateScaleMatrix(Vector2(1.0f / screen_half_extents.x, 1.0f / screen_half_extents.y));
    //auto inv_T = Matrix4::CreateTranslationMatrix(-screen_half_extents);
    //auto inv_ndc = inv_S * inv_T;

    auto inv_projection = Matrix4::CalculateInverse(renderer->_matrix_data.projection);
    auto inv_view = Matrix4::CalculateInverse(renderer->_matrix_data.view);
    
    auto projection = renderer->_matrix_data.projection;
    auto view = renderer->_matrix_data.view;

    auto screen_coords_vec = Vector4(Vector3(screen_coords, 0.0), 1.0f);
    auto non_homogeneous_screen_coords = inv_view * inv_projection * ndc;// *screen_coords_vec;
    auto homogeneous_screen_coords = Vector4::CalcHomogeneous(non_homogeneous_screen_coords);

    return Vector3(homogeneous_screen_coords);
}

Vector3 Camera3D::NdcCoordinatesFromScreenCoords(SimpleRenderer* renderer, const Vector2& screen_coords) {

    //You need the inverse of the camera_to_screen matrix.
    //The 3D pipeline overall looks like this:
    //  model->world->camera->viewport->screen
    //The world_to_camera matrix is the usual rotation / translation matrix you construct for a 3D camera system.Inverting it is trivial : https://stackoverflow.com/questions/695043/how-does-one-convert-world-coordinates-to-camera-coordinates
    //The camera->viewport matrix is your projection matrix; inverting that is trivial if you know how to construct it in the first place : http://www.gamedev.net/topic/478055-perspective-projection-matrix/
    //The viewport_to_screen matrix is generally a simple transformation that maps the coordinate space[-1, -1] : [1, 1] into [0, 0]:[ScreenWidth, ScreenHeight].
    //  You don't usually need to generate this matrix yourself as it's taken care of by the graphics hardware automatically.
    //  The order of operations of the matrix is to scale by half the screen size, then to translate by half the screen size.
    //  Inverting those operations is basic algebra.
    //Multiply those inverted matrices together in the opposite order to get your screen->world matrix.
    //  You can now multiply by the vector(ScreenX, ScreenY, Near, 1) to get the world position of the mouse cursor on the near clipping plane in world space.
    //If you need to do object picking at some point, you can take that point and the camera's position to generate a ray to do ray-collision test to find the object the cursor is pointing at.

    auto screen_extents = Vector2(renderer->_rhi_output->GetDimensions());
    auto screen_half_extents = screen_extents * 0.50f;
    auto screen_ndc_x = MathUtils::RangeMap(screen_coords.x, 0.0f, screen_extents.x, -1.0f, 1.0f);
    auto screen_ndc_y = MathUtils::RangeMap(screen_coords.y, 0.0f, screen_extents.y, 1.0f, -1.0f);
    Vector4 ndc = Vector4(screen_ndc_x, screen_ndc_y, 0.0f, 1.0f);

    auto non_homogeneous_screen_coords = ndc;// *screen_coords_vec;
    auto homogeneous_screen_coords = Vector4::CalcHomogeneous(non_homogeneous_screen_coords);

    return Vector3(homogeneous_screen_coords);
}

Vector2 Camera3D::WorldToScreenCoordinates(SimpleRenderer* renderer, const Vector3& world_coords) {

    auto projection = renderer->_matrix_data.projection;
    auto view = renderer->_matrix_data.view;
    auto non_homogeneous_point = view * projection * Vector4(world_coords, 1.0f);
    //auto non_homogeneous_point = inv_projection * inv_view * inv_model * Vector4(Vector3(screen_coords, m_nearDistance), 1.0f);
    auto homogeneous_point = Vector4::CalcHomogeneous(non_homogeneous_point);
    return Vector3(homogeneous_point);// *Vector3(Vector2(renderer->_rhi_output->GetDimensions()), 1.0f);
}

void Camera3D::SetProjectionMatrix(Matrix4&& mat) {
    _projection = std::move(mat);
}

void Camera3D::SetProjectionMatrix(const Matrix4& mat) {
    _projection = mat;
}

void Camera3D::SetupView(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance, const Vector3& worldUp /*= Vector3::Y_AXIS*/) {
    m_fovVerticalDegrees = fovVerticalDegrees;
    m_aspectRatio = aspectRatio;
    m_nearDistance = nearDistance;
    m_farDistance = farDistance;
    _worldUp = worldUp;
}