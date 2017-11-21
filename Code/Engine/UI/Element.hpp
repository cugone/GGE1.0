#pragma once

#include <vector>

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/UI/Types.hpp"

class SimpleRenderer;
class KerningFont;

namespace UI {

class EventOnEnter;

class Element {
public:
    Element();
    virtual ~Element();

    Vector2 CalcRelativePosition() const;
    Vector2 CalcRelativePosition(const Vector2& pos) const;
    const Vector2& GetPosition() const;
    const Vector2& GetPositionRatio() const;
    void SetPosition(const Vector2& ratio, const Vector2& unit);
    void SetPosition(const UI::Metric& position);
    Vector2 GetSize() const;
    const Vector2& GetSizeRatio() const;
    void SetSize(const Vector2& ratio, const Vector2& unit);
    void SetSize(const UI::Metric& size);
    void SetSizeUnit(const Vector2& unit);
    void SetSizeRatio(const Vector2& ratio);

    const Vector2& GetPivot() const;
    void SetPivot(const Vector2& ratio_in_respect_to_my_bounds);

    template<typename T>
    T* CreateChild();
    UI::Element* AddChild(UI::Element* child);

    void CalcBoundsForChildren();
    void CalcBoundsForMeThenMyChildren();

    void RemoveChild(UI::Element* child);
    void RemoveAllChildren();
    void RemoveSelf();

    Vector2 GetLocalPosition() const;

    AABB2 GetParentBounds() const;
    AABB2 GetParentLocalBounds() const;
    AABB2 GetParentRelativeBounds();

    Matrix4 GetParentWorldTransform() const;

    Matrix4 GetLocalTransform() const;
    Matrix4 GetWorldTransform() const;

    virtual void Update(float deltaSeconds, const IntVector2& mouse_position);
    virtual void DebugRender(SimpleRenderer* renderer) const;
    virtual void Render(SimpleRenderer* renderer) const;

    void SetPositionMode(const UI::POSITION_MODE& mode);

    void SetDebugColor(const Rgba& edges, const Rgba& fill);

    bool IsChild() const;
    bool IsParent() const;

    void SetFont(KerningFont* f);
    KerningFont* GetFont() const;

protected:
    void DebugRenderBottomUp(SimpleRenderer* renderer) const;
    void DebugRenderTopDown(SimpleRenderer* renderer) const;
    void DebugRenderChildren(SimpleRenderer* renderer) const;
    void DebugRenderBoundsAndPivot(SimpleRenderer* renderer) const;

    void UpdateChildren(float deltaSeconds, const IntVector2& mouse_position);
    void RenderChildren(SimpleRenderer* renderer) const;

    AABB2 GetUVforFill(AABB2 texture_size, AABB2 container, Vector2 align = Vector2::ZERO);
    void DestroyChild(UI::Element*& child);
    void DestroyAllChildren();
    void CalcBounds();
    AABB2 CalcBoundsRelativeToParent() const;
    AABB2 CalcLocalBounds() const;

    AABB2 CalcRelativeBounds();

    AABB2 CalcAbsoluteBounds();
    AABB2 CalcAbsoluteBoundsBounds();
    bool IsDirty() const;

    UI::Metric _position = UI::Metric{ {0.0f, 0.0f}, {0.0f, 0.0f} };
    Vector2 _pivot = Vector2{ 0.0f, 0.0f };
    UI::Metric _size = UI::Metric{ { 0.0f, 0.0f },{ 0.0f, 0.0f } };
    AABB2 _bounds = AABB2{ { 0.0f, 0.0f },{ 0.0f, 0.0f } };
    UI::Element* _parent = nullptr;
    std::vector<UI::Element*> _children{};
    UI::POSITION_MODE _positioning_mode = UI::POSITION_MODE::UI_POSITION_ABSOLUTE;
    Rgba _debugFill = Rgba::NOALPHA;
    Rgba _debugEdge = Rgba::WHITE;
    bool _dirtyBounds = false;
    KerningFont* _font = nullptr;
    bool _enabled = true;
    static UI::EventOnEnter* _onEnter;

private:

};

template<typename T>
T* UI::Element::CreateChild() {
    return (T*)AddChild(new T{});
}
}