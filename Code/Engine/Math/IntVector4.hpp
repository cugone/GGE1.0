#pragma once

#include <string>

class Vector4;
class IntVector3;

class IntVector4 {
public:
    int x;
    int y;
    int z;
    int w;

    static const IntVector4 ZERO;
    static const IntVector4 ONE;
    static const IntVector4 X_AXIS;
    static const IntVector4 Y_AXIS;
    static const IntVector4 Z_AXIS;
    static const IntVector4 W_AXIS;

    IntVector4() = default;
    IntVector4(const IntVector4& other) = default;
    IntVector4(IntVector4&& mother) = default;
    IntVector4& operator=(const IntVector4& rhs) = default;
    IntVector4& operator=(IntVector4&& mrhs) = default;
    ~IntVector4() = default;
    bool operator==(const IntVector4& rhs);
    bool operator!=(const IntVector4& rhs);

    IntVector4 operator+(const IntVector4& rhs) const;
    IntVector4 operator-(const IntVector4& rhs) const;
    IntVector4& operator+=(const IntVector4& rhs);
    IntVector4& operator-=(const IntVector4& rhs);

    explicit IntVector4(const std::string& value);
    explicit IntVector4(int initialX, int initialY, int initialZ, int initialW);
    explicit IntVector4(const IntVector3& xyz, int initialW);
    explicit IntVector4(const Vector4& v);

    void SetXYZW(int newX, int newY, int newZ, int newW);
    void SetXYZ(int newX, int newY, int newZ);

    friend IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float fraction);
};