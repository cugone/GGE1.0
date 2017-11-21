#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

class Panel : public UI::Element {
public:
    Panel();
    virtual ~Panel() = default;

    virtual void DebugRender(SimpleRenderer* renderer) const override;

    virtual void Render(SimpleRenderer* renderer) const override;

    void SetEdgeColor(const Rgba& color);
    void SetFillColor(const Rgba& color);

    virtual void Update(float deltaSeconds, const IntVector2& mouse_position) override;

protected:
private:
    Rgba _edgeColor = Rgba::NOALPHA;
    Rgba _fillColor = Rgba::NOALPHA;
};

} //End UI