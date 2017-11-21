#include "Engine/Math/Plane2.hpp"

#include "Engine/Math/MathUtils.hpp"

Plane2::Plane2() {
    /* DO NOTHING */
}

Plane2::Plane2(const Vector2& initialNormal, float distFromOrigin)
    : normal(initialNormal)
    , dist(distFromOrigin)
{
    /* DO NOTHING */
}

Plane2::~Plane2() {
    /* DO NOTHING */
}

Plane2 Interpolate(const Plane2& a, const Plane2& b, float fraction) {
    float  d = MathUtils::Interpolate(a.dist, b.dist, fraction);
    Vector2 n(Interpolate(a.normal, b.normal, fraction));

    return Plane2(n, d);
}