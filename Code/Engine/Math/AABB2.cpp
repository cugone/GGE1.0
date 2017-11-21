#include "Engine/Math/AABB2.hpp"

#include <algorithm>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector4.hpp"

const AABB2 AABB2::ZERO_TO_ONE(0.0f, 0.0f, 1.0f, 1.0f);

const AABB2 AABB2::NEG_ONE_TO_ONE(-1.0f, -1.0f, 1.0f, 1.0f);

AABB2::~AABB2() noexcept {
    /* DO NOTHING */
}

AABB2::AABB2(float initialX, float initialY) noexcept :
mins(initialX, initialY),
maxs(initialX, initialY)
{
    /* DO NOTHING */
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY) noexcept :
mins(minX, minY),
maxs(maxX, maxY)
{
    /* DO NOTHING */
}

AABB2::AABB2(const Vector2& mins, const Vector2& maxs) noexcept :
mins(mins),
maxs(maxs)
{
    /* DO NOTHING */
}

AABB2::AABB2(const Vector2& center, float radiusX, float radiusY) noexcept :
mins(center.x - radiusX, center.y - radiusY),
maxs(center.x + radiusX, center.y + radiusY)
{
    /* DO NOTHING */
}

void AABB2::StretchToIncludePoint(const Vector2& point) noexcept {
    if(mins.x > point.x) {
        mins.x = point.x;
    }
    if(mins.y > point.y) {
        mins.y = point.y;
    }
    if(maxs.x < point.x) {
        maxs.x = point.x;
    }
    if(maxs.y < point.y) {
        maxs.y = point.y;
    }

}

void AABB2::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius) noexcept {
    mins.x -= xPaddingRadius;
    mins.y -= yPaddingRadius;

    maxs.x += xPaddingRadius;
    maxs.y += yPaddingRadius;
}

void AABB2::AddPaddingToSidesClamped(float xPaddingRadius, float yPaddingRadius) noexcept {
    
    auto width = maxs.x - mins.x;
    auto height = maxs.y - mins.y;
    auto half_width = width * 0.5f;
    auto half_height = height * 0.5f;

    xPaddingRadius = (std::max)(-half_width, xPaddingRadius);
    yPaddingRadius = (std::max)(-half_height, yPaddingRadius);

    mins.x -= xPaddingRadius;
    mins.y -= yPaddingRadius;

    maxs.x += xPaddingRadius;
    maxs.y += yPaddingRadius;

}

void AABB2::Translate(const Vector2& translation) noexcept {
    mins += translation;
    maxs += translation;
}

bool AABB2::IsPointInside(const Vector2& point) const noexcept {
    if(maxs.x < point.x) return false;
    if(mins.x > point.x) return false;

    if(maxs.y < point.y) return false;
    if(mins.y > point.y) return false;

    return true;
}

const Vector2 AABB2::CalcDimensions() const noexcept {
    return Vector2(maxs.x - mins.x, maxs.y - mins.y);
}

const Vector2 AABB2::CalcCenter() const noexcept {
    return Vector2(mins.x + (maxs.x - mins.x) * 0.50f, mins.y + (maxs.y - mins.y) * 0.50f);
}

AABB2 AABB2::GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept {
    Vector2 boundMins = parent.GetPoint(Vector2(anchors.x, anchors.y)) + Vector2(offsets.x, offsets.y);
    Vector2 boundMaxs = parent.GetPoint(Vector2(anchors.z, anchors.w)) + Vector2(offsets.z, offsets.w);
    return AABB2(boundMins, boundMaxs);
}

Vector2 AABB2::GetSmallestOffset(AABB2 a, AABB2 b) const noexcept {
    auto width = a.CalcDimensions().x;
    auto height = a.CalcDimensions().y;
    auto center = a.CalcCenter();
    AABB2 b_prime = b;
    b_prime.AddPaddingToSides(-(width * 0.5f), -(height * 0.5f));
    Vector2 closestPoint = CalcClosestPoint(center, b_prime);
    return closestPoint - center;
}

AABB2 AABB2::MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept {
    Vector2 offset = GetSmallestOffset(obj, container);
    return obj + offset;
}

Vector2 AABB2::GetNormalizedPoint(const Vector2& pos) const noexcept {
    float x_norm = MathUtils::RangeMap(pos.x, mins.x, maxs.x, 0.0f, 1.0f);
    float y_norm = MathUtils::RangeMap(pos.y, mins.y, maxs.y, 0.0f, 1.0f);
    return std::move(Vector2(x_norm, y_norm));
}

Vector2 AABB2::GetPoint(const Vector2& uv) const noexcept {
    float x = MathUtils::RangeMap(uv.x, 0.0f, 1.0f, mins.x, maxs.x);
    float y = MathUtils::RangeMap(uv.y, 0.0f, 1.0f, mins.y, maxs.y);
    return std::move(Vector2(x, y));
}

float AABB2::GetAspectRatio() const noexcept {
    return GetWidth() / GetHeight();
}


float AABB2::GetInvAspectRatio() const noexcept {
    return 1.0f / GetAspectRatio();
}


float AABB2::GetWidth() const noexcept {
    return maxs.x - mins.x;
}

float AABB2::GetHeight() const noexcept {
    return maxs.y - mins.y;
}

Vector2 AABB2::GetTopLeft() const noexcept {
    return mins;
}

Vector2 AABB2::GetTopRight() const noexcept {
    return {maxs.x, mins.y};
}

Vector2 AABB2::GetBottomLeft() const noexcept {
    return {mins.x, maxs.y};
}

Vector2 AABB2::GetBottomRight() const noexcept {
    return {maxs};
}

const AABB2 AABB2::operator+(const Vector2& translation) const noexcept {
    return AABB2(mins.x + translation.x, mins.y + translation.y, maxs.x + translation.x, maxs.y + translation.y);
}

void AABB2::operator+=(const Vector2& translation) noexcept {
    mins += translation;
    maxs += translation;
}

const AABB2 AABB2::operator-(const Vector2& antiTranslation) const noexcept {
    return AABB2(mins.x - antiTranslation.x, mins.y - antiTranslation.y, maxs.x - antiTranslation.x, maxs.y - antiTranslation.y);
}

void AABB2::operator-=(const Vector2& antiTranslation) noexcept {
    mins -= antiTranslation;
    maxs -= antiTranslation;
}

Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb) {
    if(aabb.IsPointInside(p)) {
        return p;
    }
    //Region I
    if(p.x < aabb.mins.x && aabb.maxs.y < p.y) {
        return Vector2(aabb.mins.x, aabb.maxs.y);
    }
    //Region II
    if(p.x < aabb.mins.x && p.y < aabb.mins.y) {
        return Vector2(aabb.mins.x, aabb.mins.y);
    }
    //Region III
    if(aabb.maxs.x < p.x && p.y < aabb.mins.y) {
        return Vector2(aabb.maxs.x, aabb.mins.y);
    }
    //Region IV
    if(aabb.maxs.x < p.x && aabb.maxs.y < p.y) {
        return Vector2(aabb.maxs.x, aabb.maxs.y);
    }
    //Region V
    if(p.x < aabb.mins.x) {
        return Vector2(aabb.mins.x, p.y);
    }
    //Region VI
    if(aabb.maxs.x < p.x) {
        return Vector2(aabb.maxs.x, p.y);
    }
    //Region VII
    if(p.y < aabb.mins.y) {
        return Vector2(p.x, aabb.mins.y);
    }
    //Region VIII
    if(aabb.maxs.y < p.y) {
        return Vector2(p.x, aabb.maxs.y);
    }
    return Vector2::ZERO;
}


AABB2 AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) noexcept {
    Vector2 distance = container.GetPoint(alignment) = bounds.GetPoint(alignment);
    bounds.Translate(distance);
    return bounds;
}
