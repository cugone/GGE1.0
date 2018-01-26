#include "Engine/UI/Button.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

namespace UI {

Button::Button()
: UI::Element()
{
    /* DO NOTHING */
}

Button::Button(UI::Canvas* parentCanvas)
: UI::Element(parentCanvas)
{
    /* DO NOTHING */
}

void Button::DebugRender(SimpleRenderer* renderer) const {
    UI::Element::DebugRender(renderer);
    if(_fillColor.a > 0 || _edgeColor.a > 0) {
        renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
        renderer->DrawDebugAABB2(_bounds, _edgeColor, _fillColor);
    }
    DebugRenderChildren(renderer);
}

void Button::Render(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    if(_fillColor.a > 0 || _edgeColor.a > 0) {
        renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
        renderer->DrawDebugAABB2(_bounds, _edgeColor, _fillColor);
    }
    RenderChildren(renderer);
}

void Button::Update(float deltaSeconds, const Vector2& mouse_position) {
    if(_enabled) {
        Element::Update(deltaSeconds, mouse_position);
        UpdateChildren(deltaSeconds, mouse_position);
    }
}

void Button::SetEdgeColor(const Rgba& color) {
    _edgeColor = color;
}

void Button::SetFillColor(const Rgba& color) {
    _fillColor = color;
}

} //End UI