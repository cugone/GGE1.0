#pragma once

// Interface a skeleton I'll be using - up to you to figure out 
// how you want to store this.  
// 
// End of the day, this is a collection of global transforms in a heirachy 
// that can be referenced by name.

#include <string>

namespace fbxsdk {
class FbxScene;
class FbxNode;
class FbxNodeAttribute;
class FbxPose;
class FbxSkeleton;
}

class SimpleRenderer;

#include <vector>

#include "Engine/Core/FileUtils.hpp"

#include "Engine/Math/Matrix4.hpp"

class MeshSkeleton {
public:
    struct Joint {
        Matrix4 transform;
        Joint* parent;
        std::string name;
        Joint()
            : transform(Matrix4::GetIdentity())
            , parent(nullptr)
            , name("")
        {
            /* DO NOTHING */
        }
    };
    MeshSkeleton();
    // Reset the skeleton - clear out all bones that make up
    // this skeleton
    void clear();

    // Adds a joint.  Can be parented to another 
    // joint within this skeleton.
    void add_joint(const std::string& name,
                   const std::string& parent_name,
                   const Matrix4& transform);

    // get number of joints/bones in this skeleton.
    unsigned int get_joint_count() const;

    // Get a joint index by name, returns
    // (unsigned int)(-1) if it doesn't exist.
    unsigned int get_joint_index(const std::string& name) const;
    unsigned int get_joint_index(const std::string& name);

    // Get the global transform for a joint.
    Matrix4 get_joint_transform(unsigned int joint_idx) const;
    Matrix4 get_joint_transform(unsigned int joint_idx);
    
    Matrix4 get_joint_transform(const std::string& name) const;
    Matrix4 get_joint_transform(const std::string& name);

    void set_joint_transform(unsigned int joint_idx, const Matrix4& transform);
    void set_joint_transform(const std::string& name, const Matrix4& transoform);

    Joint* get_joint_parent(unsigned int joint_idx) const;
    Joint* get_joint_parent(const std::string& name) const;

    std::string get_joint_name(unsigned int joint_idx) const;

    bool is_loaded() const;

    void Render(SimpleRenderer& renderer) const;
    void SetLocalTransform(const Matrix4& transform);

    bool write(FileUtils::BinaryStream& stream) const;
    bool read(FileUtils::BinaryStream& stream);
public:
    // what data would you need to do this?
    std::vector<Joint*> _joint_transforms;
    Matrix4 _localTransform;
};
