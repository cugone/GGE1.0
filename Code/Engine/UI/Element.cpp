#include "Engine/UI/Element.hpp"

#include <algorithm>
#include <sstream>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/UI.hpp"

namespace UI {

Element::Element() {
    _onEnter = new EventOnEnter();
    _onLeave = new EventOnLeave();
    _onHover = new EventOnHover();
	_onMouseDown = new EventOnMouseDown();
	_onMouseUp = new EventOnMouseUp();
	_onMouseClick = new EventOnMouseClick();
	_onMouseDblClick = new EventOnMouseDblClick();
	_onStartDrag = new EventOnStartDrag();
	_onEndDrag = new EventOnEndDrag();

}

Element::Element(UI::Canvas* parentCanvas) {
    _parentCanvas = parentCanvas;
    _onEnter = new EventOnEnter();
    _onLeave = new EventOnLeave();
    _onHover = new EventOnHover();
	_onMouseDown = new EventOnMouseDown();
	_onMouseUp = new EventOnMouseUp();
	_onMouseClick = new EventOnMouseClick();
	_onMouseDblClick = new EventOnMouseDblClick();
	_onStartDrag = new EventOnStartDrag();
	_onEndDrag = new EventOnEndDrag();
}

Element::~Element() {

	delete _onEndDrag;
	_onEndDrag = nullptr;

	delete _onStartDrag;
	_onStartDrag = nullptr;

	delete _onMouseDblClick;
	_onMouseDblClick = nullptr;

	delete _onMouseClick;
	_onMouseClick = nullptr;

	delete _onMouseUp;
	_onMouseUp = nullptr;

	delete _onMouseDown;
	_onMouseDown = nullptr;

    delete _onHover;
    _onHover = nullptr;

    delete _onLeave;
    _onLeave = nullptr;

    delete _onEnter;
    _onEnter = nullptr;

    RemoveSelf();
    DestroyAllChildren();
}

Vector2 Element::CalcRelativePosition(const Vector2& pos) const {
    AABB2 parent_bounds = GetParentLocalBounds();
    return parent_bounds.GetPoint(pos);
}

Vector2 Element::CalcRelativePosition() const {
    AABB2 parent_bounds = GetParentLocalBounds();
    return parent_bounds.GetPoint(_pivot);
}

const Vector2& Element::GetPosition() const {
    return _position.unit;
}

const Vector2& Element::GetPositionRatio() const {
    return _position.ratio;
}

void Element::SetPosition(const Vector2& ratio, const Vector2& unit) {
    SetPosition({ ratio, unit });
}

void Element::SetPosition(const Metric& position) {
    _dirtyBounds = true;
    _position = position;
    CalcBoundsForMeThenMyChildren();
}

Vector2 Element::GetSize() const {
    return _parent ? (_parent->GetSize() * _size.ratio + _size.unit) : _size.unit;
}

const Vector2& Element::GetSizeRatio() const {
    return _size.ratio;
}

void Element::SetSize(const Vector2& ratio, const Vector2& unit) {
    SetSize({ ratio, unit });
}

void Element::SetSize(const UI::Metric& size) {
    _dirtyBounds = true;
    _size.ratio = size.ratio;
    _size.unit = size.unit;
    CalcBoundsForMeThenMyChildren();
}

void Element::SetSizeUnit(const Vector2& unit) {
    _size.unit = unit;
    SetSize(_size);
}

void Element::SetSizeRatio(const Vector2& ratio) {
    _size.ratio = ratio;
    SetSize(_size);
}

const Vector2& Element::GetPivot() const {
    return _pivot;
}

void Element::SetPivot(const Vector2& ratioOfBounds) {
    _dirtyBounds = true;
    _pivot = ratioOfBounds;
    CalcBoundsForMeThenMyChildren();
}

Element* Element::AddChild(Element* child) {
    _dirtyBounds = true;
    child->_parent = this;
    _children.push_back(child);
    CalcBoundsForMeThenMyChildren();
    return child;
}

void Element::CalcBoundsForMeThenMyChildren() {
    CalcBounds();
    CalcBoundsForChildren();
}

void Element::CalcBoundsForChildren() {
    for(auto& c : _children) {
        if(c) {
            c->CalcBounds();
        }
    }
}

void Element::RemoveChild(Element* child) {
    _dirtyBounds = true;
    _children.erase(std::remove_if(_children.begin(), _children.end(), [&child](UI::Element* c) { return child == c; }), _children.end());
    CalcBoundsForMeThenMyChildren();
}

void Element::RemoveAllChildren() {
    _dirtyBounds = true;
    _children.clear();
    _children.shrink_to_fit();
    CalcBounds();
}

void Element::RemoveSelf() {
    if(_parent) {
        _parent->RemoveChild(this);
        _parent = nullptr;
    }
}

Vector2 Element::GetLocalPosition() const {
    AABB2 local_bounds = GetParentBounds();
    return local_bounds.GetPoint(_position.ratio) + _position.unit;
}

AABB2 Element::GetParentBounds() const {
    return _parent ? _parent->_bounds : AABB2{ 0.0f, 0.0f, _size.unit.x, _size.unit.y };
}

AABB2 Element::GetUVforFill(AABB2 texture_size, AABB2 container, Vector2 align /*= Vector2::ZERO*/) {
    float texture_aspect_ratio = texture_size.GetAspectRatio();
    float container_aspect_ratio = container.GetAspectRatio();
    if(texture_aspect_ratio > container_aspect_ratio) {
        float u = container_aspect_ratio / texture_aspect_ratio;
        //Vector2 offset = -u * align;
        auto offset = -u * align;
        return AABB2(Vector2(offset.x, 0.0f), Vector2(offset.y, 1.0f));
    } else {
        float v = texture_aspect_ratio / container_aspect_ratio;
        //Vector2 offset = (1.0f - v) * align;
        auto offset = (1.0f - v) * align;
        return AABB2(Vector2(0.0f, offset.x), Vector2(1.0f, v - offset.y));
    }
}

void Element::DestroyChild(UI::Element*& child) {
    auto iter = std::find_if(std::begin(_children), std::end(_children), [child](UI::Element* c) { return child == c; });
    if(iter != std::end(_children)) {
        delete *iter;
        *iter = nullptr;
    }
}

void Element::DestroyAllChildren() {
    for(auto& iter : _children) {
        iter->_parent = nullptr;
        delete iter;
        iter = nullptr;
    }
}

void Element::CalcBounds() {
    
    switch(_positioning_mode) {
        case UI::POSITION_MODE::UI_POSITION_ABSOLUTE:
            _bounds = CalcAbsoluteBounds();
            break;
        case UI::POSITION_MODE::UI_POSITION_RELATIVE:
            _bounds = CalcRelativeBounds();
            break;
        default:
            ERROR_AND_DIE("Element::CalcBounds: Unhandled positioning mode.");
    }

}

AABB2 Element::CalcBoundsRelativeToParent() const {
    Vector2 my_size = GetSize();

    AABB2 parent_bounds = _parent ? _parent->CalcLocalBounds() : CalcLocalBounds();
    Vector2 parent_size = parent_bounds.CalcDimensions();

    Vector2 pivot_position = parent_bounds.mins + (parent_size * _position.ratio + _position.unit);

    AABB2 my_local_bounds = CalcLocalBounds();
    my_local_bounds.Translate(pivot_position);

    return my_local_bounds;
}

AABB2 Element::CalcLocalBounds() const {
    return {Vector2::ZERO, GetSize()};
}

AABB2 Element::CalcRelativeBounds() {
    Vector2 size = GetSize();
    Vector2 pivot_position = size * _pivot;

    AABB2 bounds;
    bounds.StretchToIncludePoint(Vector2::ZERO);
    bounds.StretchToIncludePoint(size);
    bounds.Translate(-pivot_position);
    return bounds;
}

AABB2 Element::CalcAbsoluteBounds() {

    Vector2 size = GetSize();

    Vector2 mins{ { -(size.x * _pivot.x)}, {-(size.y * _pivot.y)} };
    Vector2 maxs{ {  size.x * (1.0f - _pivot.x) }, { size.y * (1.0f - _pivot.y) } };
    AABB2 result(mins, maxs);
    return result;

}

AABB2 Element::CalcAbsoluteBoundsBounds() {
    AABB2 parent_bounds = GetParentLocalBounds();
    AABB2 alignedBounds = AlignBoundsToContainer(CalcBoundsRelativeToParent(), parent_bounds, _position.ratio);

    Vector2 offset(MathUtils::RangeMap(_position.ratio.x, 0.0f, 1.0f, 1.0f, -1.0f) * _position.unit.x, MathUtils::RangeMap(_position.ratio.y, 0.0f, 1.0f, 1.0f, -1.0f) * _position.unit.y);
    
    alignedBounds.Translate(offset);

    return alignedBounds;
}

bool Element::IsDirty() const {
    return _dirtyBounds;
}

void Element::SetParentCanvas(UI::Canvas* parentCanvas) {
    _parentCanvas = parentCanvas;
}

UI::Canvas* Element::GetParentCanvas() const {
    return _parentCanvas;
}

AABB2 Element::GetParentLocalBounds() const {
    return _parent ? _parent->CalcLocalBounds() : AABB2{ 0.0f, 0.0f, _size.unit.x, _size.unit.y };
}

AABB2 Element::GetParentRelativeBounds() {
    return _parent ? _parent->CalcBoundsRelativeToParent() : AABB2{ 0.0f, 0.0f, 0.0f, 0.0f };
}

Matrix4 Element::GetParentWorldTransform() const {
    return _parent ? _parent->GetWorldTransform() : Matrix4::GetIdentity();
}

Matrix4 Element::GetLocalTransform() const {
    return Matrix4::CreateTranslationMatrix(GetLocalPosition());
}

Matrix4 Element::GetWorldTransform() const {
    return GetParentWorldTransform() * GetLocalTransform();
}

void Element::Update(float /*deltaSeconds*/, const Vector2& canvas_position) {

    _current_canvas_position = canvas_position;

    auto inv_M = Matrix4::CalculateInverse(GetWorldTransform());
    auto cur_nh_screen_to_world = inv_M * Vector4(_current_canvas_position, 0.0f, 1.0f);
    auto cur_h_screen_to_world = Vector4::CalcHomogeneous(cur_nh_screen_to_world);
    auto cur_screen_to_world = Vector2(cur_h_screen_to_world.x, cur_h_screen_to_world.y);

    bool isMouseInElement = _bounds.IsPointInside(cur_screen_to_world);

    bool didMouseJustEnter = !_wasMouseInElement && isMouseInElement;
    bool didMouseHover = _wasMouseInElement && isMouseInElement;
    bool didMouseJustLeave = _wasMouseInElement && !isMouseInElement;
    bool didMouseMove = _last_canvas_position != _current_canvas_position;
	auto pCanvas = _parentCanvas;
	InputSystem* input = nullptr;
	UI::UiSystem* ui = nullptr;
	if (pCanvas) {
		input = pCanvas->GetInput();
		ui = pCanvas->GetUISystem();
	}
	bool hasInput = pCanvas && input && ui;
	if (hasInput) {
		HandleMouseClicks(isMouseInElement, input, didMouseJustLeave);
		HandleMouseMovement(didMouseMove, didMouseJustEnter, didMouseHover, didMouseJustLeave);
		HandleKeyboard(isMouseInElement, input);
	}
    _wasMouseInElement = isMouseInElement;
    _last_canvas_position = _current_canvas_position;
}

Vector2 Element::GetRootCanvasPosition() {
    return _parentCanvas ? _parentCanvas->GetLocalPosition() : Vector2::ZERO;
}

void Element::DebugRenderBottomUp(SimpleRenderer* renderer) const {
    DebugRenderChildren(renderer);
    DebugRenderBoundsAndPivot(renderer);
}

void Element::DebugRenderTopDown(SimpleRenderer* renderer) const {
    DebugRenderBoundsAndPivot(renderer);
    DebugRenderChildren(renderer);
}

void Element::DebugRenderChildren(SimpleRenderer* renderer) const {
    for(const auto& c : _children) {
        if(c) {
            c->DebugRender(renderer);
        }
    }
}

void Element::DebugRenderBoundsAndPivot(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
    renderer->DrawDebugAABB2(_bounds, _debugEdge, _debugFill);
    renderer->DrawDebugX2D(Vector2::ZERO, 10.0f, 1.0f, Rgba::RED);
}

void Element::UpdateChildren(float deltaSeconds, const Vector2& mouse_position) {
    for(const auto& c : _children) {
        if(c) {
            c->Update(deltaSeconds, mouse_position);
        }
    }
}

void Element::RenderChildren(SimpleRenderer* renderer) const {
    for(const auto& c : _children) {
        if(c) {
            c->Render(renderer);
        }
    }
}

void Element::DebugRender(SimpleRenderer* renderer) const {
    DebugRenderBoundsAndPivot(renderer);
}

void Element::Render(SimpleRenderer* /*renderer*/) const {
    /* DO NOTHING */
}

void Element::SetPositionMode(const UI::POSITION_MODE& mode) {
    _positioning_mode = mode;
    CalcBounds();
}

void Element::SetDebugColor(const Rgba& edges, const Rgba& fill) {
    _debugFill = fill;
    _debugEdge = edges;
}

std::pair<const Rgba&, const Rgba&> Element::GetDebugColor() {
    return std::make_pair(GetDebugEdgeColor(), GetDebugFillColor());
}

const Rgba& Element::GetDebugEdgeColor() {
    return _debugEdge;
}

const Rgba& Element::GetDebugFillColor() {
    return _debugFill;
}
bool Element::IsChild() const {
    return _parent != nullptr;
}

bool Element::IsParent() const {
    return !_children.empty();
}

void Element::SetFont(KerningFont* f) {
    _dirtyBounds = true;
    _font = f;
}

KerningFont* Element::GetFont() const {
    return _font;
}

void Element::SetOnEnterCallback(const std::function<void(EventData& event)>& callback) {
    _onEnter->Subscribe(callback);
}

void Element::SetOnLeaveCallback(const std::function<void(EventData& event)>& callback) {
    _onLeave->Subscribe(callback);
}

void Element::SetOnHoverCallback(const std::function<void(EventData& event)>& callback) {
    _onHover->Subscribe(callback);
}

void Element::SetOnMouseDownCallback(const std::function<void(EventData& event)>& callback) {
	_onMouseDown->Subscribe(callback);
}

void Element::SetOnMouseUpCallback(const std::function<void(EventData& event)>& callback) {
	_onMouseUp->Subscribe(callback);
}

void Element::SetOnMouseClickCallback(const std::function<void(EventData& event)>& callback) {
	_onMouseClick->Subscribe(callback);
}

void Element::SetOnMouseDblClickCallback(const std::function<void(EventData& event)>& callback) {
	_onMouseDblClick->Subscribe(callback);
}

void Element::SetOnStartDragCallback(const std::function<void(EventData& event)>& callback) {
	_onStartDrag->Subscribe(callback);
}

void Element::SetOnEndDragCallback(const std::function<void(EventData& event)>& callback) {
	_onEndDrag->Subscribe(callback);
}

void Element::ToggleEnable() {
    _enabled = !_enabled;
}

void Element::SetEnable(bool new_enable) {
    _enabled = new_enable;
}

bool Element::IsEnabled() const {
    return _enabled;
}

void Element::HandleMouseMovement(bool& didMouseMove, bool& didMouseJustEnter, bool& didMouseHover, bool& didMouseJustLeave) {
	if (didMouseMove && didMouseJustEnter) {
		_onEnter->Trigger(_onEnter->data);
	}
	if (didMouseHover) {
		_onHover->Trigger(_onHover->data);
	}
	if (didMouseMove && didMouseJustLeave) {
		_onLeave->Trigger(_onLeave->data);
	}
    if(_wasMousePressedInElement && didMouseMove) {
        _isDragging = true;
		_onStartDrag->Trigger(_onStartDrag->data);
	}
	if (_isDragging && _wasMouseReleasedInElement) {
        _isDragging = false;
		_onEndDrag->Trigger(_onEndDrag->data);
	}
}

void Element::HandleKeyboard(bool& /*isMouseInElement*/, InputSystem* /*input*/) {
	
}

void Element::HandleMouseClicks(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave) {
	HandleLeftMouseButton(isMouseInElement, input, didMouseJustLeave);
	HandleRightMouseButton(isMouseInElement, input, didMouseJustLeave);
	HandleMiddleMouseButton(isMouseInElement, input, didMouseJustLeave);
}

void Element::HandleLeftMouseButton(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave) {
	if (isMouseInElement && input->WasKeyJustPressed(KeyCode::LBUTTON)) {
		_wasMousePressedInElement = input->IsKeyDown(KeyCode::LBUTTON);
		auto& data = _onMouseDown->data;
		data.button = 0;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::LBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::LBUTTON);
		_onMouseDown->Trigger(data);
	}
	if (input->WasKeyJustReleased(KeyCode::LBUTTON)) {
		if (!didMouseJustLeave && isMouseInElement && _wasMousePressedInElement) {
			_wasMouseReleasedInElement = input->IsKeyUp(KeyCode::LBUTTON);
		}
	}
	bool wasMouseClickedInElement = _wasMousePressedInElement && _wasMouseReleasedInElement;
	if (wasMouseClickedInElement) {
        _wasMouseReleasedInElement = false;
        _wasMousePressedInElement = false;
		auto& data = _onMouseClick->data;
		data.button = 0;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::LBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::LBUTTON);
		_onMouseClick->Trigger(data);
	}
	if (isMouseInElement && input->WasKeyJustReleased(KeyCode::LBUTTON)) {
		auto& data = _onMouseUp->data;
		data.button = 0;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::LBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::LBUTTON);
		_onMouseUp->Trigger(data);
	}

	if (isMouseInElement && input->WasMouseLeftDoubleClicked()) {
		auto& data = _onMouseDblClick->data;
		data.button = 0;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::LBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::LBUTTON);
		_onMouseDblClick->Trigger(data);
	}
}

void Element::HandleRightMouseButton(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave) {
	if (isMouseInElement && input->WasKeyJustPressed(KeyCode::RBUTTON)) {
		_wasMousePressedInElement = input->IsKeyDown(KeyCode::RBUTTON);
		auto& data = _onMouseDown->data;
		data.button = 1;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::RBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::RBUTTON);
		_onMouseDown->Trigger(data);
	}
    if(input->WasKeyJustReleased(KeyCode::RBUTTON)) {
        if(!didMouseJustLeave && isMouseInElement && _wasMousePressedInElement) {
            _wasMouseReleasedInElement = input->IsKeyUp(KeyCode::RBUTTON);
        }
    }
    bool wasMouseClickedInElement = _wasMousePressedInElement && _wasMouseReleasedInElement;
    if(wasMouseClickedInElement) {
        _wasMouseReleasedInElement = false;
        _wasMousePressedInElement = false;
        auto& data = _onMouseClick->data;
        data.button = 0;
        data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::RBUTTON);
        data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::RBUTTON);
        _onMouseClick->Trigger(data);
    }
	if (isMouseInElement && input->WasKeyJustReleased(KeyCode::RBUTTON)) {
		auto& data = _onMouseUp->data;
		data.button = 1;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::RBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::RBUTTON);
		_onMouseUp->Trigger(data);
	}

	if (isMouseInElement && input->WasMouseRightDoubleClicked()) {
		auto& data = _onMouseDblClick->data;
		data.button = 1;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::RBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::RBUTTON);
		_onMouseDblClick->Trigger(data);
	}
}

void Element::HandleMiddleMouseButton(bool& isMouseInElement, InputSystem* input, bool& didMouseJustLeave) {
	if (isMouseInElement && input->WasKeyJustPressed(KeyCode::MBUTTON)) {
		_wasMousePressedInElement = input->IsKeyDown(KeyCode::MBUTTON);
		auto& data = _onMouseDown->data;
		data.button = 2;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::MBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::MBUTTON);
		_onMouseDown->Trigger(data);
	}

	if (!didMouseJustLeave && isMouseInElement && _wasMousePressedInElement && input->WasKeyJustReleased(KeyCode::MBUTTON)) {
		_wasMousePressedInElement = false;
		auto& data = _onMouseClick->data;
		data.button = 2;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::MBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::MBUTTON);
		_onMouseClick->Trigger(data);
	}

	if (isMouseInElement && input->WasKeyJustReleased(KeyCode::MBUTTON)) {
		auto& data = _onMouseUp->data;
		data.button = 2;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::MBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::MBUTTON);
		_onMouseUp->Trigger(data);
	}

	if (isMouseInElement && input->WasMouseMiddleDoubleClicked()) {
		auto& data = _onMouseDblClick->data;
		data.button = 2;
		data.keycode = _parentCanvas->GetInput()->ConvertKeyCodeToScanCode(KeyCode::MBUTTON);
		data.key = static_cast<std::underlying_type_t<KeyCode>>(KeyCode::MBUTTON);
		_onMouseDblClick->Trigger(data);
	}
}

} //End UI