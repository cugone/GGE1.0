#pragma once

#include <functional>
#include <utility>
#include <vector>

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/UI/Types.hpp"

class SimpleRenderer;
class KerningFont;
class InputSystem;

namespace UI {

class Canvas;
class EventOnEnter;
class EventOnLeave;
class EventOnHover;
class EventOnMouseDown;
class EventOnMouseUp;
class EventOnMouseClick;
class EventOnMouseDblClick;
class EventOnStartDrag;
class EventOnEndDrag;
struct EventData;

class Element {
public:
    Element();
    Element(UI::Canvas* parentCanvas);
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

    template<typename T>
    T* CreateChild(UI::Canvas* parentCanvas);

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

    virtual void Update(float deltaSeconds, const Vector2& canvas_position);

    Vector2 GetRootCanvasPosition();
    virtual void DebugRender(SimpleRenderer* renderer) const;
    virtual void Render(SimpleRenderer* renderer) const;

    void SetPositionMode(const UI::POSITION_MODE& mode);

    void SetDebugColor(const Rgba& edges, const Rgba& fill);
    std::pair<const Rgba&, const Rgba&> GetDebugColor();
    const Rgba& GetDebugEdgeColor();
    const Rgba& GetDebugFillColor();

    bool IsChild() const;
    bool IsParent() const;

    virtual void SetFont(KerningFont* f);
    KerningFont* GetFont() const;

    void SetOnEnterCallback(const std::function<void(EventData& event)>& callback);
    void SetOnLeaveCallback(const std::function<void(EventData& event)>& callback);
    void SetOnHoverCallback(const std::function<void(EventData& event)>& callback);
	void SetOnMouseDownCallback(const std::function<void(EventData& event)>& callback);
	void SetOnMouseUpCallback(const std::function<void(EventData& event)>& callback);
	void SetOnMouseClickCallback(const std::function<void(EventData& event)>& callback);
	void SetOnMouseDblClickCallback(const std::function<void(EventData& event)>& callback);
	void SetOnStartDragCallback(const std::function<void(EventData& event)>& callback);
	void SetOnEndDragCallback(const std::function<void(EventData& event)>& callback);


    void ToggleEnable();
    void SetEnable(bool new_enable);
    bool IsEnabled() const;

    AABB2 CalcLocalBounds() const;

protected:
    void DebugRenderBottomUp(SimpleRenderer* renderer) const;
    void DebugRenderTopDown(SimpleRenderer* renderer) const;
    void DebugRenderChildren(SimpleRenderer* renderer) const;
    void DebugRenderBoundsAndPivot(SimpleRenderer* renderer) const;

    void UpdateChildren(float deltaSeconds, const Vector2& mouse_position);
    void RenderChildren(SimpleRenderer* renderer) const;

    AABB2 GetUVforFill(AABB2 texture_size, AABB2 container, Vector2 align = Vector2::ZERO);
    void DestroyChild(UI::Element*& child);
    void DestroyAllChildren();
    void CalcBounds();
    AABB2 CalcBoundsRelativeToParent() const;

    AABB2 CalcRelativeBounds();

    AABB2 CalcAbsoluteBounds();
    AABB2 CalcAbsoluteBoundsBounds();
    bool IsDirty() const;

    UI::Canvas* GetParentCanvas() const;
    void SetParentCanvas(UI::Canvas* parentCanvas);

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
    Vector2 _last_canvas_position = Vector2(0.0f, 0.0f);
    Vector2 _current_canvas_position = Vector2(0.0f, 0.0f);
    bool _enabled = true;
    bool _wasMouseInElement = false;
	bool _focused = false;
	bool _hovered = false;
	bool _clicked = false;
    bool _isDragging = false;
	bool _wasMousePressedInElement = false;
	bool _wasMouseReleasedInElement = false;
	bool _isBeingDragged = false;
    UI::EventOnEnter* _onEnter = nullptr;
    UI::EventOnLeave* _onLeave = nullptr;
    UI::EventOnHover* _onHover = nullptr;
    UI::EventOnMouseDown* _onMouseDown = nullptr;
	UI::EventOnMouseUp* _onMouseUp = nullptr;
	UI::EventOnMouseClick* _onMouseClick = nullptr;
	UI::EventOnMouseDblClick* _onMouseDblClick = nullptr;
	UI::EventOnStartDrag* _onStartDrag = nullptr;
	UI::EventOnEndDrag* _onEndDrag = nullptr;
    UI::Canvas* _parentCanvas = nullptr;
	UI::Event* _lastEvent = nullptr;
private:

	void HandleMouseClicks(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave);

	void HandleRightMouseButton(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave);
	void HandleLeftMouseButton(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave);
	void HandleMiddleMouseButton(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave);
	void HandleMouseMovement(bool& didMouseMove, bool& didMouseJustEnter, bool& didMouseHover, bool& didMouseJustLeave);
	void HandleKeyboard(bool& isMouseInElement, InputSystem* input);
};

template<typename T>
T* UI::Element::CreateChild() {
    return (T*)AddChild(new T{});
}

template<typename T>
T* UI::Element::CreateChild(UI::Canvas* parentCanvas) {
    return (T*)AddChild(new T{parentCanvas});
}

}