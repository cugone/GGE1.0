#include "Engine/Math/LineSegment3.hpp"

#include <cmath>

#include "Engine/Math/MathUtils.hpp"

const LineSegment3 LineSegment3::UNIT_HORIZONTAL(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);

const LineSegment3 LineSegment3::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

const LineSegment3 LineSegment3::UNIT_DEPTH(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

const LineSegment3 LineSegment3::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f);

const LineSegment3 LineSegment3::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f);

const LineSegment3 LineSegment3::UNIT_CENTERED_DEPTH(0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.5f);

LineSegment3::LineSegment3() {
    /* DO NOTHING */
}

LineSegment3::LineSegment3(float startX, float startY, float startZ, float endX, float endY, float endZ)
: start(startX, startY, startZ)
, end(endX, endY, endZ)
{
    /* DO NOTHING */
}

LineSegment3::LineSegment3(const Vector3& start_position, const Vector3& end_position)
: start(start_position)
, end(end_position)
{
    /* DO NOTHING */
}

LineSegment3::LineSegment3(const Vector3& start_position, const Vector3& normalized_direction, float length)
    : start(start_position)
    , end(start_position + normalized_direction * length)
{
    /* DO NOTHING */
}

LineSegment3::LineSegment3(const LineSegment3& copy)
    : start(copy.start)
    , end(copy.end)
{
    /* DO NOTHING */
}

LineSegment3::~LineSegment3() {
    /* DO NOTHING */
}

Vector3 LineSegment3::CalcCenter() const {
    return ((end - start) * 0.50f);
}

float LineSegment3::CalcLength() const {
    return std::sqrtf(CalcLengthSquared());
}

float LineSegment3::CalcLengthSquared() const {
    return (end - start).CalcLengthSquared();
}

void LineSegment3::SetStartEndPositions(const Vector3& start_position, const Vector3& end_position) {
    start = start_position;
    end = end_position;
}

void LineSegment3::Translate(const Vector3& translation) {
    start += translation;
    end += translation;
}

Vector3 LineSegment3::CalcDisplacement() const {
    return end - start;
}

Vector3 LineSegment3::CalcDirection() const {
    return (end - start).GetNormalize();
}

const LineSegment3 LineSegment3::operator+(const Vector3& translation) const {
    return LineSegment3(start + translation, end + translation);
}

const LineSegment3 LineSegment3::operator-(const Vector3& antiTranslation) const {
    return LineSegment3(start - antiTranslation, end - antiTranslation);
}

void LineSegment3::operator-=(const Vector3& antiTranslation) {
    start -= antiTranslation;
    end -= antiTranslation;
}

bool LineSegment3::IsPointOn(const Vector3& p) const {
    return MathUtils::IsEquivalent(CalcDistanceSquared(p, *this), 0.0f);
}

void LineSegment3::operator+=(const Vector3& translation) {
    start += translation;
    end += translation;
}

float CalcDistance(const Vector3& p, const LineSegment3& line) {
    return std::sqrtf(CalcDistanceSquared(p, line));
}

float CalcDistanceSquared(const Vector3& p, const LineSegment3& line) {
    return CalcDistanceSquared(p, CalcClosestPoint(p, line));
}

Vector3 CalcClosestPoint(const Vector3& p, const LineSegment3& line) {
    //Voronoi Regions
    Vector3 D = line.end - line.start;
    Vector3 T = D.GetNormalize();
    Vector3 SP = p - line.start;
    float regionI = MathUtils::DotProduct(T, SP);
    if(regionI < 0.0f) {
        return line.start;
    }

    Vector3 EP = p - line.end;
    float regionII = MathUtils::DotProduct(T, EP);
    if(regionII > 0.0f) {
        return line.end;
    }

    Vector3 directionSE = D.GetNormalize();
    float lengthToClosestPoint = MathUtils::DotProduct(directionSE, SP);
    Vector3 C = directionSE * lengthToClosestPoint;
    Vector3 ConL = line.start + C;
    return ConL;
}

LineSegment3 Interpolate(const LineSegment3& a, const LineSegment3& b, float fraction) {
    Vector3 start = Interpolate(a.start, b.start, fraction);
    Vector3 end = Interpolate(a.end, b.end, fraction);
    return LineSegment3(start, end);
}