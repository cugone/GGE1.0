#pragma once

#include <vector>

#include "Engine/Core/FileUtils.hpp"

#include "Engine/Math/Matrix4.hpp"

class MeshPose {
public:
    std::vector<Matrix4> local_transforms;

    bool write(FileUtils::BinaryStream& stream) const;
    bool read(FileUtils::BinaryStream& stream);
};

bool operator==(const MeshPose& lhs, const MeshPose& rhs);
bool operator!=(const MeshPose& lhs, const MeshPose& rhs);