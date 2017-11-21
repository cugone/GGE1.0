#pragma once

#include <vector>

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Physics/Particle.hpp"

class ParticleEmitterDefinition;
class Texture2D;

class ParticleEmitter {
public:
    ParticleEmitter() = default;
    ~ParticleEmitter() = default;

    ParticleEmitter(const XMLElement& element);

    void Initialize();
    void BeginFrame();
    void Update(float time, float deltaSeconds);
    void Render() const;
    void EndFrame();

    float GetAge() const;

protected:
private:
    void LoadFromXML(const XMLElement& element);
    bool IsDead() const;
    bool IsAlive() const;
    void Kill();

    void SpawnParticle(const Vector3& initialPosition, const Vector3& initialVelocity, float ttl, const Rgba& color /*= Rgba::WHITE*/, const Rgba& endColor /*= Rgba::WHITE*/, const Vector3& scale /*= Vector3::ONE*/, const Vector3& endScale /*= Vector3::ONE*/, Material* initialMaterial /*= nullptr*/, float initialMass /*= 1.0f*/);
    void UpdateParticles(float time, float deltaSeconds);
    void DestroyDeadEntities();
    ParticleEmitterDefinition* _definition;
    std::vector<Particle> _particles;
    float _age;
};