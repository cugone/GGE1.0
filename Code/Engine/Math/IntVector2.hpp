#pragma once

#include <string>

class Vector2;
class IntVector3;

class IntVector2 {
public:
    int x;
    int y;

    static const IntVector2 ZERO;
    static const IntVector2 ONE;
    static const IntVector2 X_AXIS;
    static const IntVector2 Y_AXIS;

    IntVector2()=default;
    explicit IntVector2(const std::string& value);
    IntVector2(const IntVector2& other)=default;
    IntVector2(IntVector2&& mother)=default;
    IntVector2& operator=(const IntVector2& rhs)=default;
    IntVector2& operator=(IntVector2&& mrhs)=default;
    ~IntVector2()=default;
    bool operator==(const IntVector2& rhs) const;
    bool operator!=(const IntVector2& rhs) const;
    bool operator<(const IntVector2& rhs) const;

    IntVector2 operator+(const IntVector2& rhs) const;
    IntVector2 operator-(const IntVector2& rhs) const;
    IntVector2& operator+=(const IntVector2& rhs);
    IntVector2& operator-=(const IntVector2& rhs);
    IntVector2 operator/(int rhs) const;
    IntVector2& operator/=(int rhs);
    IntVector2 operator*(int rhs) const;
    IntVector2& operator*=(int rhs);

    explicit IntVector2(int initialX, int initialY);
    explicit IntVector2(const Vector2& v);
    explicit IntVector2(const IntVector3& iv3);

    void SetXY(int newX, int newY);

    friend IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float fraction);

};