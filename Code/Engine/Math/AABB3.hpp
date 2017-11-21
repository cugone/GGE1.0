#pragma once

#include "Engine/Math/Vector3.hpp"

class AABB3 {
public:
    Vector3 mins;
    Vector3 maxs;

    static const AABB3 ZERO_TO_ONE;
    static const AABB3 NEG_ONE_TO_ONE;

    AABB3();
    AABB3(const AABB3& copy);
    ~AABB3();

    explicit AABB3(float initialX, float initialY, float initalZ);
    explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    explicit AABB3(const Vector3& mins, const Vector3& maxs);
    explicit AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ);

    void StretchToIncludePoint(const Vector3& point);
    void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius);
    void Translate(const Vector3& translation);

    bool IsPointInside(const Vector3& point) const;
    const Vector3 CalcDimensions() const;
    const Vector3 CalcCenter() const;

    const AABB3 operator+(const Vector3& translation) const;
    const AABB3 operator-(const Vector3& antiTranslation) const;
    void operator+=(const Vector3& translation);
    void operator-=(const Vector3& antiTranslation);

    friend Vector3 CalcClosestPoint(const Vector3& p, const AABB3& aabb);
    friend AABB3 Interpolate(const AABB3& a, const AABB3& b, float fraction);
protected:
private:
};