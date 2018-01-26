#include "Engine/UI/Panel.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

namespace UI {

Panel::Panel()
: UI::Element()
{
    /* DO NOTHING */
}

Panel::Panel(UI::Canvas* parentCanvas)
: UI::Element(parentCanvas)
{
    /* DO NOTHING */
}

void Panel::DebugRender(SimpleRenderer* renderer) const {
    UI::Element::DebugRender(renderer);
    if(_fillColor.a > 0 || _edgeColor.a > 0) {
        renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
        renderer->DrawDebugAABB2(_bounds, _edgeColor, _fillColor);
    }
    DebugRenderChildren(renderer);
}

void Panel::Render(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    if(_fillColor.a > 0 || _edgeColor.a > 0) {
        renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
        renderer->DrawDebugAABB2(_bounds, _edgeColor, _fillColor);
    }
    RenderChildren(renderer);
}

void Panel::Update(float deltaSeconds, const Vector2& mouse_position) {
    if(_enabled) {
        Element::Update(deltaSeconds, mouse_position);
        UpdateChildren(deltaSeconds, mouse_position);
    }
}

void Panel::SetEdgeColor(const Rgba& color) {
    _edgeColor = color;
}

void Panel::SetFillColor(const Rgba& color) {
    _fillColor = color;
}

} //End UI