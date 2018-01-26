#pragma once

#include "Engine/UI/Element.hpp"

class Texture2D;
class SimpleRenderer;
class InputSystem;

namespace UI {

class UiSystem;

class Canvas : public UI::Element {
public:
    Canvas() = default;
    Canvas(SimpleRenderer* renderer, Texture2D* target, float reference_resolution);

    virtual ~Canvas() = default;

    void SetTargetTexture(SimpleRenderer* renderer, Texture2D* target);
    void SetTargetReferenceResolution(SimpleRenderer* renderer, float reference_resolution);

    Vector2 ScreenToCanvasCoordinates(const Vector2& screen_coords);

    virtual void DebugRender(SimpleRenderer* renderer) const override;
    virtual void Render(SimpleRenderer* renderer) const override;
	void Update(float deltaSeconds, InputSystem* input, UI::UiSystem* ui);

	InputSystem* GetInput() const;
	UiSystem* GetUISystem() const;

protected:
private:
	virtual void Update(float deltaSeconds, const Vector2& mouse_position) override;
    Vector2 CalcDimensionsFromTargetTextureAndReferenceResolution();
	InputSystem* _input = nullptr;
	UiSystem* _ui = nullptr;
    SimpleRenderer* _renderer = nullptr;
    Texture2D* _target_texture = nullptr;
    float _reference_resolution = 1.0f;
};

} //End UI