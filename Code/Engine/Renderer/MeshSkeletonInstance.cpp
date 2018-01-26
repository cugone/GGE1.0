#include "Engine/Renderer/MeshSkeletonInstance.hpp"

#include "Engine/Math/Matrix4.hpp"

#include "Engine/Renderer/MeshMotion.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

MeshSkeletonInstance::MeshSkeletonInstance(SimpleRenderer* renderer)
    : skeleton(nullptr)
    , current_pose()
    , _renderer(renderer)
    , _skinTransforms(nullptr)
    , _currentMotion(nullptr)
    , _currentMotionTime(0.0f)
    , _localTransform()
{
    /* DO NOTHING */
}
MeshSkeletonInstance::~MeshSkeletonInstance() {
    delete _skinTransforms;
    _skinTransforms = nullptr;
}

Matrix4 MeshSkeletonInstance::get_joint_global_transform(std::size_t joint_idx) const {
    Matrix4 result = current_pose.local_transforms[joint_idx];
    for(MeshSkeleton::Joint* cur_joint_parent = get_joint_parent(joint_idx);
        cur_joint_parent != nullptr;
        cur_joint_parent = cur_joint_parent->parent) {
        result = current_pose.local_transforms[skeleton->get_joint_index(cur_joint_parent->name)] * result;
        auto t = result.GetTranslation();
    }
    return result;
}
Matrix4 MeshSkeletonInstance::get_joint_global_transform(const std::string& joint_name) const {
    Matrix4 result = current_pose.local_transforms[skeleton->get_joint_index(joint_name)];
    for(MeshSkeleton::Joint* cur_joint_parent = get_joint_parent(joint_name);
        cur_joint_parent != nullptr;
        cur_joint_parent = cur_joint_parent->parent) {
        result = current_pose.local_transforms[skeleton->get_joint_index(cur_joint_parent->name)] * result;
        auto t = result.GetTranslation();
    }
    return result;
}
MeshSkeleton::Joint* MeshSkeletonInstance::get_joint_parent(std::size_t joint_idx) const {
    return (skeleton->get_joint_parent(joint_idx));
}
MeshSkeleton::Joint* MeshSkeletonInstance::get_joint_parent(const std::string& joint_idx) const {
    return (skeleton->get_joint_parent(joint_idx));
}
std::size_t MeshSkeletonInstance::get_joint_count() const {
    return skeleton->get_joint_count();
}
void MeshSkeletonInstance::apply_motion(MeshMotion* motion, const float time) {
    if(motion) {
        _currentMotion = motion;
        _currentMotionTime = time;
    }
}
bool MeshSkeletonInstance::is_loaded() const {
    return skeleton != nullptr;
}

void MeshSkeletonInstance::SetLocalTransform(const Matrix4& transform) {
    _localTransform = transform;
}

void MeshSkeletonInstance::Update(float /*deltaSeconds*/) {
    if(_currentMotion && skeleton && _skinTransforms) {
        _currentMotion->evaluate(current_pose, _currentMotionTime);
        auto joint_count = get_joint_count();
        for(std::size_t i = 0; i < joint_count; ++i) {
            auto A = skeleton->get_joint_transform(i);
            auto C = get_joint_global_transform(i);
            auto A_inv = Matrix4::CalculateInverse(A);
            auto S = C * A_inv;
            _skinTransforms_data[i] = S;
        }
        _skinTransforms->Update(_renderer->_rhi_context, _skinTransforms_data.data());
    }
}

void MeshSkeletonInstance::Render() const {
    _renderer->SetModelMatrix(Matrix4::GetIdentity());
    _renderer->SetMaterial(_renderer->GetMaterial("__unlit"));
    _renderer->DisableDepthStencil();
    for(unsigned int i = 0; i < get_joint_count(); ++i) {
        auto p = get_joint_parent(i);
        if(p) {
            auto p1 = _localTransform * get_joint_global_transform(p->name);
            auto p2 = _localTransform * get_joint_global_transform(i);
            _renderer->DrawLine(Vertex3D(p1.GetTranslation(), Rgba::GREEN), Vertex3D(p2.GetTranslation(), Rgba::RED));
        }
    }
}

void MeshSkeletonInstance::InitializeSkinTransforms() {
    std::size_t size = get_joint_count();
    _skinTransforms_data.resize(size);
    for(auto& skinTransform : _skinTransforms_data) {
        skinTransform = Matrix4::GetIdentity();
    }
    _skinTransforms = _renderer->_rhi_device->CreateStructuredBuffer(_skinTransforms_data.data(), sizeof(Matrix4), static_cast<unsigned int>(size), BufferUsage::DYNAMIC, BufferBindUsage::SHADER_RESOURCE);
    _renderer->_rhi_context->SetStructuredBuffer(0, _skinTransforms);
}