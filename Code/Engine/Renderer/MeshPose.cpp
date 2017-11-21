#include "Engine/Renderer/MeshPose.hpp"

#include "Engine/Core/FileUtils.hpp"

bool operator==(const MeshPose& lhs, const MeshPose& rhs) {
    return lhs.local_transforms == rhs.local_transforms;
}

bool operator!=(const MeshPose& lhs, const MeshPose& rhs) {
    return !(lhs == rhs);
}
bool MeshPose::write(FileUtils::BinaryStream& stream) const {
    std::size_t t_s = this->local_transforms.size();
    stream.write(t_s);
    for(std::size_t i = 0; i < t_s; ++i) {
        if(!stream.write(this->local_transforms[i])) {
            return false;
        }
    }
    return true;
}
bool MeshPose::read(FileUtils::BinaryStream& stream) {
    std::size_t t_s = 0;
    stream.read(t_s);
    this->local_transforms.resize(t_s);
    for(std::size_t i = 0; i < t_s; ++i) {
        if(!stream.read(this->local_transforms[i])) {
            return false;
        }
    }
    return true;
}