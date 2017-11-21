#pragma once

#include <string>
#include <map>

#include "Engine/Core/EngineSubsystem.hpp"

class Matrix4;
class ParticleEffect;
class SimpleRenderer;

class ParticleSystem : public EngineSubsystem {
public:
    ParticleSystem(SimpleRenderer* renderer);
    virtual ~ParticleSystem() override = default;

    void PlayOnce(const char* name, const Matrix4& transform);
    void PlayOnce(const std::string& name, const Matrix4& transform);

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

protected:
private:
    void Update(float time, float deltaSeconds);
    void DestroyEffect(ParticleEffect* effect);

    SimpleRenderer* _renderer;
    std::map<std::string, ParticleEffect*> _particleEffects;
};