#pragma once
#ifndef TOOLS_FBXLOAD
#define TOOLS_FBXLOAD

#include <string>

namespace fbxsdk {
    class FbxScene;
    class FbxNode;
    class FbxNodeAttribute;
    class FbxPose;
    class FbxSkeleton;
    class FbxMesh;
    class FbxAnimStack;
    class FbxTime;
    class FbxMatrix;
}

class MeshBuilder;
class MeshSkeleton;
class Pose;
class Animation;
class SkeletonInstance;
class MeshMotion;
class Matrix4;
class Vector3;

void FbxListFile(const char* filename);
void FbxListFile(const std::string& filepath);
bool FbxLoadMesh(MeshBuilder& mb, const std::string& filename, MeshSkeleton* skeleton = nullptr);
bool FbxLoadSkeleton(MeshSkeleton* skel, const std::string& filename);
bool FbxLoadMotion(MeshMotion* motion, const MeshSkeleton* skeleton, const std::string& filename, unsigned int framerate = 10);

#endif