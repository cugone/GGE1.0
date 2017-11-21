#pragma once

#include "Engine/Audio/Audio.hpp"
#include "Engine/Core/DataUtils.hpp"

#include "Engine/Physics/Particle.hpp"

class Material;

class ParticleEmitterDefinition {
public:
    ParticleEmitterDefinition() = default;
    ParticleEmitterDefinition(const XMLElement& element);
    ~ParticleEmitterDefinition() = default;
protected:
private:

    ParticleState _initialState;
    ParticleRenderState _particleRenderState;

    Vector3 _position;
    float _lifetime;
    float _mass;
    Material* _material;
    bool _is_billboarded;
    bool _is_sorted;

    friend class ParticleEmitter;
};