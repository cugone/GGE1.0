#pragma once

#include <string>

class Vector3;
class IntVector2;
class IntVector4;

class IntVector3 {
public:
    int x;
    int y;
    int z;

    static const IntVector3 ZERO;
    static const IntVector3 ONE;
    static const IntVector3 X_AXIS;
    static const IntVector3 Y_AXIS;
    static const IntVector3 Z_AXIS;

    IntVector3()=default;
    explicit IntVector3(const std::string& value);
    IntVector3(const IntVector3& other) = default;
    IntVector3(IntVector3&& mother) = default;
    IntVector3& operator=(const IntVector3& rhs) = default;
    IntVector3& operator=(IntVector3&& mrhs) = default;
    ~IntVector3() = default;
    bool operator==(const IntVector3& rhs);
    bool operator!=(const IntVector3& rhs);

    IntVector3 operator+(const IntVector3& rhs) const;
    IntVector3 operator-(const IntVector3& rhs) const;
    IntVector3& operator+=(const IntVector3& rhs);
    IntVector3& operator-=(const IntVector3& rhs);

    IntVector3 operator/(int rhs) const;
    IntVector3& operator/=(int rhs);

    IntVector3 operator*(int rhs) const;
    IntVector3& operator*=(int rhs);

    explicit IntVector3(int initialX, int initialY, int initialZ);
    explicit IntVector3(const Vector3& v);
    explicit IntVector3(const IntVector4& iv4);
    IntVector3(const IntVector2& iv2, int initialZ);

    void SetXYZ(int newX, int newY, int newZ);

    friend IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float fraction);
};