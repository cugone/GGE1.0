#pragma once

#include "Engine/Math/Matrix4.hpp"

class Material;
class Mesh;
class MeshBuilder;
class MeshSkeleton;
class MeshMotion;
class MeshSkeletonInstance;
class SimpleRenderer;

class Model {
public:

    enum RenderOptions {
        NONE = 0,
        MESH = 1 << 0,
        SKELETON = 1 << 1,
        BOTH = MESH | SKELETON,
    };

    enum class Type {
        OBJ,
        FBX,
    };

    Model(const Type& type, SimpleRenderer* renderer, const std::string& model_path, const Matrix4& initialTransform = Matrix4::GetIdentity());
    Model(SimpleRenderer* renderer, Mesh* mesh, MeshSkeleton* skeleton = nullptr, MeshMotion* motion = nullptr, const Matrix4& initialTransform = Matrix4::GetIdentity());
	~Model();

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;

    void SetSkeleton(MeshSkeleton* skeleton);
    MeshSkeleton* GetSkeleton() const;

    void SetMotion(MeshMotion* motion);
    MeshMotion* GetMotion() const;

    void ApplyMotion(float time);
    void ApplyMotion(MeshMotion* motion, float time);

    void Update(float deltaSeconds, float time);
    void Render(const Model::RenderOptions& opts = Model::RenderOptions::MESH) const;

    void SetMaterial(Material* material);
    Material* GetMaterial() const;

    void ToggleShowSkeleton();

    void SetLocalTransform(const Matrix4& transform);

    void ShowSkeleton(bool value = true);

protected:
private:
    SimpleRenderer* _renderer;
    Mesh* _mesh;
    MeshSkeleton* _skeleton;
    MeshMotion* _motion;
    MeshBuilder* _meshbuilder;
    MeshSkeletonInstance* _skeletonInstance;
    bool _showSkeleton;
};