#include "Engine/Math/Vector3.hpp"

#include <cmath>
#include <sstream>

#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);

const Vector3 Vector3::X_AXIS(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Y_AXIS(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Z_AXIS(0.0f, 0.0f, 1.0f);

Vector3::Vector3(const Vector4& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
{
    /* DO NOTHING */
}
Vector3::Vector3(const Vector2& xy, float initialZ)
    : x(xy.x)
    , y(xy.y)
    , z(initialZ)
{
    /* DO NOTHING */
}

Vector3::Vector3(const Quaternion& q)
: x(q.axis.x)
, y(q.axis.y)
, z(q.axis.z)
{
    Normalize();
}

Vector3::Vector3(const std::string& value)
: x(0.0f)
, y(0.0f)
, z(0.0f)
{
    if (value[0] == '[') {
        if (value.back() == ']') {
            std::stringstream ss(value.substr(1, value.size() - 1));
            std::string curLine;
            for (int i = 0; std::getline(ss, curLine, ','); ++i ) {
                switch (i) {
                case 0: x = std::stof(curLine); break;
                case 1: y = std::stof(curLine); break;
                case 2: z = std::stof(curLine); break;
                }
            }
        }
    }
}
void Vector3::GetXYZ(float& out_x, float& out_y, float& out_z) const {
    out_x = x;
    out_y = y;
    out_z = z;
}

const float* Vector3::GetAsFloatArray() const {
    return &x;
}

float* Vector3::GetAsFloatArray() {
    return const_cast<float*>(static_cast<const Vector3&>(*this).GetAsFloatArray());
}

float Vector3::CalcLength() const {
    return std::sqrtf(CalcLengthSquared());
}

float Vector3::CalcLengthSquared() const {
    return (x * x) + (y * y) + (z * z);
}

void Vector3::SetXYZ(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

float Vector3::Normalize() {
    float length = CalcLength();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        x *= inverse_length;
        y *= inverse_length;
        z *= inverse_length;
        return length;
    }
    return 0.0f;
}

Vector3 Vector3::GetNormalize() const {
    float length = CalcLength();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        return Vector3(x * inverse_length, y * inverse_length, z * inverse_length);
    }
    return Vector3::ZERO;
}

void Vector3::ScaleUniform(float scale) {
    x *= scale;
    y *= scale;
    z *= scale;
}

void Vector3::ScaleNonUniform(const Vector3& perAxisScaleFactors) {
    x *= perAxisScaleFactors.x;
    y *= perAxisScaleFactors.y;
    z *= perAxisScaleFactors.z;
}

void Vector3::InverseScaleNonUniform(const Vector3& perAxisDivisors) {
    x /= perAxisDivisors.x;
    y /= perAxisDivisors.y;
    z /= perAxisDivisors.z;
}

void Vector3::operator+=(const Vector3& vectorToAdd) {
    x += vectorToAdd.x;
    y += vectorToAdd.y;
    z += vectorToAdd.z;
}

void Vector3::operator-=(const Vector3& vectorToSubtract) {
    x -= vectorToSubtract.x;
    y -= vectorToSubtract.y;
    z -= vectorToSubtract.z;
}

const Vector3 Vector3::operator-(const Vector3& vectorToSubtract) const {
    return Vector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

bool Vector3::operator<(const Vector3& rhs) const {
    return this->x < rhs.x && this->y < rhs.y && this->z < rhs.z;
}


std::ostream& operator<<(std::ostream& out_stream, const Vector3& v) {
    out_stream << '[' << v.x << ',' << v.y << ',' << v.z << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, Vector3& v) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    in_stream.ignore(); //[
    in_stream >> x;
    in_stream.ignore(); //,
    in_stream >> y;
    in_stream.ignore(); //,
    in_stream >> z;
    in_stream.ignore(); //]

    v.x = x;
    v.y = y;
    v.z = z;

    return in_stream;
}

Vector3::operator std::string() const {
    return std::string("<") + std::to_string(x) + std::string(",") + std::to_string(y) + std::string(",") + std::to_string(z) + std::string(">");
}

const Vector3 Vector3::operator*(float scale) const {
    return Vector3(x * scale, y * scale, z * scale);
}

const Vector3 Vector3::operator*(const Vector3& perAxisScaleFactors) const {
    return Vector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

const Vector3 Vector3::operator/(float inverseScale) const {
    return Vector3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void Vector3::operator*=(const Vector3& perAxisScaleFactors) {
    x *= perAxisScaleFactors.x;
    y *= perAxisScaleFactors.y;
    z *= perAxisScaleFactors.z;
}

void Vector3::operator*=(float scale) {
    x *= scale;
    y *= scale;
    z *= scale;
}

const Vector3 Vector3::operator+(const Vector3& vectorToAdd) const {
    return Vector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

bool Vector3::operator!=(const Vector3& vectorToNotEqual) const {
    return !(*this == vectorToNotEqual);
}

bool Vector3::operator==(const Vector3& vectorToEqual) const {
    return x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z;
}

float CalcDistance(const Vector3& positionA, const Vector3& positionB) {
    return (positionB - positionA).CalcLength();
}

float CalcDistanceSquared(const Vector3& posA, const Vector3& posB) {
    return (posB - posA).CalcLengthSquared();
}

const Vector3 operator*(float scale, const Vector3& vectorToScale) {
    return Vector3(vectorToScale.x * scale, vectorToScale.y * scale, vectorToScale.z * scale);
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b) {
    float a1 = a.x;
    float a2 = a.y;
    float a3 = a.z;

    float b1 = b.x;
    float b2 = b.y;
    float b3 = b.z;

    return Vector3(a2 * b3 - a3 * b2, a3 * b1 - a1 * b3, a1 * b2 - a2 * b1);
}

Vector3 Interpolate(const Vector3& a, const Vector3& b, float fraction) {
    float x = MathUtils::Interpolate(a.x, b.x, fraction);
    float y = MathUtils::Interpolate(a.y, b.y, fraction);
    float z = MathUtils::Interpolate(a.z, b.z, fraction);
    return Vector3(x, y, z);
}

void swap(Vector3& a, Vector3& b) {
    using std::swap;
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.z, b.z);
}