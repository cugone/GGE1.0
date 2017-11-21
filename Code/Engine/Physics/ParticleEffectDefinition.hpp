#pragma once

#include <vector>

class ParticleEmitterDefinition;

class ParticleEffectDefinition {
public:
    std::string _name;
    std::vector<ParticleEmitterDefinition*> _particleEffectDefs;
    float _lifetime;
    bool _destroy_on_finish;
};


