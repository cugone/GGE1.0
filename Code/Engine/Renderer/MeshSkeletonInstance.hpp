#pragma once

#include "Engine/Renderer/MeshSkeleton.hpp"
#include "Engine/Renderer/MeshPose.hpp"

class RHIDevice;
class RHIDeviceContext;
class MeshMotion;
class SimpleRenderer;
class Matrix4;
class StructuredBuffer;

class MeshSkeletonInstance {
public:

    MeshSkeletonInstance(SimpleRenderer* renderer);
    ~MeshSkeletonInstance();

    MeshSkeleton* skeleton;
    MeshPose current_pose;

    void InitializeSkinTransforms();

    Matrix4 get_joint_global_transform(std::size_t joint_idx) const;
    Matrix4 get_joint_global_transform(const std::string& joint_name) const;
    MeshSkeleton::Joint* get_joint_parent(std::size_t joint_idx) const;
    MeshSkeleton::Joint* get_joint_parent(const std::string& joint_idx) const;
    std::size_t get_joint_count() const;

    void apply_motion(MeshMotion* motion, const float time);
    bool is_loaded() const;

    void SetLocalTransform(const Matrix4& transform);
    void Update(float deltaSeconds);
    void Render() const;

protected:
private:
    SimpleRenderer* _renderer;
    StructuredBuffer* _skinTransforms;
    MeshMotion* _currentMotion;
    float _currentMotionTime;
    Matrix4 _localTransform;
    std::vector<Matrix4> _skinTransforms_data;
};