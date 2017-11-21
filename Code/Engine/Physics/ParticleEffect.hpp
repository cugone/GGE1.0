#pragma once

#include <vector>

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Physics/ParticleEmitter.hpp"
#include "Engine/Physics/ParticleEffectDefinition.hpp"

class ParticleEffect {
public:
	ParticleEffect() = default;
    ParticleEffect(const XMLElement& element);
	~ParticleEffect() = default;

    bool PlayOnce() const;
    void PlayOnce(bool value);
    bool IsFinished() const;

    void BeginFrame();
    void Update(float time, float deltaSeconds);
    void Render() const;
    void EndFrame();

protected:
private:
    ParticleEffectDefinition* _definition;
    std::vector<ParticleEmitter> _emitters;

    void LoadFromXml(const XMLElement& element);
};
