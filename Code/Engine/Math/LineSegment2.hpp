#pragma once

#include "Engine/Math/Vector2.hpp"

class LineSegment2 {
public:
    Vector2 start;
    Vector2 end;

    static const LineSegment2 UNIT_HORIZONTAL;
    static const LineSegment2 UNIT_VERTICAL;
    static const LineSegment2 UNIT_CENTERED_HORIZONTAL;
    static const LineSegment2 UNIT_CENTERED_VERTICAL;

	LineSegment2();
    explicit LineSegment2(float startX, float startY, float endX, float endY);
    explicit LineSegment2(const Vector2& start_position, const Vector2& end_position);
    explicit LineSegment2(const Vector2& start_position, const Vector2& normalized_direction, float length);
    explicit LineSegment2(const Vector2& start_position, float angleDegrees, float length);
    LineSegment2(const LineSegment2& copy);
	~LineSegment2();

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

    const LineSegment2 operator+(const Vector2& translation) const;
    const LineSegment2 operator-(const Vector2& antiTranslation) const;
    void operator+=(const Vector2& translation);
    void operator-=(const Vector2& antiTranslation);

    bool IsPointOn(const Vector2& p) const;

    friend float CalcDistance(const Vector2& p, const LineSegment2& line);
    friend float CalcDistanceSquared(const Vector2& p, const LineSegment2& line);
    friend Vector2 CalcClosestPoint(const Vector2& p, const LineSegment2& line);
    friend LineSegment2 Interpolate(const LineSegment2& a, const LineSegment2& b, float fraction);

protected:
private:
	void SetAngle(float angleDegrees);

    friend class Capsule2;
};