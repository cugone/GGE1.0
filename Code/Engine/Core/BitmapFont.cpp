#include "Engine/Core/BitmapFont.hpp"

#include "Engine/Renderer/SpriteSheet.hpp"

#include <algorithm>

BitmapFont::BitmapFont(SpriteSheet* fontSheet)
    : m_bitmapFont(fontSheet)
    , m_height(1.0f)
    , m_aspect(1.0f)
{
    /* DO NOTHING */
}

BitmapFont::~BitmapFont() {
    /* DO NOTHING */
}
float BitmapFont::CalcWidth(const BitmapFont& font, const std::string& text) {
    return CalcWidth(text, font.m_height, font.m_aspect);
}
float BitmapFont::CalcWidth(const std::string& text, float fontHeight, float fontAspect) {
    float glyph_width = fontHeight * fontAspect;
    std::size_t text_length = text.size();
    return glyph_width * text_length;
}
float BitmapFont::CalcWidth(const std::string& text) {
    return CalcWidth(*this, text);
}

float BitmapFont::CalcMultilineHeight(const BitmapFont& font, const std::string& text) {
    return CalcMultilineHeight(text, font.m_height);
}
float BitmapFont::CalcMultilineHeight(const std::string& text, float fontHeight) {
    auto line_count = std::count(text.begin(), text.end(), '\n') + 1;
    return fontHeight * line_count;
}
float BitmapFont::CalcMultilineHeight(const std::string& text) {
    return CalcMultilineHeight(*this, text);
}
void BitmapFont::SetHeight(float height) {
    m_height = height;
}

float BitmapFont::GetHeight() const {
    return m_height;
}

void BitmapFont::SetAspect(float aspect) {
    m_aspect = aspect;
}

float BitmapFont::GetAspect() const {
    return m_aspect;
}
const SpriteSheet* BitmapFont::GetSheet() const {
    return m_bitmapFont;
}