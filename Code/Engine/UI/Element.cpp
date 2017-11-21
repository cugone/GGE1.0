#include "Engine/UI/Element.hpp"

#include <algorithm>
#include <sstream>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Core/KerningFont.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/UI/EventOnEnter.hpp"

namespace UI {

UI::EventOnEnter* Element::_onEnter = nullptr;

Element::Element() {
    _onEnter = new EventOnEnter();
}

Element::~Element() {
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

void Element::Update(float /*deltaSeconds*/, const IntVector2& /*mouse_position*/) {
    /* DO NOTHING */
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

void Element::UpdateChildren(float deltaSeconds, const IntVector2& mouse_position) {
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

bool Element::IsChild() const {
    return _parent != nullptr;
}

bool Element::IsParent() const {
    return !_children.empty();
}

void Element::SetFont(KerningFont* f) {
    _font = f;
}

KerningFont* Element::GetFont() const {
    return _font;
}

} //End UI