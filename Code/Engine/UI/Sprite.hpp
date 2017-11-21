#pragma once

#include "Engine/UI/Element.hpp"

class SpriteSheet;
class SpriteAnimation;
class Texture2D;
class Texture;

namespace UI {

class Sprite : public UI::Element {
public:
    Sprite() = default;
    Sprite(const SpriteSheet& sprite);
    Sprite(const SpriteAnimation& sprite);

    void SetImage(const SpriteSheet& sprite);
    void SetImage(const SpriteAnimation& sprite);
    const Texture2D* GetTexture2D() const;
    const Texture* GetTexture() const;

    void SetFillMode(const UI::FILL_MODE& mode);

    virtual ~Sprite();

    virtual void DebugRender(SimpleRenderer* renderer) const override;
    virtual void Render(SimpleRenderer* renderer) const override;

    virtual void Update(float deltaSeconds, const IntVector2& mouse_position) override;

protected:
private:
    SpriteAnimation* _image = nullptr;
    UI::FILL_MODE _fillmode = UI::FILL_MODE::FILL_MODE_STRETCH;
    
};

} //End UI