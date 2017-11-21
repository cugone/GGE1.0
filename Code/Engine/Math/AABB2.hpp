#pragma once

#include "Engine/Math/Vector2.hpp"

class Vector4;

class AABB2 {
public:
    Vector2 mins = Vector2::ZERO;
    Vector2 maxs = Vector2::ZERO;

    static const AABB2 ZERO_TO_ONE;
    static const AABB2 NEG_ONE_TO_ONE;

    AABB2() = default;
    AABB2(const AABB2& other) = default;
    AABB2(AABB2&& r_other) = default;
    AABB2& operator=(const AABB2& r_rhs) = default;
    AABB2& operator=(AABB2&& r_rhs) = default;
    inline ~AABB2() noexcept;
    AABB2(float initialX, float initialY) noexcept;
    AABB2(float minX, float minY, float maxX, float maxY) noexcept;
    AABB2(const Vector2& mins, const Vector2& maxs) noexcept;
    AABB2(const Vector2& center, float radiusX, float radiusY) noexcept;
    
    void StretchToIncludePoint(const Vector2& point) noexcept;
    void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius) noexcept;
    void AddPaddingToSidesClamped(float xPaddingRadius, float yPaddingRadius) noexcept;
    void Translate(const Vector2& translation) noexcept;

    bool IsPointInside(const Vector2& point) const noexcept;
    const Vector2 CalcDimensions() const noexcept;
    const Vector2 CalcCenter() const noexcept;

    AABB2 GetBounds(const AABB2& parent, const Vector4& anchors, const Vector4& offsets) const noexcept;
    Vector2 GetSmallestOffset(AABB2 a, AABB2 b) const noexcept;
    AABB2 MoveToBestFit(const AABB2& obj, const AABB2& container) const noexcept;
    Vector2 GetNormalizedPoint(const Vector2& pos) const noexcept;
    Vector2 GetPoint(const Vector2& uv) const noexcept;

    float GetAspectRatio() const noexcept;
    float GetInvAspectRatio() const noexcept;

    float GetWidth() const noexcept;
    float GetHeight() const noexcept;

    Vector2 GetTopLeft() const noexcept;
    Vector2 GetTopRight() const noexcept;
    Vector2 GetBottomLeft() const noexcept;
    Vector2 GetBottomRight() const noexcept;

    const AABB2 operator+(const Vector2& translation) const noexcept;
    const AABB2 operator-(const Vector2& antiTranslation) const noexcept;
    void operator+=(const Vector2& translation) noexcept;
    void operator-=(const Vector2& antiTranslation) noexcept;

    friend Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb);
protected:
private:
};

AABB2 AlignBoundsToContainer(AABB2 bounds, AABB2 container, const Vector2& alignment) noexcept;