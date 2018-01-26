#include "Engine/UI/Canvas.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Input/InputSystem.hpp"

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

	_parentCanvas = this;

}

Vector2 Canvas::CalcDimensionsFromTargetTextureAndReferenceResolution() {
    if(!_target_texture) {
        _target_texture = _renderer->_rhi_output->GetBackBuffer();
    }
    Vector2 target_dims = static_cast<Vector2>(static_cast<IntVector2>(_target_texture->GetDimensions()));

    auto target_AR = target_dims.x / target_dims.y;
    Vector2 dims = Vector2::ZERO;
    if(target_AR <= 1.0f) {
        dims.x = _reference_resolution;
        dims.y = target_AR * _reference_resolution;
    } else {
        dims.x = target_AR * _reference_resolution;
        dims.y = _reference_resolution;
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

Vector2 Canvas::ScreenToCanvasCoordinates(const Vector2& screen_coords) {

    auto extents = GetSize();
    auto half_extents = extents * 0.50f;
    auto canvas_ndc_x = MathUtils::RangeMap(screen_coords.x, 0.0f, extents.x, -1.0f, 1.0f);
    auto canvas_ndc_y = MathUtils::RangeMap(screen_coords.y, 0.0f, extents.y, 1.0f, -1.0f);
    Vector4 ndc = Vector4(canvas_ndc_x, canvas_ndc_y, 0.0f, 1.0f);

    auto inv_projection = Matrix4::CalculateInverse(_renderer->_matrix_data.projection);
    auto inv_view = Matrix4::CalculateInverse(_renderer->_matrix_data.view);

    auto screen_coords_vec = Vector4(Vector3(screen_coords, 0.0), 1.0f);
    auto non_homogeneous_screen_coords = inv_view * inv_projection * ndc;
    auto homogeneous_screen_coords = Vector4::CalcHomogeneous(non_homogeneous_screen_coords);

    return Vector3(homogeneous_screen_coords);
}

void Canvas::DebugRender(SimpleRenderer* renderer) const {
    auto old_projection = renderer->GetProjectionMatrix();
    renderer->SetRenderTarget(_target_texture);
    renderer->SetOrthoProjection(Vector2(0.0f, GetSize().y), Vector2(GetSize().x, 0.0f), Vector2(0.0f, 1.0f));
    renderer->SetViewMatrix(Matrix4::GetIdentity());
    UI::Element::DebugRender(renderer);
    DebugRenderChildren(renderer);
    renderer->SetProjectionMatrix(old_projection);
    renderer->SetRenderTarget(nullptr);
}

void Canvas::Render(SimpleRenderer* renderer) const {
    auto old_projection = renderer->GetProjectionMatrix();
    renderer->SetRenderTarget(_target_texture);
    auto target_dims = Vector2((float)_target_texture->GetDimensions().x, (float)_target_texture->GetDimensions().y);
    renderer->SetOrthoProjection(Vector2(0.0f, target_dims.y), Vector2(target_dims.x, 0.0f), Vector2(0.0f, 1.0f));
    renderer->SetViewMatrix(Matrix4::GetIdentity());
    RenderChildren(renderer);
    renderer->SetProjectionMatrix(old_projection);
    renderer->SetRenderTarget(nullptr);
}

void Canvas::Update(float deltaSeconds, const Vector2& mouse_position) {
    auto canvas_pos = ScreenToCanvasCoordinates(mouse_position);
    if(_enabled) {
        Element::Update(deltaSeconds, canvas_pos);
        UpdateChildren(deltaSeconds, canvas_pos);
    }
}

void Canvas::Update(float deltaSeconds, InputSystem* input, UiSystem* ui) {
	if (!input) {
		ERROR_AND_DIE("Can not use UI without input system.");
	}
	_input = input;
	_ui = ui;
	Update(deltaSeconds, input->GetMouseCoords());
}

InputSystem* Canvas::GetInput() const {
	return _input;
}

UI::UiSystem* Canvas::GetUISystem() const {
	return _ui;
}

} //End UI