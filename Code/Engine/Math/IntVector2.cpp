#include "Engine/Math/IntVector2.hpp"

#include <cmath>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector2.hpp"


const IntVector2 IntVector2::ZERO(0, 0);
const IntVector2 IntVector2::ONE(1, 1);
const IntVector2 IntVector2::X_AXIS(1, 0);
const IntVector2 IntVector2::Y_AXIS(0, 1);

IntVector2 IntVector2::operator+(const IntVector2& rhs) const {
    return IntVector2(x + rhs.x, y + rhs.y);
}

IntVector2 IntVector2::operator-(const IntVector2& rhs) const {
    return IntVector2(x - rhs.x, y - rhs.y);
}

IntVector2& IntVector2::operator+=(const IntVector2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

IntVector2& IntVector2::operator-=(const IntVector2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

IntVector2 IntVector2::operator/(int rhs) const {
    return IntVector2(x / rhs, y / rhs);
}

IntVector2& IntVector2::operator/=(int rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
}

IntVector2 IntVector2::operator*(int rhs) const {
    return IntVector2(x * rhs, y * rhs);
}

IntVector2& IntVector2::operator*=(int rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
}

bool IntVector2::operator<(const IntVector2& rhs) const {
    if(this->x < rhs.x) {
        return true;
    }
    if(rhs.x < this->x) {
        return false;
    }
    if(this->y < rhs.y) {
        return true;
    }
    return false;
}
IntVector2::IntVector2(int initialX, int initialY)
: x(initialX)
, y(initialY)
{
    /* DO NOTHING */
}

IntVector2::IntVector2(const Vector2& v)
: x(static_cast<int>(std::floor(v.x)))
, y(static_cast<int>(std::floor(v.y)))
{
    /* DO NOTHING */
}

IntVector2::IntVector2(const std::string& value)
    : x(0)
    , y(0)
{
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::stringstream ss(value.substr(1, value.size() - 1));
            std::string curLine;
            for(int i = 0; std::getline(ss, curLine, ','); ++i) {
                switch(i) {
                    case 0: x = std::stoi(curLine); break;
                    case 1: y = std::stoi(curLine); break;
                }
            }
        }
    }
}

IntVector2::IntVector2(const IntVector3& iv3)
    : x(iv3.x)
    , y(iv3.y)
{
    /* DO NOTHING */
}

void IntVector2::SetXY(int newX, int newY) {
    x = newX;
    y = newY;
}

bool IntVector2::operator!=(const IntVector2& rhs) const {
    return !(*this == rhs);
}

bool IntVector2::operator==(const IntVector2& rhs) const {
    return this->x == rhs.x && this->y == rhs.y;
}

IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float fraction) {
    float Ax = static_cast<float>(a.x);
    float Ay = static_cast<float>(a.y);

    float Bx = static_cast<float>(b.x);
    float By = static_cast<float>(b.y);

    float x = MathUtils::Interpolate(Ax, Bx, fraction);
    float y = MathUtils::Interpolate(Ay, By, fraction);

    return IntVector2(Vector2(x, y));
}