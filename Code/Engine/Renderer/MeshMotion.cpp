#include "Engine/Renderer/MeshMotion.hpp"

#include <algorithm>
#include <cmath>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/MeshSkeleton.hpp"

#include "Engine/Core/FileUtils.hpp"

void MeshMotion::set_name(const std::string& newName) {
    name = newName;
}
float MeshMotion::set_duration(float time) {
    float duration = std::ceil(framerate * time) + 1;
    poses.resize((unsigned int)duration);
    return duration;
}

void MeshMotion::set_framerate(float newFramerate) {
    framerate = newFramerate;
}
float MeshMotion::get_duration() const {
    return (poses.size() - 1) / framerate;
}

std::size_t MeshMotion::get_frame_count() const {
    return poses.size();
}

const MeshPose& MeshMotion::get_pose(std::size_t frame_idx) const {
    if(frame_idx >= poses.size()) {
        ERROR_AND_DIE("MeshMotion::get_pose: frame index out of bounds.")
    }
    return poses[frame_idx];
}

MeshPose& MeshMotion::get_pose(std::size_t frame_idx) {
    return const_cast<MeshPose&>(static_cast<const MeshMotion&>(*this).get_pose(frame_idx));
}

void MeshMotion::evaluate(MeshPose& out, float time) const {
    float duration = get_duration();
    float timeInAnimation = std::fmod(time, duration);
    float calculatedFrame = timeInAnimation * framerate;

    float firstKeyFrame_idx = std::floor(calculatedFrame);
    float secondKeyFrame_idx = std::ceil(calculatedFrame);

    MeshPose firstKeyFrame = poses[(unsigned int)firstKeyFrame_idx];
    MeshPose secondKeyFrame = poses[(unsigned int)secondKeyFrame_idx];
    //out = firstKeyFrame;
    //return;

    MeshPose result;
    std::size_t s = (std::min)(firstKeyFrame.local_transforms.size(), secondKeyFrame.local_transforms.size());
    for(unsigned int pose_idx = 0; pose_idx < s; ++pose_idx) {
        const auto& first_transform = firstKeyFrame.local_transforms[pose_idx];
        const auto& second_transform = secondKeyFrame.local_transforms[pose_idx];

        Vector3 startTranslation = first_transform.GetTranslation();
        Vector3 endTranslation = second_transform.GetTranslation();

        //Quaternion startRot = Quaternion(first_transform);
        //Quaternion endRot = Quaternion(second_transform);

        Matrix4 startRot = Matrix4(first_transform.GetIBasis(), first_transform.GetJBasis(), first_transform.GetKBasis(), Vector4::W_AXIS);
        Matrix4 endRot = Matrix4(second_transform.GetIBasis(), second_transform.GetJBasis(), second_transform.GetKBasis(), Vector4::W_AXIS);

        Vector3 startScale = first_transform.GetScale();
        Vector3 endScale = second_transform.GetScale();

        float t = calculatedFrame - firstKeyFrame_idx;
        Matrix4 T = Matrix4::CreateTranslationMatrix(Interpolate(startTranslation, endTranslation, t));
        Matrix4 Q = Matrix4(MathUtils::Interpolate(startRot, endRot, t));
        Matrix4 S = Matrix4::CreateScaleMatrix(Interpolate(startScale, endScale, t));
        auto result_mat = T * Q * S;
        result.local_transforms.push_back(result_mat);
    }
    out = result;
}

bool MeshMotion::write(FileUtils::BinaryStream& stream) const {
    stream.write(name);
    stream.write(framerate);
    std::size_t p_s = poses.size();
    stream.write(p_s);
    for(std::size_t i = 0; i < p_s; ++i) {
        if(!poses[i].write(stream)) {
            return false;
        }
    }
    return true;
}
bool MeshMotion::read(FileUtils::BinaryStream& stream) {
    stream.read(name);
    stream.read(framerate);
    unsigned int p_s = 0;
    stream.read(p_s);
    poses.resize(p_s);
    for(unsigned int i = 0; i < p_s; ++i) {
        if(!poses[i].read(stream)) {
            return false;
        }
    }
    return true;
}