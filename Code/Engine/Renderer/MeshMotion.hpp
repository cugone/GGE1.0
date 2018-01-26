#pragma once

#include <string>
#include <vector>

#include "Engine/Core/FileUtils.hpp"

#include "Engine/Renderer/MeshPose.hpp"

class MeshSkeleton;

class MeshMotion {
public:

    std::string name;

    float framerate;
    std::vector<MeshPose> poses;

    void set_name(const std::string& newName);

    // Make sure poses is exactly long enough to encompass this time.
    float set_duration(float time);

    void set_framerate(float newFramerate);

    // framerate is 10hz, that means the time between two frames takes 1/10th of a second.
    // therefore if we have 3 frames in this motion, the total duration would be (3 - 1) * 0.1,
    // or 0.2 seconds long.  
    float get_duration() const;
    std::size_t get_frame_count() const;

    const MeshPose& get_pose(std::size_t frame_idx) const;
    MeshPose& get_pose(std::size_t frame_idx);

    void evaluate(MeshPose& out, float time) const;

    bool write(FileUtils::BinaryStream& stream) const;
    bool read(FileUtils::BinaryStream& stream);
};
