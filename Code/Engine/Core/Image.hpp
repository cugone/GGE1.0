#pragma once

#include <string>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
class Image {
public:
    Image(const std::string& filePath);
    Image(unsigned int width = 1, unsigned int height = 1);
    ~Image(); //stbi_image_free(m_texelBytes)
    
    Rgba GetTexel(const IntVector2& texelPos) const;
    void SetTexel(const IntVector2& texelPos, const Rgba& color);

    const std::string& GetFilepath() const;
    const IntVector2& GetDimensions() const;

    unsigned char* GetData() const;
protected:
private:
    unsigned char* m_texelBytes;
    IntVector2 m_dimensions;
    int m_bytesPerTexel;
    std::string m_filepath;
    bool m_memload;
};