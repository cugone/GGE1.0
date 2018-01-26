#include "Engine/UI/Text.hpp"

#include "Engine/Core/KerningFont.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/UI/Canvas.hpp"

namespace UI {

Text::Text(KerningFont* f)
    : UI::Element()
{
    _font = f;
}

Text::Text(UI::Canvas* parentCanvas)
    : UI::Element(parentCanvas)
{
    /* DO NOTHING */
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

void Text::Update(float deltaSeconds, const Vector2& mouse_position) {
    if(_enabled) {
        Element::Update(deltaSeconds, mouse_position);
        UpdateChildren(deltaSeconds, mouse_position);
    }
}

void Text::SetScale(float scale) {
    _dirtyBounds = true;
    _scale = scale;
    CalcBoundsFromFont(_font);
}

float Text::GetScale() const {
    return _scale;
}

void Text::SetText(const std::string& text) {
    _dirtyBounds = true;
    _text = text;
    CalcBoundsFromFont(_font);
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

void Text::CalcBoundsFromFont(KerningFont* f) {
    if(f == nullptr) {
        return;
    }
    float width = f->CalculateTextWidth(_text, _scale);
    float height = f->CalculateTextHeight(_text, _scale);
    auto old_size = GetSize();
    float old_width = old_size.x;
    float old_height = old_size.y;
    if(old_width < width) {
        auto ratio = GetSizeRatio();
        SetSize({ {0.0f, 0.0f},{ width, old_height } });
    }
    if(old_height < height) {
        auto ratio = GetSizeRatio();
        SetSize({ { 0.0f, 0.0f },{ old_width, height } });
    }
}

void Text::SetFont(KerningFont* f) {
    UI::Element::SetFont(f);
    CalcBoundsFromFont(_font);
}

} //End UI