#include "Vector4.hpp"

#include <sstream>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"

const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Vector4::ZERO_XYZ_ONE_W(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Vector4::ONE_XYZ_ZERO_W(1.0f, 1.0f, 1.0f, 0.0f);
const Vector4 Vector4::X_AXIS(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::Y_AXIS(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::Z_AXIS(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::W_AXIS(0.0f, 0.0f, 0.0f, 1.0f);


Vector4::Vector4(const std::string& value)
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , w(0.0f)
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
                    case 3: w = std::stof(curLine); break;
                }
            }
        }
    }
}

Vector4 Vector4::CalcHomogeneous(const Vector4& v) {
    return std::fabs(v.w - 0.0f) < 0.0001f == false ? v / v.w : v;
}
void Vector4::CalcHomogeneous() {
    if(std::fabs(w - 0.0f) < 0.0001f == false) {
        x /= w;
        y /= w;
        z /= w;
        w = 1.0f;
    }
}
Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW)
    : x(initialX)
    , y(initialY)
    , z(initialZ)
, w(initialW)
{
    /* DO NOTHING */
}

Vector4::Vector4(const IntVector4& intVector)
: x(static_cast<float>(intVector.x))
, y(static_cast<float>(intVector.y))
, z(static_cast<float>(intVector.z))
, w(static_cast<float>(intVector.w)) {
    /* DO NOTHING */
}

Vector4::Vector4() {
    /* DO NOTHING */
}
Vector4::Vector4(const Vector3& vec3, float initialW)
    : x(vec3.x)
    , y(vec3.y)
    , z(vec3.z)
    , w(initialW)
{
    /* DO NOTHING */
}
void Vector4::GetXYZ(float& out_x, float& out_y, float& out_z) const {
    out_x = x;
    out_y = y;
    out_z = z;
}

void Vector4::GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const {
    GetXYZ(out_x, out_y, out_z);
    out_w = w;
}

const float* Vector4::GetAsArray() const {
    return &x;
}

float* Vector4::GetAsArray() {
    return const_cast<float*>(static_cast<const Vector4&>(*this).GetAsArray());
}

float Vector4::CalcLength3D() const {
    return std::sqrtf(CalcLengthSquared3D());
}

float Vector4::CalcLengthSquared3D() const {
    return (x * x) + (y * y) + (z * z);
}

float Vector4::CalcLength4D() const {
    return std::sqrtf(CalcLengthSquared4D());
}

float Vector4::CalcLengthSquared4D() const {
    return (x * x) + (y * y) + (z * z) + (w * w);
}

void Vector4::SetXYZ(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

void Vector4::SetXYZW(float newX, float newY, float newZ, float newW) {
    SetXYZ(newX, newY, newZ);
    w = newW;
}

float Vector4::Normalize4D() {
    float length = CalcLength4D();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        x *= inverse_length;
        y *= inverse_length;
        z *= inverse_length;
        w *= inverse_length;
        return length;
    }
    return 0.0f;
}

float Vector4::Normalize3D() {
    float length = CalcLength3D();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        x *= inverse_length;
        y *= inverse_length;
        z *= inverse_length;
        return length;
    }
    return 0.0f;
}

Vector4 Vector4::GetNormalize4D() const {
    float length = CalcLength4D();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        return Vector4(x * inverse_length, y * inverse_length,
                       z * inverse_length, w * inverse_length);
    }
    return Vector4::ZERO;
}

Vector4 Vector4::GetNormalize3D() const {
    float length = CalcLength3D();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        return Vector4(x * inverse_length, y * inverse_length, z * inverse_length, w);
    }
    return Vector4::ZERO_XYZ_ONE_W;
}

void Vector4::ScaleUniform(float scale) {
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
}

void Vector4::ScaleNonUniform(const Vector4& perAxisScaleFactors) {
    x *= perAxisScaleFactors.x;
    y *= perAxisScaleFactors.y;
    z *= perAxisScaleFactors.z;
    w *= perAxisScaleFactors.w;
}

void Vector4::InverseScaleNonUniform(const Vector4& perAxisDivisors) {
    x /= perAxisDivisors.x;
    y /= perAxisDivisors.y;
    z /= perAxisDivisors.z;
    w /= perAxisDivisors.w;
}

void Vector4::operator+=(const Vector4& vectorfloatoAdd) {
    x += vectorfloatoAdd.x;
    y += vectorfloatoAdd.y;
    z += vectorfloatoAdd.z;
    w += vectorfloatoAdd.w;
}

void Vector4::operator-=(const Vector4& vectorfloatoSubtract) {
    x -= vectorfloatoSubtract.x;
    y -= vectorfloatoSubtract.y;
    z -= vectorfloatoSubtract.z;
    w -= vectorfloatoSubtract.w;
}

const Vector4 Vector4::operator-(const Vector4& vectorfloatoSubtract) const {
    return Vector4(x - vectorfloatoSubtract.x, y - vectorfloatoSubtract.y,
                   z - vectorfloatoSubtract.z, w - vectorfloatoSubtract.w);
}


Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

const Vector4 Vector4::operator*(float scale) const {
    return Vector4(x * scale, y * scale,
                   z * scale, w * scale);
}

const Vector4 Vector4::operator*(const Vector4& perAxisScaleFactors) const {
    return Vector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y,
                   z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

const Vector4 Vector4::operator/(float inverseScale) const {
    return Vector4(x / inverseScale, y / inverseScale,
                   z / inverseScale, w / inverseScale);
}

void Vector4::operator*=(const Vector4& perAxisScaleFactors) {
    x *= perAxisScaleFactors.x;
    y *= perAxisScaleFactors.y;
    z *= perAxisScaleFactors.z;
    w *= perAxisScaleFactors.w;
}

void Vector4::operator*=(float scale) {
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
}

const Vector4 Vector4::operator+(const Vector4& vectorfloatoAdd) const {
    return Vector4(x + vectorfloatoAdd.x, y + vectorfloatoAdd.y, z + vectorfloatoAdd.z, w + vectorfloatoAdd.w);
}

bool Vector4::operator!=(const Vector4& vectorfloatoNotEqual) const {
    return !(*this == vectorfloatoNotEqual);
}

bool Vector4::operator==(const Vector4& vectorfloatoEqual) const {
    return x == vectorfloatoEqual.x && y == vectorfloatoEqual.y
        && z == vectorfloatoEqual.z && w == vectorfloatoEqual.w;
}


std::ostream& operator<<(std::ostream& out_stream, const Vector4& v) {
    out_stream << '[' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ']';
    return out_stream;
}

std::istream& operator >> (std::istream& in_stream, Vector4& v) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    in_stream.ignore(); //[
    in_stream >> x;
    in_stream.ignore(); //,
    in_stream >> y;
    in_stream.ignore(); //,
    in_stream >> z;
    in_stream.ignore(); //,
    in_stream >> w;
    in_stream.ignore(); //]

    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;

    return in_stream;
}

float CalcDistance(const Vector4& positionA, const Vector4& positionB) {
    return (positionB - positionA).CalcLength4D();
}

float CalcDistanceSquared(const Vector4& posA, const Vector4& posB) {
    return (posB - posA).CalcLengthSquared4D();
}

const Vector4 operator*(float scale, const Vector4& vectorfloatoScale) {
    return Vector4(vectorfloatoScale.x * scale, vectorfloatoScale.y * scale,
                   vectorfloatoScale.z * scale, vectorfloatoScale.w * scale);
}

Vector4 Interpolate(const Vector4& a, const Vector4& b, float fraction) {
    float x = MathUtils::Interpolate(a.x, b.x, fraction);
    float y = MathUtils::Interpolate(a.y, b.y, fraction);
    float z = MathUtils::Interpolate(a.z, b.z, fraction);
    float w = MathUtils::Interpolate(a.w, b.w, fraction);
    return Vector4(x, y, z, w);
}
