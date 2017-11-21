#pragma once

#include <string>

#include "Engine/Math/IntVector3.hpp"

class Vector4;
class Vector2;
class Quaternion;

class Vector3 {
public:
    static const Vector3 ZERO;
    static const Vector3 ONE;
    static const Vector3 X_AXIS;
    static const Vector3 Y_AXIS;
    static const Vector3 Z_AXIS;


    Vector3();
    explicit Vector3(const std::string& value);
    Vector3(const Vector3& copy)=default;
    Vector3(Vector3&& r_other)=default;
    Vector3& operator=(const Vector3& rhs)=default;
    Vector3& operator=(Vector3&& rrhs)=default;
    Vector3(float initialX, float initialY, float initialZ);
    explicit Vector3(const IntVector3& intVector);
    explicit Vector3(const Quaternion& q);
    explicit Vector3(const Vector4& rhs);
    Vector3(const Vector2& xy, float initialZ);
    void GetXYZ(float& out_x, float& out_y, float& out_z) const;
    const float* GetAsFloatArray() const;
    float* GetAsFloatArray();
    float CalcLength() const;
    float CalcLengthSquared() const;
    void SetXYZ(float newX, float newY, float newZ);
    float Normalize();
    Vector3 GetNormalize() const;
    void ScaleUniform(float scale);
    void ScaleNonUniform(const Vector3& perAxisScaleFactors);
    void InverseScaleNonUniform(const Vector3& perAxisDivisors);

    bool operator==(const Vector3& vectorToEqual) const;
    bool operator!=(const Vector3& vectorToNotEqual) const;
    const Vector3 operator+(const Vector3& vectorToAdd) const;
    const Vector3 operator-(const Vector3& vectorToSubtract) const;
    const Vector3 operator*(float scale) const;
    const Vector3 operator*(const Vector3& perAxisScaleFactors) const;
    const Vector3 operator/(float inverseScale) const;
    void operator*=(float scale);
    void operator*=(const Vector3& perAxisScaleFactors);
    void operator+=(const Vector3& vectorToAdd);
    void operator-=(const Vector3& vectorToSubtract);
    Vector3 operator-() const;
    bool operator<(const Vector3& rhs) const;

    explicit operator std::string() const;
    friend std::ostream& operator<<(std::ostream& out_stream, const Vector3& v);
    friend std::istream& operator>>(std::istream& in_stream, Vector3& v);

    friend float CalcDistance(const Vector3& positionA, const Vector3& positionB);
    friend float CalcDistanceSquared(const Vector3& posA, const Vector3& posB);
    friend const Vector3 operator*(float scale, const Vector3& vectorToScale);
    friend Vector3 CrossProduct(const Vector3& a, const Vector3& b);

    friend Vector3 Interpolate(const Vector3& a, const Vector3& b, float fraction);
    friend void swap(Vector3& a, Vector3& b);

    float x;
    float y;
    float z;
};


inline Vector3::Vector3() {
    /* DO NOTHING */
}

inline Vector3::Vector3(float initialX, float initialY, float initialZ)
    : x(initialX)
    , y(initialY)
    , z(initialZ) {
    /* DO NOTHING */
}

inline Vector3::Vector3(const IntVector3& intVector)
    : x(static_cast<float>(intVector.x))
    , y(static_cast<float>(intVector.y))
    , z(static_cast<float>(intVector.z)) {
    /* DO NOTHING */
}
