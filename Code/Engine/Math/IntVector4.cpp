#include "Engine/Math/IntVector4.hpp"

#include <cmath>

#include "Engine/Math/Intvector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector4.hpp"

const IntVector4 IntVector4::ZERO(0, 0, 0, 0);
const IntVector4 IntVector4::ONE(1, 1, 1, 1);
const IntVector4 IntVector4::X_AXIS(1, 0, 0, 0);
const IntVector4 IntVector4::Y_AXIS(0, 1, 0, 0);
const IntVector4 IntVector4::Z_AXIS(0, 0, 1, 0);
const IntVector4 IntVector4::W_AXIS(0, 0, 0, 1);

IntVector4 IntVector4::operator+(const IntVector4& rhs) const {
    return IntVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

IntVector4 IntVector4::operator-(const IntVector4& rhs) const {
    return IntVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

IntVector4& IntVector4::operator-=(const IntVector4& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

IntVector4::IntVector4(int initialX, int initialY, int initialZ, int initialW)
    : x(initialX)
    , y(initialY)
    , z(initialZ)
    , w(initialW)
{
    /* DO NOTHING */
}

IntVector4::IntVector4(const Vector4& v)
    : x(static_cast<int>(std::floor(v.x)))
    , y(static_cast<int>(std::floor(v.y)))
    , z(static_cast<int>(std::floor(v.z)))
    , w(static_cast<int>(std::floor(v.w)))
{
    /* DO NOTHING */
}

IntVector4::IntVector4(const std::string& value)
    : x(0)
    , y(0)
    , z(0)
    , w(0)
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
                    case 3: w = std::stoi(curLine); break;
                }
            }
        }
    }
}

IntVector4::IntVector4(const IntVector3& xyz, int initialW)
    : x(xyz.x)
    , y(xyz.y)
    , z(xyz.z)
    , w(initialW)
{
    /* DO NOTHING */
}

void IntVector4::SetXYZW(int newX, int newY, int newZ, int newW) {
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
}

void IntVector4::SetXYZ(int newX, int newY, int newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

IntVector4& IntVector4::operator+=(const IntVector4& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

bool IntVector4::operator!=(const IntVector4& rhs) {
    return !(*this == rhs);
}

bool IntVector4::operator==(const IntVector4& rhs) {
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float fraction) {
    float Ax = static_cast<float>(a.x);
    float Ay = static_cast<float>(a.y);
    float Az = static_cast<float>(a.z);
    float Aw = static_cast<float>(a.w);

    float Bx = static_cast<float>(b.x);
    float By = static_cast<float>(b.y);
    float Bz = static_cast<float>(b.z);
    float Bw = static_cast<float>(b.w);

    float x = MathUtils::Interpolate(Ax, Bx, fraction);
    float y = MathUtils::Interpolate(Ay, By, fraction);
    float z = MathUtils::Interpolate(Az, Bz, fraction);
    float w = MathUtils::Interpolate(Aw, Bw, fraction);

    return IntVector4(Vector4(x, y, z, w));
}