#include "Engine/Renderer/Model.hpp"

#include <sstream>

#include "Engine/Renderer/Material.hpp"

#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshSkeleton.hpp"
#include "Engine/Renderer/MeshMotion.hpp"
#include "Engine/Renderer/MeshSkeletonInstance.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

Model::Model(const Type& type, SimpleRenderer* renderer, const std::string& model_path, const Matrix4& initialTransform /* = Matrix4::GetIdentity() */)
    : _renderer(renderer)
    , _mesh(nullptr)
    , _skeleton(nullptr)
    , _motion(nullptr)
    , _meshbuilder(new MeshBuilder)
    , _skeletonInstance(new MeshSkeletonInstance(renderer))
    , _showSkeleton(false)
{
    _skeleton = _renderer->CreateOrGetSkeleton(model_path, initialTransform);
    _mesh = _renderer->CreateOrGetMesh(type, model_path, initialTransform, _skeleton);
    _motion = _renderer->CreateOrGetMotion(model_path, *_skeleton, *_skeletonInstance, initialTransform);
    _mesh->SetMaterial(_renderer->GetMaterial("__unlit"));
}

Model::Model(SimpleRenderer* renderer, Mesh* mesh, MeshSkeleton* skeleton /*= nullptr*/, MeshMotion* motion /*= nullptr*/, const Matrix4& initialTransform /* = Matrix4::GetIdentity() */)
    : _renderer(renderer)
    , _mesh(mesh)
    , _skeleton(skeleton)
    , _motion(motion)
    , _meshbuilder(new MeshBuilder)
    , _skeletonInstance(new MeshSkeletonInstance(renderer))
    , _showSkeleton(false)
{
    _mesh->SetLocalTransform(initialTransform);
    _mesh->SetMaterial(_renderer->GetMaterial("__unlit"));
}

Model::~Model() {

    _showSkeleton = false;

    _renderer = nullptr;

    _mesh = nullptr;
    _skeleton = nullptr;
    _motion = nullptr;

    delete _skeletonInstance;
    _skeletonInstance = nullptr;

    delete _meshbuilder;
    _meshbuilder = nullptr;

}

void Model::SetMesh(Mesh* mesh) {
    _mesh = mesh;
}

Mesh* Model::GetMesh() const {
    return _mesh;
}

void Model::SetSkeleton(MeshSkeleton* skeleton) {
    _skeleton = skeleton;
}

MeshSkeleton* Model::GetSkeleton() const {
    return _skeleton;
}

void Model::SetMotion(MeshMotion* motion) {
    _motion = motion;
}

MeshMotion* Model::GetMotion() const {
    return _motion;
}

void Model::ApplyMotion(float time) {
    if(_skeletonInstance) {
        _skeletonInstance->apply_motion(_motion, time);
    }
}

void Model::ApplyMotion(MeshMotion* motion, float time) {
    SetMotion(motion);
    ApplyMotion(time);
}

void Model::Update(float deltaSeconds, float time) {
    ApplyMotion(time);
    if(_skeletonInstance) {
        _skeletonInstance->Update(deltaSeconds);
    }
}

void Model::Render(const Model::RenderOptions& opts /*= Model::RenderOptions::MESH*/) const {
    if(opts == Model::RenderOptions::NONE) {
        return;
    }

    if((opts & Model::RenderOptions::MESH) == Model::RenderOptions::MESH) {
        _mesh->Render(*_renderer);
    }

    if(_showSkeleton || ((opts & Model::RenderOptions::SKELETON) == Model::RenderOptions::SKELETON)) {
        if(_skeletonInstance && _skeletonInstance->is_loaded()) {
            _skeletonInstance->Render();
        }
    }

}

void Model::SetMaterial(Material* material) {
    _mesh->SetMaterial(material);
}

Material* Model::GetMaterial() const {
    return _mesh->GetMaterial();
}

void Model::ToggleShowSkeleton() {
    _showSkeleton = !_showSkeleton;
}

void Model::SetLocalTransform(const Matrix4& transform) {
    if(_skeleton) {
        _skeleton->SetLocalTransform(transform);
    }
    if(_mesh) {
        _mesh->SetLocalTransform(transform);
    }
    if(_skeletonInstance) {
        _skeletonInstance->SetLocalTransform(transform);
    }
}

void Model::ShowSkeleton(bool value /*= true*/) {
    _showSkeleton = value;
}
