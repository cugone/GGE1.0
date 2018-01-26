#include "Engine/UI/Menu.hpp"

#include "Engine/UI/Canvas.hpp"

namespace UI {

Menu::Menu(UI::Canvas* parentCanvas)
    : Element(parentCanvas)
{
    /* DO NOTHING */
}

void Menu::Update(float deltaSeconds, const Vector2& mouse_position) {
    if(_enabled) {
        Element::Update(deltaSeconds, mouse_position);
        UpdateChildren(deltaSeconds, mouse_position);
    }
}

void Menu::DebugRender(SimpleRenderer* renderer) const {
    UI::Element::DebugRender(renderer);
    DebugRenderChildren(renderer);
}

void Menu::Render(SimpleRenderer* renderer) const {
    RenderChildren(renderer);
}

} //End UI