#pragma once

#include <functional>

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector3.hpp"

class Texture2D;
class Particle;
class Material;
class SimpleRenderer;

class ParticleRenderState {
public:
    enum ParticleShape {
        QUAD,
        CUBE,
    };
    ParticleRenderState();

    ParticleRenderState(const ParticleRenderState& other) = default;
    ParticleRenderState& operator=(const ParticleRenderState& rhs) = default;

    void SetLifetime(float lifetimeSeconds);
    void SetScales(const Vector3& start, const Vector3& end);

    void SetColors(const Rgba& start, const Rgba& end);
    const Rgba& GetColor() const;
    Rgba& GetColor();
    
    void SetMaterial(Material* tex);
    void SetBillboarding(bool isBillboarded);

    void SetShape(const ParticleShape& renderShape);
    const ParticleShape& GetShape() const;

private:
    Vector3 start_scale;
    Vector3 scale;
    Vector3 end_scale;
    float start_age;
    float age;
    Rgba start_color;
    Rgba color;
    Rgba end_color;
    ParticleShape shape;
    Material* particle_material;
    bool billboarded;

    friend class Particle;
};

class ParticleState {
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    float mass;

    ParticleState();
    ParticleState& operator=(const ParticleState& rhs) = default;
    ParticleState operator*(float scalar);
    ParticleState operator*=(float scalar);
    ParticleState operator+(const ParticleState& rhs);
    ParticleState operator+=(const ParticleState& rhs);
    
    friend class Particle;
};

using ParticleIntegrator = std::function<ParticleState(float time, const ParticleState& state)>;

class Particle {
public:
    static ParticleIntegrator semi_implicit_euler;

    Particle(const ParticleRenderState& initialRenderState, const ParticleState& initialState);
    Particle(const ParticleRenderState& initialRenderState, const ParticleState& initialState, const ParticleIntegrator& model);
    Particle(const Particle& other) = default;
    Particle& operator=(const Particle& rhs) = default;
	~Particle() = default;

    const ParticleState& GetState() const;
    ParticleState& GetState();

    const ParticleRenderState& GetRenderState() const;
    ParticleRenderState& GetRenderState();

    void SetIntegrationModel(const ParticleIntegrator& model);

    void Update(float time, float deltaSeconds);
    void Render(SimpleRenderer* renderer) const;

    bool IsAlive() const;
    bool IsDead() const;
    void Kill();

    void SetParentTransform(const Matrix4& transform);
    const Matrix4& GetParentTransform() const;
    Matrix4& GetParentTransform();

    void SetExtents(const Vector3& extents);
    const Vector3& GetExtents() const;
    Vector3& GetExtents();



protected:
private:
    ParticleRenderState _renderState;
    ParticleState _nextState;
    ParticleState _curState;
    ParticleIntegrator _model;
    Matrix4 _parentTransform;
    Vector3 _extents;
};

bool operator<(const Particle& a, const Particle& b);