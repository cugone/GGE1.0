#include "Engine/Math/Vector2.hpp"

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <cmath>
#include <sstream>

const Vector2 Vector2::ZERO(0.0f, 0.0f);
const Vector2 Vector2::ONE(1.0f, 1.0f);
const Vector2 Vector2::X_AXIS(1.0f, 0.0f);
const Vector2 Vector2::Y_AXIS(0.0f, 1.0f);


Vector2::Vector2()
{
	/* Do nothing */
}

Vector2::Vector2(const Vector2& copy) :
x(copy.x),
y(copy.y)
{
	/* Do nothing */
}

Vector2::Vector2(float initialX, float initialY) :
x(initialX),
y(initialY)
{
	/* Do nothing */
}

Vector2::Vector2(const IntVector2& intVector)
: x(static_cast<float>(intVector.x))
, y(static_cast<float>(intVector.y))
{
    /* DO NOTHING */
}

Vector2::Vector2(const Vector3& vec3)
    : x(vec3.x)
    , y(vec3.y)
{
    /* DO NOTHING */
}

Vector2::Vector2(const std::string& value)
    : x(0.0f)
    , y(0.0f)
{
    if (value[0] == '[') {
        if (value.back() == ']') {
            std::stringstream ss(value.substr(1, value.size() - 1));
            std::string curLine;
            for(int i = 0; std::getline(ss, curLine, ','); ++i ) {
                switch(i) {
                    case 0: x = std::stof(curLine); break;
                    case 1: y = std::stof(curLine); break;
                }
            }
        }
    }
}

void Vector2::GetXY(float& out_x, float& out_y) const {
	out_x = x;
	out_y = y;
}

const float* Vector2::GetAsFloatArray() const {
	return &x;
}

float* Vector2::GetAsFloatArray() {
	return const_cast<float*>(static_cast<const Vector2&>(*this).GetAsFloatArray());
}

float Vector2::CalcLength() const {
	return std::sqrtf(CalcLengthSquared());
}

float Vector2::CalcLengthSquared() const {
	return (x * x) + (y * y);
}

float Vector2::CalcHeadingDegrees() const {
	return MathUtils::ConvertRadiansToDegrees(CalcHeadingRadians());
}

float Vector2::CalcHeadingRadians() const {
	return std::atan2f(y, x);
}

void Vector2::SetXY(float newX, float newY) {
	x = newX;
	y = newY;
}

void Vector2::Rotate90Degrees() {
	SetXY(-y, x);
}

void Vector2::RotateNegative90Degrees() {
	SetXY(y, -x);
}

void Vector2::RotateDegrees(float degrees) {
	RotateRadians(MathUtils::ConvertDegreesToRadians(degrees));
}

void Vector2::RotateRadians(float radians) {
	float R = CalcLength();
	float old_angle = std::atan2(y, x);
	float new_angle = old_angle + radians;

	x = R * std::cos(new_angle);
	y = R * std::sin(new_angle);
}

float Vector2::Normalize() {
	float length = CalcLength();
	if(length > 0.0f) {
		float inverse_length = 1.0f / length;
		x *= inverse_length;
		y *= inverse_length;
		return length;
	}
	return 0.0f;
}

Vector2 Vector2::GetNormalize() const {
    float length = CalcLength();
    if(length > 0.0f) {
        float inverse_length = 1.0f / length;
        return Vector2(x * inverse_length, y * inverse_length);
    }
    return Vector2::ZERO;
}

float Vector2::SetLength(float newLength) {
	float R = CalcLength();
	float theta = CalcHeadingRadians();
	x = newLength * std::cos(theta);
	y = newLength * std::sin(theta);
	return R;
}

void Vector2::ScaleUniform(float scale) {
	x *= scale;
	y *= scale;
}

void Vector2::ScaleNonUniform(const Vector2& perAxisScaleFactors) {
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void Vector2::InverseScaleNonUniform(const Vector2& perAxisDivisors) {
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

void Vector2::SetHeadingDegrees(float headingDegrees) {
	SetHeadingRadians(MathUtils::ConvertDegreesToRadians(headingDegrees));
}

void Vector2::SetHeadingRadians(float headingRadians) {
	float R = CalcLength();
	float theta = headingRadians;
	x = R * std::cos(theta);
	y = R * std::sin(theta);
}

void Vector2::SetUnitLengthAndHeadingDegrees(float headingDegrees) {
	SetUnitLengthAndHeadingRadians(MathUtils::ConvertDegreesToRadians(headingDegrees));
}

void Vector2::SetUnitLengthAndHeadingRadians(float headingRadians) {
	Normalize();
	SetHeadingRadians(headingRadians);
}

void Vector2::SetLengthAndHeadingDegrees(float newLength, float headingDegrees) {
	SetLengthAndHeadingRadians(newLength, MathUtils::ConvertDegreesToRadians(headingDegrees));
}

void Vector2::SetLengthAndHeadingRadians(float newLength, float headingRadians) {
	SetLength(newLength);
	SetHeadingRadians(headingRadians);
}

bool Vector2::operator==(const Vector2& vectorToEqual) const {
	return  (x == vectorToEqual.x) && (y == vectorToEqual.y);
}

bool Vector2::operator!=(const Vector2& vectorToNotEqual) const {
	return !(*this == vectorToNotEqual);
}

const Vector2 Vector2::operator+(const Vector2& vectorToAdd) const {
	return Vector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

const Vector2 Vector2::operator-(const Vector2& vectorToSubtract) const {
	return Vector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}

std::ostream& operator<<(std::ostream& out_stream, const Vector2& v) {
    out_stream << '[' << v.x << ',' << v.y << ']';
    return out_stream;
}

std::istream& operator>>(std::istream& in_stream, Vector2& v) {
    float x = 0.0f;
    float y = 0.0f;
    
    in_stream.ignore(); //[
    in_stream >> x;
    in_stream.ignore(); //,
    in_stream >> y;
    in_stream.ignore(); //]
    
    v.x = x;
    v.y = y;

    return in_stream;
}

const Vector2 Vector2::operator*(float scale) const {
	return Vector2(x * scale, y * scale);
}

const Vector2 Vector2::operator*(const Vector2& perAxisScaleFactors) const {
	return Vector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

Vector2 Interpolate(const Vector2& a, const Vector2& b, float fraction) {
    float x = MathUtils::Interpolate(a.x, b.x, fraction);
    float y = MathUtils::Interpolate(a.y, b.y, fraction);
    return Vector2(x, y);
}

const Vector2 Vector2::operator/(float inverseScale) const {
	return Vector2(x / inverseScale, y / inverseScale);
}

void Vector2::operator*=(float scale) {
	x *= scale;
	y *= scale;
}

void Vector2::operator*=(const Vector2& perAxisScaleFactors) {
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void Vector2::operator+=(const Vector2& vectorToAdd) {
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}
void Vector2::operator-=(const Vector2& vectorToSubtract) {
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

float CalcDistance(const Vector2& positionA, const Vector2& positionB) {
	return (positionB - positionA).CalcLength();
}

float CalcDistanceSquared(const Vector2& posA, const Vector2& posB) {
	return (posB - posA).CalcLengthSquared();
}

const Vector2 operator*(float scale, const Vector2& vectorToScale) {
	return Vector2(vectorToScale.x * scale, vectorToScale.y * scale);
}
