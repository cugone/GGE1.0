#pragma once

#include <string>

class SpriteSheet;

class BitmapFont {
public:
    static float CalcWidth(const BitmapFont& font, const std::string& text);
    static float CalcWidth(const std::string& text, float fontHeight, float fontAspect);
    static float CalcMultilineHeight(const BitmapFont& font, const std::string& text);
    static float CalcMultilineHeight(const std::string& text, float fontHeight);
	BitmapFont(SpriteSheet* fontSheet);
	~BitmapFont();

    float CalcWidth(const std::string& text);
    float CalcMultilineHeight(const std::string& text);

    void SetHeight(float height);
    float GetHeight() const;

    void SetAspect(float aspect);
    float GetAspect() const;

    const SpriteSheet* GetSheet() const;

protected:
private:
	SpriteSheet* m_bitmapFont;
    float m_height;
    float m_aspect;
};