#include "Engine/Math/IntVector3.hpp"

#include <cmath>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Vector3.hpp"

const IntVector3 IntVector3::ZERO(0, 0, 0);
const IntVector3 IntVector3::ONE(1, 1, 1);
const IntVector3 IntVector3::X_AXIS(1, 0, 0);
const IntVector3 IntVector3::Y_AXIS(0, 1, 0);
const IntVector3 IntVector3::Z_AXIS(0, 0, 1);

IntVector3 IntVector3::operator+(const IntVector3& rhs) const {
    return IntVector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

IntVector3 IntVector3::operator-(const IntVector3& rhs) const {
    return IntVector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

IntVector3& IntVector3::operator-=(const IntVector3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

IntVector3::IntVector3(int initialX, int initialY, int initialZ)
: x(initialX)
, y(initialY)
, z(initialZ)
{
    /* DO NOTHING */
}

IntVector3::IntVector3(const Vector3& v)
    : x(static_cast<int>(std::floor(v.x)))
    , y(static_cast<int>(std::floor(v.y)))
    , z(static_cast<int>(std::floor(v.z)))
{
    /* DO NOTHING */
}

IntVector3::IntVector3(const IntVector2& iv2, int initialZ)
    : x(iv2.x)
    , y(iv2.y)
    , z(initialZ)
{
    /* DO NOTHING */
}

IntVector3::IntVector3(const std::string& value)
    : x(0)
    , y(0)
    , z(0)
{
    if(value[0] == '[') {
        if(value.back() == ']') {
            std::stringstream ss(value.substr(1, value.size() - 1));
            std::string curLine;
            for(int i = 0; std::getline(ss, curLine, ','); ++i) {
                switch(i) {
                    case 0: x = std::stoi(curLine); break;
                    case 1: y = std::stoi(curLine); break;
                    case 2: z = std::stoi(curLine); break;
                }
            }
        }
    }
}

IntVector3::IntVector3(const IntVector4& iv4)
    : x(iv4.x)
    , y(iv4.y)
    , z(iv4.z)
{
    /* DO NOTHING */
}

void IntVector3::SetXYZ(int newX, int newY, int newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

IntVector3& IntVector3::operator+=(const IntVector3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

bool IntVector3::operator!=(const IntVector3& rhs) {
    return !(*this == rhs);
}

bool IntVector3::operator==(const IntVector3& rhs) {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

IntVector3 IntVector3::operator/(int rhs) const {
    return IntVector3(x / rhs, y / rhs, z / rhs);
}

IntVector3& IntVector3::operator/=(int rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

IntVector3 IntVector3::operator*(int rhs) const {
    return IntVector3(x * rhs, y * rhs, z * rhs);
}

IntVector3& IntVector3::operator*=(int rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float fraction) {
    float Ax = static_cast<float>(a.x);
    float Ay = static_cast<float>(a.y);
    float Az = static_cast<float>(a.z);

    float Bx = static_cast<float>(b.x);
    float By = static_cast<float>(b.y);
    float Bz = static_cast<float>(b.z);

    float x = MathUtils::Interpolate(Ax, Bx, fraction);
    float y = MathUtils::Interpolate(Ay, By, fraction);
    float z = MathUtils::Interpolate(Az, Bz, fraction);

    return IntVector3(Vector3(x, y, z));
}