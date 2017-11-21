#pragma once

#include <string>

#include "Engine/Core/Rgba.hpp"

#include "Engine/UI/Element.hpp"

class KerningFont;

namespace UI {

class Text : public UI::Element {
public:
    Text() = default;
    Text(KerningFont* f);
    virtual ~Text() = default;

    virtual void DebugRender(SimpleRenderer* renderer) const override;

    virtual void Render(SimpleRenderer* renderer) const override;

    virtual void Update(float deltaSeconds, const IntVector2& mouse_position) override;

    void SetScale(float scale);
    float GetScale() const;

    void SetText(const std::string& text);
    const std::string& GetText() const;

    void SetTextColor(const Rgba& textColor);
    const Rgba& GetTextColor() const;

protected:
private:
    float _scale = 1.0f;
    Rgba _color = Rgba::BLACK;
    std::string _text = std::string{};
};

} //End UI