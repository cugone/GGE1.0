#include "Engine/Physics/Particle.hpp"

#include "Engine/EngineConfig.hpp"

#include "Engine/Math/Matrix4.hpp"

#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"

ParticleIntegrator Particle::semi_implicit_euler = [=](float /*time*/, const ParticleState& state)->ParticleState {
    ParticleState dS = state;
    if(dS.mass > 0.0f) {
        dS.acceleration = Vector3::ZERO;
        dS.velocity += state.acceleration;
        dS.position += state.velocity;
    }
    return dS;
};

Particle::Particle(const ParticleRenderState& initialRenderState, const ParticleState& initialState)
    : _renderState(initialRenderState)
    , _nextState(initialState)
    , _curState(initialState)
    , _model(semi_implicit_euler)
    , _parentTransform(Matrix4::GetIdentity())
    , _extents()
{
    /* DO NOTHING */
}

Particle::Particle(const ParticleRenderState& initialRenderState, const ParticleState& initialState, const ParticleIntegrator& model)
    : _renderState(initialRenderState)
    , _nextState(initialState)
    , _curState(initialState)
    , _model(model)
    , _parentTransform(Matrix4::GetIdentity())
    , _extents()
{
    /* DO NOTHING */
}

const ParticleState& Particle::GetState() const {
    return _nextState;
}

ParticleState& Particle::GetState() {
    return const_cast<ParticleState&>(static_cast<const Particle&>(*this).GetState());
}

const ParticleRenderState& Particle::GetRenderState() const {
    return _renderState;
}

ParticleRenderState& Particle::GetRenderState() {
    return const_cast<ParticleRenderState&>(static_cast<const Particle&>(*this).GetRenderState());
}

void Particle::SetIntegrationModel(const ParticleIntegrator& model) {
    _model = model;
}

void Particle::Update(float time, float deltaSeconds) {

    _renderState.age -= deltaSeconds;

    if(_renderState.age <= 0.0f) {
        Kill();
        return;
    }

    //Update visuals
    float ageRatio = _renderState.age / _renderState.start_age;
    //Interpolate "backwards" because age starts at 1 and goes to 0
    _renderState.scale = Interpolate(_renderState.end_scale, _renderState.start_scale, ageRatio);
    _renderState.color = Interpolate(_renderState.end_color, _renderState.start_color, ageRatio);

    //Update physics
    _nextState = _curState + _model(time, _nextState) * deltaSeconds;
    _curState = _nextState;

}

void Particle::Render(SimpleRenderer* renderer) const {

    if(_renderState.color.a == 0) {
        return;
    }

    const Vector3& position = _curState.position;
    const Vector3& scale = _renderState.scale;
    Matrix4 pointlight_p = _parentTransform;
    Matrix4 pointlight_neg_t = Matrix4::CreateTranslationMatrix(-position);
    Matrix4 pointlight_t = Matrix4::CreateTranslationMatrix(position);
    Matrix4 pointlight_s = Matrix4::CreateScaleMatrix(scale);
    Matrix4 pointlight_r = Matrix4::Create2DRotationDegreesMatrix(0.0f);

    Matrix4 pointlight_model = Matrix4::GetIdentity();

    pointlight_model = pointlight_r * pointlight_t * pointlight_s * pointlight_p; //Column-major, Left-side multiply

    renderer->SetModelMatrix(pointlight_model);

    renderer->SetMaterial(_renderState.particle_material);

    switch(_renderState.shape) {
        case ParticleRenderState::ParticleShape::QUAD:
        _renderState.billboarded ?
            renderer->DrawQuadTwoSided(Vector3::ZERO, Vector3::ONE * _extents, _renderState.color)
            :
            renderer->DrawQuad(Vector3::ZERO, Vector3::ONE * _extents, _renderState.color)
            ;
        break;
        case ParticleRenderState::ParticleShape::CUBE:
            renderer->DrawCube(Vector3::ZERO, Vector3::ONE * _extents, _renderState.color);
            break;
    }

    renderer->SetModelMatrix(Matrix4::GetIdentity());
}

bool Particle::IsAlive() const {
    return _renderState.age > 0.0f;
}

bool Particle::IsDead() const {
    return !IsAlive();
}
void Particle::Kill() {
    _renderState.age = 0.0f;
}

const Matrix4& Particle::GetParentTransform() const {
    return _parentTransform;
}

Matrix4& Particle::GetParentTransform() {
    return const_cast<Matrix4&>(static_cast<const Particle&>(*this).GetParentTransform());
}

void Particle::SetExtents(const Vector3& extents) {
    _extents = extents;
}

const Vector3& Particle::GetExtents() const {
    return _extents;
}

Vector3& Particle::GetExtents() {
    return const_cast<Vector3&>(static_cast<const Particle&>(*this).GetExtents());
}

void Particle::SetParentTransform(const Matrix4& transform) {
    _parentTransform = transform;
}

bool operator<(const Particle& a, const Particle& b) {
    return a.GetRenderState().GetColor().a < b.GetRenderState().GetColor().a;
}

//------------------------------------------------------------------------------
ParticleRenderState::ParticleRenderState(): start_scale(1.0f, 1.0f, 1.0f)
, scale(start_scale)
, end_scale(start_scale)
, start_age(1.0f)
, age(start_age)
, start_color(Rgba::WHITE)
, color(start_color)
, end_color(start_color)
, shape(ParticleShape::QUAD)
, particle_material(nullptr)
, billboarded(false) {
    /* DO NOTHING */
}

void ParticleRenderState::SetLifetime(float lifetimeSeconds) {
    start_age = lifetimeSeconds;
    age = start_age;
}

void ParticleRenderState::SetScales(const Vector3& start, const Vector3& end) {
    start_scale = start;
    end_scale = end;
    scale = start;
}

void ParticleRenderState::SetColors(const Rgba& start, const Rgba& end) {
    start_color = start;
    end_color = end;
    color = start;
}

const Rgba& ParticleRenderState::GetColor() const {
    return color;
}

Rgba& ParticleRenderState::GetColor() {
    return const_cast<Rgba&>(static_cast<const ParticleRenderState&>(*this).GetColor());
}

void ParticleRenderState::SetMaterial(Material* material) {
    particle_material = material;
}

void ParticleRenderState::SetBillboarding(bool isBillboarded) {
    billboarded = isBillboarded;
}

void ParticleRenderState::SetShape(const ParticleShape& renderShape) {
    shape = renderShape;
}
const ParticleRenderState::ParticleShape& ParticleRenderState::GetShape() const {
    return shape;
}

//------------------------------------------------------------------------------
ParticleState::ParticleState()
: position(0.0f, 0.0f, 0.0f)
, velocity(0.0f, 0.0f, 0.0f)
, acceleration(0.0f, 0.0f, 0.0f)
, mass(1.0f) {
    /* DO NOTHING */
}

ParticleState ParticleState::operator*=(float scalar) {
    acceleration *= scalar;
    velocity *= scalar;
    position *= scalar;
    return *this;
}

ParticleState ParticleState::operator*(float scalar) {
    ParticleState ps = *this;
    ps.acceleration = acceleration * scalar;
    ps.velocity = velocity * scalar;
    ps.position = position * scalar;
    return ps;
}

ParticleState ParticleState::operator+=(const ParticleState& rhs) {
    velocity += rhs.acceleration;
    position += rhs.velocity;
    return *this;
}

ParticleState ParticleState::operator+(const ParticleState& rhs) {
    ParticleState ps = *this;
    ps.velocity += rhs.acceleration;
    ps.position += rhs.velocity;
    return ps;
}