#include "Engine/Math/Sphere3.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector3.hpp"

const Sphere3 Sphere3::UNIT_CIRCLE(Vector3::ZERO, 1.0f);

Sphere3::Sphere3() {
    /* DO NOTHING */
}

Sphere3::Sphere3(const Sphere3& copy)
: center(copy.center)
, radius(copy.radius)
{
    /* DO NOTHING */
}

Sphere3::Sphere3(float initialX, float initialY, float initialZ, float initialRadius)
: center(initialX, initialY, initialZ)
, radius(initialRadius)
{
    /* DO NOTHING */
}

Sphere3::Sphere3(const Vector3& initialCenter, float initialRadius)
: center(initialCenter)
, radius(initialRadius)
{
    /* DO NOTHING */
}

Sphere3::~Sphere3() {
    /* DO NOTHING */
}

void Sphere3::StretchToIncludePoint(const Vector3& point) {
    if(CalcDistanceSquared(center, point) < (radius * radius)) {
        return;
    }
    radius = CalcDistance(center, point);
}

void Sphere3::AddPadding(float paddingRadius) {
    radius += paddingRadius;
}

void Sphere3::Translate(const Vector3& translation) {
    center += translation;
}

bool Sphere3::IsPointInside(const Vector3& point) const {
    return CalcDistanceSquared(center, point) < (radius * radius);
}

bool Sphere3::IsPointOn(const Vector3& point) const {
    float distanceSquared = CalcDistanceSquared(center, point);
    float radiusSquared = radius * radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

const Sphere3 Sphere3::operator+(const Vector3& translation) const {
    return Sphere3(center + translation, radius);
}

const Sphere3 Sphere3::operator-(const Vector3& antiTranslation) const {
    return Sphere3(center - antiTranslation, radius);
}

void Sphere3::operator-=(const Vector3& antiTranslation) {
    center -= antiTranslation;
}

void Sphere3::operator+=(const Vector3& translation) {
    center += translation;
}

Sphere3 Interpolate(const Sphere3& a, const Sphere3& b, float fraction) {
    Vector3 c(Interpolate(a.center, b.center, fraction));
    float r(MathUtils::Interpolate(a.radius, b.radius, fraction));

    return Sphere3(c, r);
}