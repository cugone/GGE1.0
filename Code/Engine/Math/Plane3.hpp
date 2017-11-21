#pragma once

#include "Engine/Math/Vector3.hpp"

class Plane3 {
public:
    Vector3 normal; //ABC
    float dist; //-D

    Plane3();
    Plane3(const Vector3& n, float distanceFromOrigin);
    ~Plane3();

    friend Plane3 Interpolate(const Plane3& a, const Plane3& b, float fraction);
protected:
private:

};