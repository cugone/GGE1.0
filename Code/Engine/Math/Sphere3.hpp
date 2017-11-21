#pragma once

#include "Engine/Math/Vector3.hpp"

class Sphere3 {
public:

    Vector3 center;
    float radius;

    static const Sphere3 UNIT_CIRCLE;

    Sphere3();
    Sphere3(const Sphere3& copy);
    explicit Sphere3(float initialX, float initialY, float initialZ, float initialRadius);
    explicit Sphere3(const Vector3& initialCenter, float initialRadius);
    ~Sphere3();

    void StretchToIncludePoint(const Vector3& point);
    void AddPadding(float paddingRadius);
    void Translate(const Vector3& translation);

    bool IsPointInside(const Vector3& point) const;
    bool IsPointOn(const Vector3& point) const;

    const Sphere3 operator+(const Vector3& translation) const;
    const Sphere3 operator-(const Vector3& antiTranslation) const;
    void operator+=(const Vector3& translation);
    void operator-=(const Vector3& antiTranslation);

    friend Sphere3 Interpolate(const Sphere3& a, const Sphere3& b, float fraction);

protected:
private:

};