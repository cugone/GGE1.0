#include "Engine/Physics/ParticleSystem.hpp"

#include "Engine/EngineConfig.hpp"

#include "Engine/Math/Matrix4.hpp"

#include "Engine/Physics/ParticleEffect.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

ParticleSystem::ParticleSystem(SimpleRenderer* renderer)
    : _renderer(renderer)
{
    /* DO NOTHING */
}

void ParticleSystem::PlayOnce(const char* name, const Matrix4& transform) {
    PlayOnce(std::string(name ? name : ""), transform);
}

void ParticleSystem::PlayOnce(const std::string& name, const Matrix4& /*transform*/) {
    auto effect = _particleEffects.find(name);
    if(effect != _particleEffects.end()) {
        (*effect).second->PlayOnce(true);
    }
}

void ParticleSystem::Initialize() {
    /* DO NOTHING */
}

void ParticleSystem::BeginFrame() {
    for(auto& effect : _particleEffects) {
        effect.second->BeginFrame();
    }
}

void ParticleSystem::Update(float deltaSeconds) {
    Update(_renderer->_time_data.game_time, deltaSeconds);
}

void ParticleSystem::Update(float time, float deltaSeconds) {
    for(auto& effect : _particleEffects) {
        effect.second->Update(time, deltaSeconds);
    }
}
void ParticleSystem::Render() const {
    for(auto& effect : _particleEffects) {
        effect.second->Render();
    }
}

void ParticleSystem::EndFrame() {
    for(auto& effect : _particleEffects) {
        effect.second->EndFrame();
    }

    bool didEffectFinish = false;
    for(auto& effect : _particleEffects) {
        didEffectFinish = effect.second->IsFinished();
        if(didEffectFinish) {
            DestroyEffect(effect.second);
            effect.second = nullptr;
        }
    }
    if(didEffectFinish) {
        for(auto iter = _particleEffects.begin(); iter != _particleEffects.end(); /* DO NOTHING */) {
            if((*iter).second == nullptr) {
                iter = _particleEffects.erase(iter);
            } else {
                ++iter;
            }
        }
    }
}

bool ParticleSystem::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}

void ParticleSystem::DestroyEffect(ParticleEffect* effect) {
    delete effect;
}