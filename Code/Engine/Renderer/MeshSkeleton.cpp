#include "Engine/Renderer/MeshSkeleton.hpp"

#include <algorithm>
#include <iterator>

#include "Thirdparty/FBX/fbx.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"

MeshSkeleton::MeshSkeleton()
    : _joint_transforms{}
    , _localTransform(Matrix4::GetIdentity())
{
    /* DO NOTHING */
}

void MeshSkeleton::clear() {
    for(auto& j : _joint_transforms) {
        delete j;
        j = nullptr;
    }
    _joint_transforms.clear();
}

void MeshSkeleton::add_joint(const std::string& name, const std::string& parent_name, const Matrix4& transform) {
    Joint* new_joint = new Joint;
    new_joint->name = name;
    auto found_iter = std::find_if(std::begin(_joint_transforms), std::end(_joint_transforms), [&](const Joint* j) { return j->name == name; });
    if(found_iter != _joint_transforms.end()) {
        return;
    }
    if(!_joint_transforms.empty()) {
        auto p_index = get_joint_index(parent_name);
        if(p_index < _joint_transforms.size()) {
            Joint* parent = _joint_transforms[p_index];
            new_joint->parent = parent;
        }
    }
    new_joint->transform = transform;
    this->_joint_transforms.push_back(new_joint);
}

std::size_t MeshSkeleton::get_joint_count() const {
    return _joint_transforms.size();
}

std::size_t MeshSkeleton::get_joint_index(const std::string& name) const {
    std::size_t index = 0;
    auto found_iter = std::find_if(std::begin(_joint_transforms), std::end(_joint_transforms), [&](const Joint* j) { return j->name == name; });
    index = std::distance(_joint_transforms.begin(), found_iter);
    if(found_iter == _joint_transforms.end()) {
        return _joint_transforms.size();
    }
    return index;
}

std::size_t MeshSkeleton::get_joint_index(const std::string& name) {
    return static_cast<const MeshSkeleton&>(*this).get_joint_index(name);
}

Matrix4 MeshSkeleton::get_joint_transform(std::size_t joint_idx) const {
    if(joint_idx >= _joint_transforms.size()) {
        return Matrix4::GetIdentity();
    }
    auto idx = std::distance(_joint_transforms.begin(), _joint_transforms.begin() + joint_idx);
    return _joint_transforms[idx]->transform;
}

Matrix4 MeshSkeleton::get_joint_transform(std::size_t joint_idx) {
    return static_cast<const MeshSkeleton&>(*this).get_joint_transform(joint_idx);
}

Matrix4 MeshSkeleton::get_joint_transform(const std::string& name) const {
    auto found_iter = std::find_if(std::begin(_joint_transforms), std::end(_joint_transforms), [&](const Joint* j) { return j->name == name; });
    if(found_iter == _joint_transforms.end()) {
        return Matrix4::GetIdentity();
    }
    return (*found_iter)->transform;
}

Matrix4 MeshSkeleton::get_joint_transform(const std::string& name) {
    return static_cast<const MeshSkeleton&>(*this).get_joint_transform(name);
}

void MeshSkeleton::set_joint_transform(std::size_t joint_idx, const Matrix4& transform) {
    if(joint_idx >= _joint_transforms.size()) {
        return;
    }
    _joint_transforms[joint_idx]->transform = transform;
}

void MeshSkeleton::set_joint_transform(const std::string& name, const Matrix4& transform) {
    std::size_t i = get_joint_index(name);
    if(i >= _joint_transforms.size()) {
        return;
    }
    _joint_transforms[i]->transform = transform;
}

MeshSkeleton::Joint* MeshSkeleton::get_joint_parent(std::size_t joint_idx) const {
    if(joint_idx >= _joint_transforms.size()) {
        return nullptr;
    }
    return _joint_transforms[joint_idx]->parent;
}

MeshSkeleton::Joint* MeshSkeleton::get_joint_parent(const std::string& name) const {
    auto i = get_joint_index(name);
    if(i >= _joint_transforms.size()) {
        return nullptr;
    }
    return _joint_transforms[i]->parent;
}

std::string MeshSkeleton::get_joint_name(std::size_t joint_idx) const {
    if(joint_idx >= _joint_transforms.size()) {
        return "";
    }
    if(_joint_transforms[joint_idx]) {
        return _joint_transforms[joint_idx]->name;
    } else {
        return "";
    }
}
bool MeshSkeleton::is_loaded() const {
    return get_joint_count() > 0;
}

void MeshSkeleton::Render(SimpleRenderer& renderer) const {
    renderer.SetModelMatrix(Matrix4::GetIdentity());
    renderer.SetMaterial(renderer.GetMaterial("__unlit"));
    for(std::size_t i = 0; i < get_joint_count(); ++i) {
        auto p = get_joint_parent(i);
        if(p) {
            auto p1 = _localTransform * get_joint_transform(p->name);
            auto p2 = _localTransform * get_joint_transform(i);
            renderer.DrawLine(Vertex3D(Vector3(p1.GetTBasis()), Rgba::GREEN), Vertex3D(Vector3(p2.GetTBasis()), Rgba::RED));
        }
    }
}

void MeshSkeleton::SetLocalTransform(const Matrix4& transform) {
    _localTransform = transform;
}
bool MeshSkeleton::write(FileUtils::BinaryStream& stream) const {
    if(!stream.write(this->_joint_transforms.size())) {
        return false;
    }
    for(std::size_t i = 0; i < this->_joint_transforms.size(); ++i) {
        if(!stream.write(this->_joint_transforms[i]->name)) {
            return false;
        }
        if(!stream.write(this->_joint_transforms[i]->transform)) {
            return false;
        }
        bool has_parent = this->_joint_transforms[i]->parent != nullptr;
        if(!stream.write(has_parent)) {
            return false;
        }
        if(has_parent) {
            if(!stream.write(this->_joint_transforms[i]->parent->name)) {
                return false;
            }
        }
    }
    return true;
}
bool MeshSkeleton::read(FileUtils::BinaryStream& stream) {
    unsigned int jt_s = 0;
    if(!stream.read(jt_s)) {
        return false;
    }
    this->_joint_transforms.resize(jt_s);
    for(auto& jt : this->_joint_transforms) {
        jt = nullptr;
    }
    bool read_fail = false;
    std::map<std::string, std::string> joint_to_parent;
    for(std::size_t i = 0; i < this->_joint_transforms.size(); ++i) {
        this->_joint_transforms[i] = new Joint;
        if(!stream.read(this->_joint_transforms[i]->name)) {
            read_fail = true;
            break;
        }
        if(!stream.read(this->_joint_transforms[i]->transform)) {
            read_fail = true;
            break;
        }
        bool has_parent = false;
        if(!stream.read(has_parent)) {
            read_fail = true;
            break;
        }
        if(has_parent) {
            std::string parent_name;
            if(!stream.read(parent_name)) {
                read_fail = true;
                break;
            }
            joint_to_parent.insert(std::make_pair(this->_joint_transforms[i]->name, parent_name));
        }
    }
    if(!read_fail) {
        for(auto& jt : this->_joint_transforms) {
            std::string& my_name = jt->name;
            auto iter_find = joint_to_parent.find(my_name);
            if(iter_find == joint_to_parent.end()) {
                continue;
            }
            std::string& my_parent_name = iter_find->second;
            for(auto& jt_p : this->_joint_transforms) {
                if(jt_p->name == my_parent_name) {
                    jt->parent = jt_p;
                    break;
                }
            }
        }
    }
    if(read_fail) {
        for(auto& jt : this->_joint_transforms) {
            delete jt;
            jt = nullptr;
        }
        return false;
    }
    return true;
}