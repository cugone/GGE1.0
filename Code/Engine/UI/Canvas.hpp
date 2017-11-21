#pragma once

#include "Engine/UI/Element.hpp"

class Texture2D;
class SimpleRenderer;

namespace UI {

class Canvas : public UI::Element {
public:
    Canvas() = default;
    Canvas(SimpleRenderer* renderer, Texture2D* target, float reference_resolution);

    virtual ~Canvas() = default;

    void SetTargetTexture(SimpleRenderer* renderer, Texture2D* target);
    void SetTargetReferenceResolution(SimpleRenderer* renderer, float reference_resolution);

    virtual void DebugRender(SimpleRenderer* renderer) const override;
    virtual void Render(SimpleRenderer* renderer) const override;
    virtual void Update(float deltaSeconds, const IntVector2& mouse_position) override;
protected:
private:
    Vector2 CalcDimensionsFromTargetTextureAndReferenceResolution();

    SimpleRenderer* _renderer = nullptr;
    Texture2D* _target_texture = nullptr;
    float _reference_resolution = 1.0f;
};

} //End UI