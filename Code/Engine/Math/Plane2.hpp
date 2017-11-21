#pragma once

#include "Engine/Math/Vector2.hpp"

class Plane2 {
public:
    Vector2 normal;
    float dist;

    Plane2();
	Plane2(const Vector2& normal, float distFromOrigin);
	~Plane2();

    friend Plane2 Interpolate(const Plane2& a, const Plane2& b, float fraction);
protected:
private:
	
};