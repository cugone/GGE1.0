#include "Engine/Renderer/SpriteSheet.hpp"

#include <filesystem>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Texture2D.hpp"


//SpriteSheet::SpriteSheet(const std::string& imageFilePath, int tilesWide, int tilesHigh)
//: m_spriteSheetTexture()
//, m_spriteLayout(IntVector2::ONE)
//{
//    namespace FS = std::experimental::filesystem;
//    FS::path p(imageFilePath);
//    if(FS::exists(p) == false) {
//
//    }
//}

SpriteSheet::SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh)
    : m_spriteSheetTexture(&texture)
    , m_spriteSheetTexture2D(nullptr)
    , m_spriteLayout(tilesWide, tilesHigh)
{
    /* DO NOTHING */
}

SpriteSheet::SpriteSheet(const Texture2D* texture, int tilesWide, int tilesHigh)
    : m_spriteSheetTexture(nullptr)
    , m_spriteSheetTexture2D(texture)
    , m_spriteLayout(tilesWide, tilesHigh)
{
    /* DO NOTHING */
}

SpriteSheet::~SpriteSheet() {
    /* DO NOTHING */
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(int spriteX, int spriteY) const {
    Vector2 texCoords(1.0f / m_spriteLayout.x, 1.0f / m_spriteLayout.y);
    return AABB2(Vector2(texCoords.x * spriteX, texCoords.y * spriteY), Vector2(texCoords.x * (spriteX + 1), texCoords.y * (spriteY + 1)));
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const {
    return GetTexCoordsForSpriteCoords(spriteCoords.x, spriteCoords.y);
}
AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const {
    int x = spriteIndex % m_spriteLayout.x;
    int y = spriteIndex / m_spriteLayout.x;
    return GetTexCoordsForSpriteCoords(x, y);
}

int SpriteSheet::GetNumSprites() const {
    if(m_spriteSheetTexture) {
        return ((*m_spriteSheetTexture).m_texelDimensions.x / m_spriteLayout.x) * m_spriteLayout.y;
    }
    if(m_spriteSheetTexture2D) {
        return ((*m_spriteSheetTexture2D).GetDimensions().x / m_spriteLayout.x) * m_spriteLayout.y;
    }
    return 0;
}

const Texture& SpriteSheet::GetTexture() const {
    ASSERT_OR_DIE(m_spriteSheetTexture != nullptr, "OpenGL texture is nullptr, use GetTexture2D.");
    return *m_spriteSheetTexture;
}

const Texture2D& SpriteSheet::GetTexture2D() const {
    ASSERT_OR_DIE(m_spriteSheetTexture2D != nullptr, "DirectX texture is nullptr, use GetTexture.");
    return *m_spriteSheetTexture2D;
}
