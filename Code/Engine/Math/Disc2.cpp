#include "Engine/Math/Disc2.hpp"

#include "Engine/Math/MathUtils.hpp"

const Disc2 Disc2::UNIT_CIRCLE(0.0f, 0.0f, 1.0f);

Disc2::Disc2()
{
    /* DO NOTHING */
}

Disc2::Disc2(const Disc2& copy) :
center(copy.center),
radius(copy.radius)
{
    /* DO NOTHING */
}

Disc2::Disc2(float initialX, float initialY, float initialRadius) :
center(initialX, initialY),
radius(initialRadius)
{
    /* DO NOTHING */
}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius) :
center(initialCenter),
radius(initialRadius)
{
    /* DO NOTHING */
}

Disc2::~Disc2()
{
    /* DO NOTHING */
}

void Disc2::StretchToIncludePoint(const Vector2& point) {
    if(CalcDistanceSquared(center, point) < (radius * radius)) {
        return;
    }
    radius = CalcDistance(center, point);
}

void Disc2::AddPadding(float paddingRadius) {
    radius += paddingRadius;
}

void Disc2::Translate(const Vector2& translation) {
    center += translation;
}

bool Disc2::IsPointInside(const Vector2& point) const {
    return CalcDistanceSquared(center, point) < (radius * radius);
}

bool Disc2::IsPointOn(const Vector2& point) const {
    float distanceSquared = CalcDistanceSquared(center, point);
    float radiusSquared = radius * radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

const Disc2 Disc2::operator+(const Vector2& translation) const {
    return Disc2(center + translation, radius);
}

const Disc2 Disc2::operator-(const Vector2& antiTranslation) const {
    return Disc2(center - antiTranslation, radius);
}

void Disc2::operator-=(const Vector2& antiTranslation) {
    center -= antiTranslation;
}

void Disc2::operator+=(const Vector2& translation) {
    center += translation;
}

Disc2 Interpolate(const Disc2& a, const Disc2& b, float fraction) {
    Vector2 p = MathUtils::Interpolate(a.center, b.center, fraction);
    float r = MathUtils::Interpolate(a.radius, b.radius, fraction);

    return Disc2(p, r);
}