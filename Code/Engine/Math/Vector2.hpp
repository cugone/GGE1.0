#pragma once

#include <string>

class IntVector2;
class Vector3;

class Vector2 {
public:

    static const Vector2 ZERO;
    static const Vector2 ONE;
    static const Vector2 X_AXIS;
    static const Vector2 Y_AXIS;

	Vector2();
	Vector2(const Vector2& copy);
    explicit Vector2(const std::string& value);
	Vector2(float initialX, float initialY);
    Vector2(const Vector3& vec3);
    explicit Vector2(const IntVector2& intVector);
	void GetXY(float& out_x, float& out_y) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
	float CalcHeadingDegrees() const;
	float CalcHeadingRadians() const;
	void SetXY(float newX, float newY);
	void Rotate90Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float degrees);
	void RotateRadians(float radians);
	float Normalize();
    Vector2 GetNormalize() const;
	float SetLength(float newLength);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector2& perAxisDivisors);
	void SetHeadingDegrees(float headingDegrees);
	void SetHeadingRadians(float headingRadians);
	void SetUnitLengthAndHeadingDegrees(float headingDegrees);
	void SetUnitLengthAndHeadingRadians(float headingRadians);
	void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
	void SetLengthAndHeadingRadians(float newLength, float headingRadians);

	bool operator==(const Vector2& vectorToEqual) const;
	bool operator!=(const Vector2& vectorToNotEqual) const;
	const Vector2 operator+(const Vector2& vectorToAdd) const;
	const Vector2 operator-(const Vector2& vectorToSubtract) const;
	const Vector2 operator*(float scale) const;
	const Vector2 operator*(const Vector2& perAxisScaleFactors) const;
	const Vector2 operator/(float inverseScale) const;
	void operator*=(float scale);
	void operator*=(const Vector2& perAxisScaleFactors);
	void operator+=(const Vector2& vectorToAdd);
	void operator-=(const Vector2& vectorToSubtract);
    Vector2 operator-() const;
    friend std::ostream& operator<<(std::ostream& out_stream, const Vector2& v);
    friend std::istream& operator>>(std::istream& in_stream, Vector2& v);
	friend float CalcDistance(const Vector2& positionA, const Vector2& positionB);
	friend float CalcDistanceSquared(const Vector2& posA, const Vector2& posB);
	friend const Vector2 operator*(float scale, const Vector2& vectorToScale);

    friend Vector2 Interpolate(const Vector2& a, const Vector2& b, float fraction);

	float x;
	float y;
};