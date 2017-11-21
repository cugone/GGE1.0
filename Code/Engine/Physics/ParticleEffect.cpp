#include "Engine/Physics/ParticleEffect.hpp"

ParticleEffect::ParticleEffect(const XMLElement& element) {
    LoadFromXml(element);
}

void ParticleEffect::PlayOnce(bool value) {
    _definition->_destroy_on_finish = value;
}

bool ParticleEffect::PlayOnce() const {
    return _definition->_destroy_on_finish;
}

bool ParticleEffect::IsFinished() const {
    float lifetime = this->_definition->_lifetime;
    auto oldest_emitter = std::max_element(_emitters.begin(), _emitters.end(), [](const ParticleEmitter& a, const ParticleEmitter& b) { return a.GetAge() < b.GetAge(); });
    return oldest_emitter->GetAge() > lifetime;
}

void ParticleEffect::BeginFrame() {
    for(auto& emitter : _emitters) {
        emitter.BeginFrame();
    }
}

void ParticleEffect::Update(float time, float deltaSeconds) {
    for(auto& emitter : _emitters) {
        emitter.Update(time, deltaSeconds);
    }
}

void ParticleEffect::Render() const {
    for(auto& emitter : _emitters) {
        emitter.Render();
    }
}

void ParticleEffect::EndFrame() {
    for(auto& emitter : _emitters) {
        emitter.EndFrame();
    }
}
void ParticleEffect::LoadFromXml(const XMLElement& element) {
    
    _definition = new ParticleEffectDefinition;
    _definition->_name = DataUtils::ParseXmlAttribute(element, std::string("name"), std::string("UNNAMED_PARTICLE_EFFECT"));

    for(auto emitterXml = element.FirstChildElement("ParticleEmitter");
        emitterXml != nullptr;
        emitterXml = emitterXml->NextSiblingElement("ParticleEmitter")) {

        _emitters.emplace_back(ParticleEmitter());

    }

}