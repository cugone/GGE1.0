#include "Engine/Math/LineSegment2.hpp"

#include <cmath>

#include "Engine/Math/MathUtils.hpp"

const LineSegment2 LineSegment2::UNIT_HORIZONTAL(0.0f, 0.0f, 1.0f, 0.0f);
const LineSegment2 LineSegment2::UNIT_VERTICAL(0.0f, 0.0f, 0.0f, 1.0f);
const LineSegment2 LineSegment2::UNIT_CENTERED_HORIZONTAL(-0.5f, 0.0f, 0.5f, 0.0f);
const LineSegment2 LineSegment2::UNIT_CENTERED_VERTICAL(0.0f, -0.5f, 0.0f, 0.5f);

LineSegment2::LineSegment2()
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(float startX, float startY, float endX, float endY) :
start(startX, startY),
end(endX, endY)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& start_position, const Vector2& end_position) :
start(start_position),
end(end_position)
{
/* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& start_position, const Vector2& normalized_direction, float length) :
start(start_position),
end(start_position + normalized_direction * length)
{
    /* DO NOTHING */
}

LineSegment2::LineSegment2(const Vector2& start_position, float angleDegrees, float length) :
start(0.0f, 0.0f),
end(0.0f, 0.0f)
{
    end.SetXY(length * MathUtils::CosDegrees(angleDegrees), length * MathUtils::SinDegrees(angleDegrees));
    start += start_position;
    end += start_position;
}

LineSegment2::LineSegment2(const LineSegment2& copy) :
start(copy.start),
end(copy.end)
{
    /* DO NOTHING */
}

LineSegment2::~LineSegment2()
{
    /* DO NOTHING */
}

void LineSegment2::SetLengthFromStart(float length) {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();

    end = start + Vector2(length * MathUtils::CosDegrees(angleDegrees), length * MathUtils::SinDegrees(angleDegrees));
}

void LineSegment2::SetLengthFromCenter(float length) {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();
    float half_length = length * 0.5f;

    Vector2 center = CalcCenter();
    Vector2 half_extent(half_length * MathUtils::CosDegrees(angleDegrees), half_length * MathUtils::SinDegrees(angleDegrees));

    start = center - half_extent;
    end = center + half_extent;
}

void LineSegment2::SetLengthFromEnd(float length) {
    float angleDegrees = CalcDisplacement().CalcHeadingDegrees();

    start = end - Vector2(length * MathUtils::CosDegrees(angleDegrees), length * MathUtils::SinDegrees(angleDegrees));
}

Vector2 LineSegment2::CalcCenter() const {
    Vector2 displacement = CalcDisplacement();
    return start + displacement * 0.5f;
}

float LineSegment2::CalcLength() const {
    return (end - start).CalcLength();
}

float LineSegment2::CalcLengthSquared() const {
    return (end - start).CalcLengthSquared();
}

void LineSegment2::SetDirectionFromStart(float angleDegrees) {
    Vector2 t = start;
    Translate(-t);
    SetAngle(angleDegrees);
    Translate(t);
}

void LineSegment2::SetDirectionFromEnd(float angleDegrees) {
    Vector2 t = end;
    Translate(-t);
    SetAngle(angleDegrees);
    Translate(t);
}

void LineSegment2::SetDirectionFromCenter(float angleDegrees)
{
    Vector2 center = start + (end - start) * 0.5f;

    Translate(-center);
    SetAngle(angleDegrees);
    Translate(center);
}

void LineSegment2::SetStartEndPositions(const Vector2& start_position, const Vector2& end_position) {
    start = start_position;
    end = end_position;
}

void LineSegment2::Translate(const Vector2& translation) {
    start += translation;
    end += translation;
}

void LineSegment2::Rotate(float angleDegrees) {
    float heading = (end - start).CalcHeadingDegrees();
    SetAngle(heading + angleDegrees);
}

void LineSegment2::RotateStartPosition(float angleDegrees) {
    Vector2 t = end;
    Translate(-t);
    Rotate(angleDegrees);
    Translate(t);
}

void LineSegment2::RotateEndPosition(float angleDegrees) {
    Vector2 t = start;
    Translate(-t);
    Rotate(angleDegrees);
    Translate(t);
}

void LineSegment2::Rotate90Degrees() {
    Rotate(90.0f);
}

void LineSegment2::RotateNegative90Degrees() {
    Rotate(-90.0f);
}

void LineSegment2::Rotate180Degrees() {
    Vector2 temp = end;
    end = start;
    start = temp;
}

Vector2 LineSegment2::CalcDisplacement() const {
    return end - start;
}

Vector2 LineSegment2::CalcDirection() const {
    Vector2 dir = end - start;
    dir.Normalize();
    return dir;
}

Vector2 LineSegment2::CalcPositiveNormal() const {
    Vector2 dir = CalcDirection();
    dir.Rotate90Degrees();
    return dir;
}

Vector2 LineSegment2::CalcNegativeNormal() const {
    Vector2 dir = CalcDirection();
    dir.RotateNegative90Degrees();
    return dir;
}

const LineSegment2 LineSegment2::operator+(const Vector2& translation) const {
    return LineSegment2(start + translation, end + translation);
}

const LineSegment2 LineSegment2::operator-(const Vector2& antiTranslation) const {
    return LineSegment2(start - antiTranslation, end - antiTranslation);
}

void LineSegment2::operator-=(const Vector2& antiTranslation) {
    start -= antiTranslation;
    end -= antiTranslation;
}

bool LineSegment2::IsPointOn(const Vector2& p) const {
    return MathUtils::IsEquivalent(CalcDistanceSquared(p, *this), 0.0f);
}

float CalcDistance(const Vector2& p, const LineSegment2& line) {
    return std::sqrt(CalcDistanceSquared(p, line));
}

float CalcDistanceSquared(const Vector2& p, const LineSegment2& line) {
    return CalcDistanceSquared(p, CalcClosestPoint(p, line));
}

Vector2 CalcClosestPoint(const Vector2& p, const LineSegment2& line) {
    //Voronoi Regions
    Vector2 D = line.end - line.start;
    Vector2 T = D.GetNormalize();
    Vector2 SP = p - line.start;
    float regionI = MathUtils::DotProduct(T, SP);
    if(regionI < 0.0f) {
        return line.start;
    }

    Vector2 EP = p - line.end;
    float regionII = MathUtils::DotProduct(T, EP);
    if(regionII > 0.0f) {
        return line.end;
    }

    Vector2 directionSE = D.GetNormalize();
    float lengthToClosestPoint = MathUtils::DotProduct(directionSE, SP);
    Vector2 C = directionSE * lengthToClosestPoint;
    Vector2 ConL = line.start + C;
    return ConL;
}
void LineSegment2::operator+=(const Vector2& translation) {
    start += translation;
    end += translation;
}

void LineSegment2::SetAngle(float angleDegrees) {
    //Algebraic Rotation.
    //TODO: IMPLEMENT MATRICIES!!
    float oldX = start.x;
    float oldY = start.y;

    float c = MathUtils::CosDegrees(angleDegrees);
    float s = MathUtils::SinDegrees(angleDegrees);

    start.x = oldX * c - oldY * s;
    start.y = oldX * s + oldY * c;

    oldX = end.x;
    oldY = end.y;
    end.x = oldX * c - oldY * s;
    end.y = oldX * s + oldY * c;
}

LineSegment2 Interpolate(const LineSegment2& a, const LineSegment2& b, float fraction) {
    Vector2 start(Interpolate(a.start, b.start, fraction));
    Vector2 end(Interpolate(a.end, b.end, fraction));

    return LineSegment2(start, end);
}