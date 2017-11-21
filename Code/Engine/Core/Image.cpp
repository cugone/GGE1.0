#include "Engine/Core/Image.hpp"

#include <sstream>
#include <vector>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "ThirdParty/stb/stb_image.h"

Image::Image(const std::string& filePath)
    : m_texelBytes(nullptr)
    , m_dimensions()
    , m_bytesPerTexel(0)
    , m_filepath(filePath)
    , m_memload(false)
{
    m_texelBytes = stbi_load(m_filepath.c_str(), &m_dimensions.x, &m_dimensions.y, &m_bytesPerTexel, 4);
    std::ostringstream ss;
    ss << "Failed to load image. " << m_filepath << " is not a supported image type.";
    ASSERT_OR_DIE(m_texelBytes != nullptr, ss.str());
}
Image::Image(unsigned int width /*= 1*/, unsigned int height /*= 1*/)
    : m_texelBytes(nullptr)
    , m_dimensions(width, height)
    , m_bytesPerTexel(0)
    , m_filepath()
    , m_memload(true)
{
    m_texelBytes = new unsigned char[width * height * 4];
    std::fill(&m_texelBytes[0], &m_texelBytes[0] + width * height * 4, static_cast<unsigned char>(0));
}

Image::~Image() {
    if(m_memload) {
        delete[] m_texelBytes;
        m_texelBytes = nullptr;
    } else {
        stbi_image_free(m_texelBytes);
        m_texelBytes = nullptr;
    }
}
Rgba Image::GetTexel(const IntVector2& texelPos) const {
    
    int index = texelPos.x + texelPos.y * m_dimensions.x;
    int byteOffset = index * m_bytesPerTexel;
    //HACK: If too slow, use following commented line instead.
    Rgba color;// = *(reinterpret_cast<Rgba>(&m_texelBytes[byteOffset]));
    color.r = m_texelBytes[byteOffset + 0];
    color.g = m_texelBytes[byteOffset + 1];
    color.b = m_texelBytes[byteOffset + 2];
    if(m_bytesPerTexel == 4) {
        color.a = m_texelBytes[byteOffset + 3];
    } else {
        color.a = 255;
    }
    return color;
}
void Image::SetTexel(const IntVector2& texelPos, const Rgba& color) {
    Rgba oldColor = GetTexel(texelPos);
    int index = texelPos.x + texelPos.y * m_dimensions.x;
    int byteOffset = index * m_bytesPerTexel;
    //HACK: If too slow, use following commented line instead.
    //m_texelBytes[byteOffset] = reinterpret_cast<unsigned char*>(&color);
    m_texelBytes[byteOffset + 0] = color.r;
    m_texelBytes[byteOffset + 1] = color.g;
    m_texelBytes[byteOffset + 2] = color.b;
    if(m_bytesPerTexel == 4) {
        m_texelBytes[byteOffset + 3] = color.a;
    }
}

const std::string& Image::GetFilepath() const {
    return m_filepath;
}

const IntVector2& Image::GetDimensions() const {
    return m_dimensions;
}
unsigned char* Image::GetData() const {
    return m_texelBytes;
}