#include "Engine/UI/Text.hpp"

#include "Engine/Core/KerningFont.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

namespace UI {

Text::Text(KerningFont* f)
    : UI::Element()
{
    _font = f;
}

void Text::DebugRender(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    renderer->DrawTextLine(_font, _text, _bounds.GetBottomLeft(), _color, _scale);
    UI::Element::DebugRender(renderer);
}

void Text::Render(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    renderer->DrawTextLine(_font, _text, _bounds.GetBottomLeft(), _color, _scale);
}

void Text::Update(float deltaSeconds, const IntVector2& mouse_position) {
    if(_enabled) {
        UpdateChildren(deltaSeconds, mouse_position);
    }
}

void Text::SetScale(float scale) {
    _scale = scale;
    SetSize({0.0f, 0.0f}, { _font->CalculateTextWidth(_text, _scale), _font->CalculateTextHeight(_text, _scale) });
}

float Text::GetScale() const {
    return _scale;
}

void Text::SetText(const std::string& text) {
    _text = text;
    float width = _font->CalculateTextWidth(_text, _scale);
    float height = _font->CalculateTextHeight(_text, _scale);
    auto old_size = GetSize();
    float old_width = old_size.x;
    float old_height = old_size.y;
    if(old_width < width) {
        SetSize(_size.ratio, {width, old_height});
    }
    old_size = GetSize();
    old_width = old_size.x;
    old_height = old_size.y;
    if(old_height < height) {
        SetSize(_size.ratio, {old_width, height});
    }
}

const std::string& Text::GetText() const {
    return _text;
}

void Text::SetTextColor(const Rgba& textColor) {
    _color = textColor;
}

const Rgba& Text::GetTextColor() const {
    return _color;
}

} //End UI