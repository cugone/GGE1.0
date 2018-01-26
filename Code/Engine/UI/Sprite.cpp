#include "Engine/UI/Sprite.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Engine/UI/Canvas.hpp"

namespace UI {

Sprite::Sprite(SpriteSheet* sprite)
    : _image{new SpriteAnimation{ sprite, 0.0f, SpriteAnimation::SpriteAnimMode::PLAY_TO_END, 0, 0 }}
{
    /* DO NOTHING */
}

Sprite::Sprite(const SpriteAnimation& sprite)
    : _image{ new SpriteAnimation{sprite} }
{
    /* DO NOTHING */
}

Sprite::Sprite(UI::Canvas* parentCanvas)
    : UI::Element(parentCanvas)
{
    /* DO NOTHING */
}

SpriteAnimation* Sprite::GetImage() const {
    return _image;
}

void Sprite::SetImage(SpriteSheet* sprite) {
    if(sprite) {
        delete _image;
        _image = new SpriteAnimation(sprite, 1.0f / sprite->GetNumSprites(), SpriteAnimation::SpriteAnimMode::PLAY_TO_END, 0, sprite->GetNumSprites());
    }
}

void Sprite::SetImage(const SpriteAnimation& sprite) {
    delete _image;
    _image = new SpriteAnimation(sprite);
}

const Texture2D* Sprite::GetTexture2D() const {
    return _image->GetTexture2D();
}

const Texture* Sprite::GetTexture() const {
    return _image->GetTexture();
}

void Sprite::SetFillMode(const UI::FILL_MODE& mode) {
    _fillmode = mode;
}

Sprite::~Sprite() {
    if(_image) {
        delete _image;
        _image = nullptr;
    }
}

void Sprite::DebugRender(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
    if(_image) {
        auto AABB2tex_coords = _image->GetCurrentTexCoords();
        Vector4 tex_coords{ AABB2tex_coords.mins.y, AABB2tex_coords.mins.x, AABB2tex_coords.maxs.y, AABB2tex_coords.maxs.x };
        renderer->DrawTexturedQuad(const_cast<Texture2D*>(_image->GetTexture2D()), Vector3(_bounds.GetTopLeft(), 0.0f), Vector3(GetSize() * 0.5f, 0.0f), Rgba::WHITE, tex_coords);
    }
    renderer->SetMaterial(nullptr);
    UI::Element::DebugRender(renderer);
}

void Sprite::Render(SimpleRenderer* renderer) const {
    renderer->SetModelMatrix(GetWorldTransform());
    renderer->SetMaterial(renderer->CreateOrGetMaterial("__2D"));
    if(_image) {
        auto AABB2tex_coords = _image->GetCurrentTexCoords();
        Vector4 tex_coords{ AABB2tex_coords.mins.y, AABB2tex_coords.mins.x, AABB2tex_coords.maxs.y, AABB2tex_coords.maxs.x };
        renderer->DrawTexturedQuad(const_cast<Texture2D*>(_image->GetTexture2D()), Vector3(_bounds.GetTopLeft(), 0.0f), Vector3(GetSize() * 0.5f, 0.0f), Rgba::WHITE, tex_coords);
    }
    renderer->SetMaterial(nullptr);
}

void Sprite::Update(float deltaSeconds, const Vector2& mouse_position) {

    if(IsEnabled()) {
        Element::Update(deltaSeconds, mouse_position);
        _image->Update(deltaSeconds);
    }
}

} //End UI