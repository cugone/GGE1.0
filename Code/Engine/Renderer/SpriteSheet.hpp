#pragma once

#include <string>

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"


class Texture;
class Texture2D;
class IntVector2;
class SimpleRenderer;
class Renderer;

class SpriteSheet {
public:

    SpriteSheet(SimpleRenderer& renderer, const XMLElement& elem);
    SpriteSheet(Renderer& renderer, const XMLElement& elem);
    SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh);
    SpriteSheet(const Texture2D* texture, int tilesWide, int tilesHigh);
    SpriteSheet(SimpleRenderer& renderer, const std::string& texturePath, int tilesWide, int tilesHigh);
    SpriteSheet(Renderer& renderer, const std::string& texturePath, int tilesWide, int tilesHigh);

    ~SpriteSheet();
    AABB2 GetTexCoordsForSpriteCoords(int spriteX, int spriteY) const; // mostly for atlases
    AABB2 GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const;
    AABB2 GetTexCoordsForSpriteIndex(int spriteIndex) const; // mostly for sprite animations
    int GetNumSprites() const;
    const Texture& GetTexture() const;
    const Texture2D& GetTexture2D() const;
private:
    void LoadFromXML(Renderer& renderer, const XMLElement& elem);
    void LoadFromXML(SimpleRenderer& renderer, const XMLElement& elem);

    const Texture* 	m_spriteSheetTexture = nullptr;	// Texture with grid-based layout of sub-textures
    const Texture2D* m_spriteSheetTexture2D = nullptr;
    IntVector2	m_spriteLayout;		// # of sprites across, and down, on the sheet
                                    //... and other data member variables as needed
};
