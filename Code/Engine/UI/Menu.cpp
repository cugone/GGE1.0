#include "Engine/UI/Menu.hpp"

namespace UI {

void Menu::Update(float deltaSeconds, const IntVector2& mouse_position) {
    if(_enabled) {
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