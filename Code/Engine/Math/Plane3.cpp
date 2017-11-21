#include "Engine/Math/Plane3.hpp"

#include "Engine/Math/MathUtils.hpp"

Plane3::Plane3() {
    /* DO NOTHING */
}

Plane3::Plane3(const Vector3& n, float distanceFromOrigin)
: normal(n)
, dist(distanceFromOrigin)
{
    /* DO NOTHING */
}

Plane3::~Plane3() {
    /* DO NOTHING */
}

Plane3 Interpolate(const Plane3& a, const Plane3& b, float fraction) {
    Vector3 n(Interpolate(a.normal, b.normal, fraction));
    float d = MathUtils::Interpolate(a.dist, b.dist, fraction);

    return Plane3(n, d);
}