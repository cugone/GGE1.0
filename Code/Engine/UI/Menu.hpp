#pragma once

#include "Engine/UI/Element.hpp"

namespace UI {

class Menu : public UI::Element {
public:
    Menu() = default;
    virtual ~Menu() = default;

    virtual void Update(float deltaSeconds, const IntVector2& mouse_position) override;

    virtual void DebugRender(SimpleRenderer* renderer) const override;

    virtual void Render(SimpleRenderer* renderer) const override;

protected:
private:

};

} //End UI