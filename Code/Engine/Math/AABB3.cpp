#include "Engine/Math/AABB3.hpp"

#include "Engine/Math/MathUtils.hpp"

const AABB3 AABB3::ZERO_TO_ONE(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

const AABB3 AABB3::NEG_ONE_TO_ONE(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

AABB3::AABB3() {
    /* DO NOTHING */
}

AABB3::~AABB3() {
    /* DO NOTHING */
}

AABB3::AABB3(const AABB3& copy)
    : mins(copy.mins)
    , maxs(copy.maxs)
{
    /* DO NOTHING */
}

AABB3::AABB3(float initialX, float initialY, float initialZ)
    : mins(initialX, initialY, initialZ)
    , maxs(initialX, initialY, initialZ)
{
    /* DO NOTHING */
}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
    : mins(minX, minY, minZ)
    , maxs(maxX, maxY, maxZ)
{
    /* DO NOTHING */
}

AABB3::AABB3(const Vector3& mins, const Vector3& maxs)
    : mins(mins)
    , maxs(maxs)
{
    /* DO NOTHING */
}

AABB3::AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ)
    : mins(center.x - radiusX, center.y - radiusY, center.z - radiusZ)
    , maxs(center.x + radiusX, center.y + radiusY, center.z + radiusZ)
{
    /* DO NOTHING */
}

void AABB3::StretchToIncludePoint(const Vector3& point) {
    if(mins.x > point.x) {
        mins.x = point.x;
    }
    if(mins.y > point.y) {
        mins.y = point.y;
    }
    if(mins.z > point.z) {
        mins.z = point.z;
    }
    if(maxs.x < point.x) {
        maxs.x = point.x;
    }
    if(maxs.y < point.y) {
        maxs.y = point.y;
    }
    if(maxs.z < point.z) {
        maxs.z = point.z;
    }

}

void AABB3::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius) {
    mins.x -= xPaddingRadius;
    mins.y -= yPaddingRadius;
    mins.z -= zPaddingRadius;

    maxs.x += xPaddingRadius;
    maxs.y += yPaddingRadius;
    maxs.z += zPaddingRadius;
}

void AABB3::Translate(const Vector3& translation) {
    mins += translation;
    maxs += translation;
}

bool AABB3::IsPointInside(const Vector3& point) const {
    if(maxs.x < point.x) return false;
    if(mins.x > point.x) return false;

    if(maxs.y < point.y) return false;
    if(mins.y > point.y) return false;

    if(maxs.z < point.z) return false;
    if(mins.z > point.z) return false;

    return true;

}

const Vector3 AABB3::CalcDimensions() const {
    return Vector3(maxs.x - mins.x, maxs.y - mins.y, maxs.z - mins.z);
}

const Vector3 AABB3::CalcCenter() const {
    return Vector3(mins.x + (maxs.x - mins.x) * 0.50f, mins.y + (maxs.y - mins.y) * 0.50f, mins.z + (maxs.z - mins.z) * 0.50f);
}

const AABB3 AABB3::operator+(const Vector3& translation) const {
    return AABB3(mins.x + translation.x, mins.y + translation.y, mins.z + translation.z, maxs.x + translation.x, maxs.y + translation.y, maxs.z + translation.z);
}

const AABB3 AABB3::operator-(const Vector3& antiTranslation) const {
    return AABB3(mins.x - antiTranslation.x, mins.y - antiTranslation.y, mins.z - antiTranslation.z, maxs.x - antiTranslation.x, maxs.y - antiTranslation.y, maxs.z - antiTranslation.z);
}

void AABB3::operator-=(const Vector3& antiTranslation) {
    mins -= antiTranslation;
    maxs -= antiTranslation;
}

Vector3 CalcClosestPoint(const Vector3& p, const AABB3& aabb) {
    float nearestX = MathUtils::Clamp(p.x, aabb.mins.x, aabb.maxs.x);
    float nearestY = MathUtils::Clamp(p.y, aabb.mins.y, aabb.maxs.y);
    float nearestZ = MathUtils::Clamp(p.z, aabb.mins.z, aabb.maxs.z);

    return Vector3(nearestX, nearestY, nearestZ);
}

void AABB3::operator+=(const Vector3& translation) {
    mins += translation;
    maxs += translation;
}

AABB3 Interpolate(const AABB3& a, const AABB3& b, float fraction) {
    Vector3 mins(Interpolate(a.mins, b.mins, fraction));
    Vector3 maxs(Interpolate(a.maxs, b.maxs, fraction));

    return AABB3(mins, maxs);
}