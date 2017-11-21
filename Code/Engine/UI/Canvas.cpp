#include "Engine/UI/Canvas.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/UI/UiSystem.hpp"

namespace UI {

Canvas::Canvas(SimpleRenderer* renderer, Texture2D* target, float reference_resolution)
: Element()
, _renderer(renderer)
, _target_texture(target)
, _reference_resolution(reference_resolution)
{
    if(!_target_texture) {
        _target_texture = _renderer->_rhi_output->GetBackBuffer();
    }
    GUARANTEE_OR_DIE(!MathUtils::IsEquivalent(_reference_resolution, 0.0f), "Canvas::Canvas: reference resolution must not be zero.");

    SetSize({ Vector2::ZERO, CalcDimensionsFromTargetTextureAndReferenceResolution() });

}

Vector2 Canvas::CalcDimensionsFromTargetTextureAndReferenceResolution() {
    if(!_target_texture) {
        _target_texture = _renderer->_rhi_output->GetBackBuffer();
    }
    Vector2 target_dims = target_dims = static_cast<Vector2>(static_cast<IntVector2>(_target_texture->GetDimensions()));

    auto target_AR = target_dims.x / target_dims.y;
    auto target_AR_inv = 1.0f / target_AR;
    Vector2 dims = Vector2::ZERO;
    if(target_AR <= 1.0f) {
        dims.x = target_AR * target_dims.y;
        dims.y = _reference_resolution;
    } else {
        dims.x = _reference_resolution;
        dims.y = target_AR_inv * target_dims.x;
    }
    return dims;
}

void Canvas::SetTargetTexture(SimpleRenderer* renderer, Texture2D* target) {

    GUARANTEE_OR_DIE(renderer != nullptr, "Canvas::SetTargetTexture: renderer must not be nullptr.");

    if(!target) {
        target = _renderer->_rhi_output->GetBackBuffer();
    }
    _renderer = renderer;
    _target_texture = target;
    auto dims = static_cast<Vector2>(static_cast<IntVector2>(_target_texture->GetDimensions()));
    SetSize({ Vector2::ZERO, CalcDimensionsFromTargetTextureAndReferenceResolution() });
}

void Canvas::SetTargetReferenceResolution(SimpleRenderer* renderer, float reference_resolution) {
    GUARANTEE_OR_DIE(renderer != nullptr, "Canvas::SetTargetTexture: renderer must not be nullptr.");
    GUARANTEE_OR_DIE(!MathUtils::IsEquivalent(_reference_resolution, 0.0f), "Canvas::SetTargetReferenceResolution: reference resolution must not be zero.");

    _renderer = renderer;
    _reference_resolution = reference_resolution;

    SetSize({ Vector2::ZERO, CalcDimensionsFromTargetTextureAndReferenceResolution() });

}

void Canvas::DebugRender(SimpleRenderer* renderer) const {
    auto old_projection = renderer->GetProjectionMatrix();
    renderer->SetRenderTarget(_target_texture);
    renderer->SetOrthoProjection(Vector2(0.0f, GetSize().y), Vector2(GetSize().x, 0.0f), Vector2(0.0f, 1.0f));
    UI::Element::DebugRender(renderer);
    DebugRenderChildren(renderer);
    renderer->SetProjectionMatrix(old_projection);
    renderer->SetRenderTarget(nullptr);
}

void Canvas::Render(SimpleRenderer* renderer) const {
    auto old_projection = renderer->GetProjectionMatrix();
    renderer->SetRenderTarget(_target_texture);
    renderer->SetOrthoProjection(Vector2(0.0f, GetSize().y), Vector2(GetSize().x, 0.0f), Vector2(0.0f, 1.0f));
    RenderChildren(renderer);
    renderer->SetProjectionMatrix(old_projection);
    renderer->SetRenderTarget(nullptr);
}

void Canvas::Update(float deltaSeconds, const IntVector2& mouse_position) {
    if(_enabled) {
        UpdateChildren(deltaSeconds, mouse_position);
    }
}

} //End UI