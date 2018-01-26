#pragma once

#include <cmath>
#include <string>
#include <type_traits>

#include "Engine/Math/IntVector4.hpp"
//#include "Engine/Math/MathUtils.hpp"

class Vector3;
class Vector2;

class Vector4 {
public:
    static const Vector4 ZERO;
    static const Vector4 ONE;
    static const Vector4 ZERO_XYZ_ONE_W;
    static const Vector4 ONE_XYZ_ZERO_W;
    static const Vector4 X_AXIS;
    static const Vector4 Y_AXIS;
    static const Vector4 Z_AXIS;
    static const Vector4 W_AXIS;

    static Vector4 CalcHomogeneous(const Vector4& v);

    Vector4();
    explicit Vector4(const std::string& value);
    Vector4(const Vector4& copy) = default;
    Vector4(Vector4&& r_other) = default;
    Vector4& operator=(const Vector4& rhs) = default;
    Vector4& operator=(Vector4&& rrhs) = default;
    Vector4(const Vector3& vec3, float initialW);
    Vector4(const Vector2& vec2, float initialZ, float initialW);
    Vector4(float initialX, float initialY, float initialZ, float initialW);
    explicit Vector4(const IntVector4& intVector);
    void GetXYZ(float& out_x, float& out_y, float& out_z) const;
    void GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const;
    const float* GetAsArray() const;
    float* GetAsArray();
    float CalcLength3D() const;
    float CalcLengthSquared3D() const;
    float CalcLength4D() const;
    float CalcLengthSquared4D() const;
    void CalcHomogeneous();
    void SetXYZ(float newX, float newY, float newZ);
    void SetXYZW(float newX, float newY, float newZ, float newW);
    float Normalize4D();
    float Normalize3D();
    Vector4 GetNormalize4D() const;
    Vector4 GetNormalize3D() const;
    void ScaleUniform(float scale);
    void ScaleNonUniform(const Vector4& perAxisScaleFactors);
    void InverseScaleNonUniform(const Vector4& perAxisDivisors);

    bool operator==(const Vector4& vectorfloatoEqual) const;
    bool operator!=(const Vector4& vectorfloatoNotEqual) const;
    const Vector4 operator+(const Vector4& vectorfloatoAdd) const;
    const Vector4 operator-(const Vector4& vectorfloatoSubtract) const;
    const Vector4 operator*(float scale) const;
    const Vector4 operator*(const Vector4& perAxisScaleFactors) const;
    const Vector4 operator/(float inverseScale) const;
    void operator*=(float scale);
    void operator*=(const Vector4& perAxisScaleFactors);
    void operator+=(const Vector4& vectorfloatoAdd);
    void operator-=(const Vector4& vectorfloatoSubtract);
    Vector4 operator-() const;

    friend std::ostream& operator>>(std::ostream& out_stream, const Vector4& v);
    friend std::istream& operator<<(std::istream& in_stream, Vector4& v);
    friend float CalcDistance(const Vector4& positionA, const Vector4& positionB);
    friend float CalcDistanceSquared(const Vector4& posA, const Vector4& posB);
    friend const Vector4 operator*(float scale, const Vector4& vectorfloatoScale);

    friend Vector4 Interpolate(const Vector4& a, const Vector4& b, float fraction);

    float x;
    float y;
    float z;
    float w;

private:

};
