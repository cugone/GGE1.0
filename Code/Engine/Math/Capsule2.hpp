#pragma once

#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Vector2.hpp"

class Capsule2 {
public:

    LineSegment2 line;
    float radius;

	Capsule2();
    Capsule2(const LineSegment2& line, float radius);
    Capsule2(const Vector2& start_position, const Vector2& end_position, float radius);
    Capsule2(const Vector2& start_position, const Vector2& normalized_direction, float length, float radius);
    Capsule2(const Vector2& start_position, float angleDegrees, float length, float radius);
    Capsule2(const Capsule2& copy);
	~Capsule2();

    static const Capsule2 UNIT_HORIZONTAL;
    static const Capsule2 UNIT_VERTICAL;
    static const Capsule2 UNIT_CENTERED_HORIZONTAL;
    static const Capsule2 UNIT_CENTERED_VERTICAL;

    void SetLengthFromStart(float length);
    void SetLengthFromCenter(float length);
    void SetLengthFromEnd(float length);

    Vector2 CalcCenter() const;

    float CalcLength() const;
    float CalcLengthSquared() const;

    void SetDirectionFromCenter(float angleDegrees);
    void SetDirectionFromStart(float angleDegrees);
    void SetDirectionFromEnd(float angleDegrees);

    void SetStartEndPositions(const Vector2& start_position, const Vector2& end_position);

    void Translate(const Vector2& translation);
    void Rotate(float angleDegrees);
    void RotateStartPosition(float angleDegrees);
    void RotateEndPosition(float angleDegrees);

    void Rotate90Degrees();
    void RotateNegative90Degrees();
    void Rotate180Degrees();

    Vector2 CalcDisplacement() const;
    Vector2 CalcDirection() const;

    Vector2 CalcPositiveNormal() const;
    Vector2 CalcNegativeNormal() const;

    float GetRadius() const;
    float GetRadius();

    const Capsule2 operator+(const Vector2& translation) const;
    const Capsule2 operator-(const Vector2& antiTranslation) const;
    void operator+=(const Vector2& translation);
    void operator-=(const Vector2& antiTranslation);

    friend Capsule2 Interpolate(const Capsule2& a, const Capsule2& b, float fraction);

protected:
private:
    void SetAngle(float angleDegrees);
};