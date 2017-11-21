#include "Engine/Physics/ParticleEmitter.hpp"

#include <algorithm>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Physics/ParticleEmitterDefinition.hpp"

#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Texture2D.hpp"

ParticleEmitter::ParticleEmitter(const XMLElement& element) {
    LoadFromXML(element);
}

void ParticleEmitter::Initialize() {

}

void ParticleEmitter::BeginFrame() {

}

void ParticleEmitter::Update(float time, float deltaSeconds) {
    _definition->_lifetime -= deltaSeconds;
    if(_definition->_lifetime > 0.0f) {

        Vector3 x = Vector3(0.0f, 0.0f, 0.0f);
        float rand_float_norm_x = MathUtils::GetRandomFloatInRange(-1.0f, 1.0f) * 0.25f;
        float rand_float_norm_y = MathUtils::GetRandomFloatInRange(0.0f, 1.0f) * 10.0f;
        //float rand_float_norm_z = MathUtils::GetRandomFloatInRange(-1.0f, 1.0f);
        Vector3 v = Vector3(rand_float_norm_x, rand_float_norm_y, 0.0f);
        Rgba c = Rgba::RED;
        Rgba ec = Rgba::ORANGE;
        Vector3 s = Vector3::ONE * 0.50f;
        Vector3 es = Vector3::ONE * 0.50f;
        SpawnParticle(x, v, _definition->_lifetime, c, ec, s, es, _definition->_material, _definition->_mass);
        UpdateParticles(time, deltaSeconds);
    } else {
        for(auto& p : _particles) {
            p.Kill();
        }
    }
}

void ParticleEmitter::Render() const {

}

void ParticleEmitter::EndFrame() {

}

float ParticleEmitter::GetAge() const {
    return _age;
}

void ParticleEmitter::LoadFromXML(const XMLElement& /*element*/) {

}


bool ParticleEmitter::IsDead() const {
    return false;
}
bool ParticleEmitter::IsAlive() const {
    return true;
}
void ParticleEmitter::Kill() {
    
}

void ParticleEmitter::UpdateParticles(float time, float deltaSeconds) {

    std::sort(_particles.begin(), _particles.end());
    Matrix4 result = Matrix4::GetIdentity();
    Matrix4 loc = Matrix4::CreateTranslationMatrix(_definition->_position);
    Matrix4 billboard = Matrix4::GetIdentity(); // (_definition->_is_billboarded ? (Matrix4::CalculateInverse(_camera->GetViewMatrix())) : (Matrix4::GetIdentity()));
    result = loc * billboard;
    for(Particle& p : _particles) {
        p.SetParentTransform(result);
        p.Update(time, deltaSeconds);
    }
}

void ParticleEmitter::SpawnParticle(const Vector3& initialPosition, const Vector3& initialVelocity,
                         float ttl,
                         const Rgba& color /*= Rgba::WHITE*/, const Rgba& endColor /*= Rgba::WHITE*/,
                         const Vector3& scale /*= Vector3::ONE*/, const Vector3& endScale /*= Vector3::ONE*/,
                         Material* initialMaterial /*= nullptr*/, float initialMass /*= 1.0f*/) {

    ParticleState initialState;
    initialState.mass = initialMass;
    initialState.position = initialPosition;
    initialState.velocity = initialVelocity;
    initialState.acceleration = Vector3::ZERO;

    ParticleRenderState initialRender;
    initialRender.SetColors(color, endColor);
    initialRender.SetScales(scale, endScale);
    initialRender.SetLifetime(ttl);
    initialRender.SetMaterial(initialMaterial);
    initialRender.SetBillboarding(_definition->_is_billboarded);

    _particles.emplace_back(std::move(Particle(initialRender, initialState)));

}

void ParticleEmitter::DestroyDeadEntities() {
    if(_particles.empty()) {
        return;
    }

    _particles.erase( //Move to end and erase if dead
                     std::remove_if(_particles.begin(), _particles.end(), [](const Particle& p) { return p.IsDead(); }), _particles.end());
}
