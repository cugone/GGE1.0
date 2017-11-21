#pragma once

#include "Engine/Math/Vector3.hpp"

class LineSegment3 {
public:
    Vector3 start;
    Vector3 end;

    static const LineSegment3 UNIT_HORIZONTAL;
    static const LineSegment3 UNIT_VERTICAL;
    static const LineSegment3 UNIT_DEPTH;
    static const LineSegment3 UNIT_CENTERED_HORIZONTAL;
    static const LineSegment3 UNIT_CENTERED_VERTICAL;
    static const LineSegment3 UNIT_CENTERED_DEPTH;

    LineSegment3();
    explicit LineSegment3(float startX, float startY, float startZ, float endX, float endY, float endZ);
    explicit LineSegment3(const Vector3& start_position, const Vector3& end_position);
    explicit LineSegment3(const Vector3& start_position, const Vector3& normalized_direction, float length);
    LineSegment3(const LineSegment3& copy);
    ~LineSegment3();

    Vector3 CalcCenter() const;

    float CalcLength() const;
    float CalcLengthSquared() const;

    void SetStartEndPositions(const Vector3& start_position, const Vector3& end_position);

    void Translate(const Vector3& translation);

    Vector3 CalcDisplacement() const;
    Vector3 CalcDirection() const;

    const LineSegment3 operator+(const Vector3& translation) const;
    const LineSegment3 operator-(const Vector3& antiTranslation) const;
    void operator+=(const Vector3& translation);
    void operator-=(const Vector3& antiTranslation);

    bool IsPointOn(const Vector3& p) const;

    friend float CalcDistance(const Vector3& p, const LineSegment3& line);
    friend float CalcDistanceSquared(const Vector3& p, const LineSegment3& line);
    friend Vector3 CalcClosestPoint(const Vector3& p, const LineSegment3& line);
    friend LineSegment3 Interpolate(const LineSegment3& a, const LineSegment3& b, float fraction);
protected:
private:

    friend class Capsule3;
};