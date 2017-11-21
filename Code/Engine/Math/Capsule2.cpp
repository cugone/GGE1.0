#include "Engine/Math/Capsule2.hpp"

#include "Engine/Math/MathUtils.hpp"

const Capsule2 Capsule2::UNIT_HORIZONTAL(Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), 1.0f);
const Capsule2 Capsule2::UNIT_VERTICAL(Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), 1.0f);
const Capsule2 Capsule2::UNIT_CENTERED_HORIZONTAL(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f), 1.0f);
const Capsule2 Capsule2::UNIT_CENTERED_VERTICAL(Vector2(0.0f, -0.5f), Vector2(0.0f, 0.5f), 1.0f);


Capsule2::Capsule2()
: line()
, radius(0.0f)
{
    /* DO NOTHING */
}
Capsule2::Capsule2(const LineSegment2& line, float radius)
: line(line)
, radius(radius)
{
    /* DO NOTHING */
}
Capsule2::Capsule2(const Vector2& start_position, const Vector2& end_position, float radius)
: line(start_position, end_position)
, radius(radius)
{
    /* DO NOTHING */
}

Capsule2::Capsule2(const Vector2& start_position, const Vector2& normalized_direction, float length, float radius)
: line(start_position, normalized_direction, length)
, radius(radius)
{
    /* DO NOTHING */
}

Capsule2::Capsule2(const Vector2& start_position, float angleDegrees, float length, float radius)
: line(start_position, angleDegrees, length)
, radius(radius)
{
    /* DO NOTHING */
}

Capsule2::Capsule2(const Capsule2& copy)
: line(copy.line)
, radius(copy.radius)
{
    /* DO NOTHING */
}

Capsule2::~Capsule2() {
    /* DO NOTHING */
}

void Capsule2::SetLengthFromStart(float length) {
    line.SetLengthFromStart(length);
}

void Capsule2::SetLengthFromCenter(float length) {
    line.SetLengthFromCenter(length);
}

void Capsule2::SetLengthFromEnd(float length) {
    line.SetLengthFromEnd(length);
}

Vector2 Capsule2::CalcCenter() const {
    return line.CalcCenter();
}

float Capsule2::CalcLength() const {
    return line.CalcLength();
}

float Capsule2::CalcLengthSquared() const {
    return line.CalcLengthSquared();
}

void Capsule2::SetDirectionFromCenter(float angleDegrees) {
    line.SetDirectionFromCenter(angleDegrees);
}

void Capsule2::SetDirectionFromStart(float angleDegrees) {
    line.SetDirectionFromStart(angleDegrees);
}

void Capsule2::SetDirectionFromEnd(float angleDegrees) {
    line.SetDirectionFromEnd(angleDegrees);
}

void Capsule2::SetStartEndPositions(const Vector2& start_position, const Vector2& end_position) {
    line.SetStartEndPositions(start_position, end_position);
}

void Capsule2::Translate(const Vector2& translation) {
    line.Translate(translation);
}

void Capsule2::Rotate(float angleDegrees) {
    line.Rotate(angleDegrees);
}

void Capsule2::RotateStartPosition(float angleDegrees) {
    line.RotateStartPosition(angleDegrees);
}

void Capsule2::RotateEndPosition(float angleDegrees) {
    line.RotateEndPosition(angleDegrees);
}

void Capsule2::Rotate90Degrees() {
    line.Rotate90Degrees();
}

void Capsule2::RotateNegative90Degrees() {
    line.RotateNegative90Degrees();
}

void Capsule2::Rotate180Degrees() {
    line.Rotate180Degrees();
}

Vector2 Capsule2::CalcDisplacement() const {
    return line.CalcDisplacement();
}

Vector2 Capsule2::CalcDirection() const {
    return line.CalcDirection();
}

Vector2 Capsule2::CalcPositiveNormal() const {
    return line.CalcPositiveNormal();
}

Vector2 Capsule2::CalcNegativeNormal() const {
    return line.CalcNegativeNormal();
}

float Capsule2::GetRadius() const {
    return radius;
}

float Capsule2::GetRadius() {
    return static_cast<const Capsule2&>(*this).GetRadius();
}

const Capsule2 Capsule2::operator+(const Vector2& translation) const {
    return Capsule2(line + translation, radius);
}

const Capsule2 Capsule2::operator-(const Vector2& antiTranslation) const {
    return Capsule2(line - antiTranslation, radius);
}

void Capsule2::operator-=(const Vector2& antiTranslation) {
    line -= antiTranslation;
}

void Capsule2::operator+=(const Vector2& translation) {
    line += translation;
}

void Capsule2::SetAngle(float angleDegrees) {
    line.SetAngle(angleDegrees);
}

Capsule2 Interpolate(const Capsule2& a, const Capsule2& b, float fraction) {
    LineSegment2 line(Interpolate(a.line, b.line, fraction));
    float radius(MathUtils::Interpolate(a.radius, b.radius, fraction));
    return Capsule2(line, radius);
}