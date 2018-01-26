#include "Engine/Renderer/Renderer.hpp"

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment(lib, "glu32") //Link Opengl Utilities library

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "ThirdParty/OpenGL/glext.h"
#include "ThirdParty/OpenGL/wglext.h"

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <filesystem>
#include <iterator>
#include <regex>
#include <string>

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/BitmapFont.hpp"

#include "Engine/Core/EngineBase.hpp"

#include "ThirdParty/stb/stb_image.h"

PFNGLGENBUFFERSPROC     glGenBuffers     = nullptr;
PFNGLBINDBUFFERPROC     glBindBuffer     = nullptr;
PFNGLBUFFERDATAPROC     glBufferData     = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;
PFNGLDELETEBUFFERSPROC  glDeleteBuffers  = nullptr;

GLenum GetGlBufferDataUsage(const Renderer::BufferDataUsage& usage);
GLenum GetGlBufferBindingTarget(const Renderer::BufferBindingTarget& target);
GLenum GetGlMipMapTarget(const Renderer::GenerateMipMapTarget& target);
GLenum GetGlMatrixMode(const Renderer::MatrixMode& mode);

void Renderer::HookUpOpenGlExtensions() {
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
}

unsigned int Renderer::CreateVbo() {
    unsigned int vboId;
    glGenBuffers(1, &vboId);
    return vboId;
}

void Renderer::BindBuffers(const Renderer::BufferBindingTarget& target, int vboId) {
    glBindBuffer(GetGlBufferBindingTarget(target), vboId);
}

void Renderer::BufferData(const Renderer::BufferBindingTarget& target, int byteCount, const std::vector<Vertex3D>& verts) {
    glBufferData(GetGlBufferBindingTarget(target), byteCount, verts.data(), GetGlBufferDataUsage(Renderer::BufferDataUsage::STATIC_DRAW));
}

void Renderer::GenerateMipMap(const Renderer::GenerateMipMapTarget& target) {
    glGenerateMipmap(GetGlMipMapTarget(target));
}

void Renderer::DeleteVbo(unsigned int vbo) {
    glDeleteBuffers(1, &vbo);
}

void Renderer::UnHookOpenGlExtensions() {
    glGenBuffers = nullptr;
    glBindBuffer = nullptr;
    glBufferData = nullptr;
    glGenerateMipmap = nullptr;
    glDeleteBuffers = nullptr;
}
Renderer::Renderer()
: m_deviceContext(nullptr)
, m_openGLRenderingContext(nullptr)
, m_currentTexture(nullptr)
{
    unsigned char white[] = {255, 255, 255, 255};
    int id = this->CreateOpenGLTexture(white, 1, 1, 4);
    Texture* m_defaultWhiteTexture = new Texture;
    m_defaultWhiteTexture->m_imageFilePath = "white";
    m_defaultWhiteTexture->m_textureID = id;
    m_defaultWhiteTexture->m_texelDimensions = IntVector2(1, 1);
    m_alreadyLoadedTextures.push_back(m_defaultWhiteTexture);
}


Renderer::~Renderer() {

    m_currentTexture = nullptr;

    for(auto t : m_alreadyLoadedTextures) {
        delete t;
        t = nullptr;
    }
    m_alreadyLoadedTextures.clear();

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(reinterpret_cast<HGLRC>(m_openGLRenderingContext));

    m_openGLRenderingContext = nullptr;
    m_deviceContext = nullptr;

    UnHookOpenGlExtensions();

}

void Renderer::BeginFrame() {
    EnableRenderCapabilities(GL_BLEND);
    SetBlendFunction(SourceBlendFactor::SOURCE_ALPHA, DestinationBlendFactor::ONE_MINUS_SOURCE_ALPHA);
    SetLineWidth(2.0f);
    EnableRenderCapabilities(GL_LINE_SMOOTH);
    ClearScreen(Rgba::BLACK);
}

void Renderer::EndFrame() {
    SwapBuffers(reinterpret_cast<HDC>(m_deviceContext));
}

void Renderer::DrawLineStrip(const Vertex2D* vertexes, int numVertexes) {
    std::vector<Vertex3D> verts(numVertexes);
    for(std::size_t i = 0; i < verts.size(); ++i) {
        verts[i] = vertexes[i];
    }
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::LINE_STRIP);
}

void Renderer::DrawLineLoop(const Vertex2D* vertexes, int numVertexes) {
    std::vector<Vertex3D> verts(numVertexes);
    for(std::size_t i = 0; i < verts.size(); ++i) {
        verts[i] = vertexes[i];
    }
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::LINE_LOOP);
}

void Renderer::DrawLines(const Vertex2D* vertexes, int numVertexes) {
    std::vector<Vertex3D> verts(numVertexes);
    for(std::size_t i = 0; i < verts.size(); ++i) {
        verts[i] = vertexes[i];
    }
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::LINES);
}

SpriteSheet* Renderer::CreateSpriteSheet(const std::string& filepath, int tileWidth, int tileHeight) {
    namespace FS = std::experimental::filesystem;
    Texture* t = nullptr;
    FS::path p(filepath);
    if(FS::exists(p) == false) {
        g_theFileLogger->LogFlush();
        ASSERT_OR_DIE(false, "Renderer::CreateSpriteSheet(...) filepath does not exist.");
        return nullptr;
    } else {
        t = CreateOrGetTexture(p.string());
    }
    return new SpriteSheet(*t, tileWidth, tileHeight);
}

Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath) {
    // Try to find that texture from those already loaded
    Texture* texture = GetTexture(imageFilePath);
    if(texture)
        return texture;

    texture = CreateTextureFromFile(imageFilePath);
    return texture;
}

Texture* Renderer::CreateOrGetTexture(const Image& image) {
    return CreateOrGetTexture(image.GetFilepath());
}

BitmapFont* Renderer::CreateBitmapFont(const std::string& filepath, int glyphWidth, int glyphHeight) {
    return new BitmapFont(CreateSpriteSheet(filepath, glyphWidth, glyphHeight));
}

void Renderer::DrawTextLine(const BitmapFont& font, const std::string& text,
                        const Vector2& bottomLeftStartPos, float fontHeight,
                        float fontAspect, const Rgba& tint /*= Rgba::WHITE*/,
                        const FontJustification& justification /*= FontJustification::LEFT*/)
{
    Vector2 current_glyph_position = bottomLeftStartPos;
    float glyph_width = fontHeight * fontAspect;
    float glyph_height = fontHeight;
    float draw_direction = 1.0f;
    Vector2 glyph_offset(glyph_width, 0.0f);
    Vector2 glyph_upper_right_offset(glyph_width, glyph_height);
    float text_width = BitmapFont::CalcWidth(text, glyph_height, fontAspect);
    float half_extents = text_width * 0.5f;
    std::string string_copy(text.begin(), text.end());
    switch(justification) {
        case Renderer::FontJustification::LEFT: {
            draw_direction = 1.0f;
            break;
        } case Renderer::FontJustification::CENTER: {
            current_glyph_position.x -= half_extents;
            draw_direction = 1.0f;
            break;
        } case Renderer::FontJustification::RIGHT: {
            draw_direction = -1.0f;
            string_copy = std::string(text.rbegin(), text.rend());
            break;
        } default: {
            /* DO NOTHING */;
        }
    }
    DisableRenderCapabilities(GL_DEPTH_TEST);
    for(const auto& c : string_copy) {
        AABB2 current_glyph_bounds(current_glyph_position, current_glyph_position + glyph_upper_right_offset);
        AABB2 glyph_coords = font.GetSheet()->GetTexCoordsForSpriteIndex(static_cast<int>(c));
        DrawTexturedAABB(current_glyph_bounds, font.GetSheet()->GetTexture(), glyph_coords.mins, glyph_coords.maxs, tint);
        current_glyph_position += glyph_offset * draw_direction;
    }

}
void Renderer::DrawMultilineText(const BitmapFont& font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect, const Rgba& tint /*= Rgba::WHITE*/, const FontJustification& justification /*= FontJustification::LEFT*/) {
    std::regex textlineRegex("(.+)+");
    auto textlineBegin = std::sregex_iterator(text.begin(), text.end(), textlineRegex);
    auto textlineEnd = std::sregex_iterator();
    float lineIndex = 0.0f;
    for(auto i = textlineBegin; i != textlineEnd; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        DrawTextLine(font, match_str, bottomLeftStartPos - Vector2(0.0f, (lineIndex * fontHeight)), fontHeight, fontAspect, tint, justification);
        lineIndex += 1.0f;
    }
}

void Renderer::DrawTexturedAABB2(const AABB2& bounds, const Texture& texture) {
    EnableRenderCapabilities(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);
    glColor4ub(255, 255, 255, 255);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.f, 1.f);
        glVertex2f(bounds.mins.x, bounds.mins.y);

        glTexCoord2f(1.f, 1.f);
        glVertex2f(bounds.maxs.x, bounds.mins.y);

        glTexCoord2f(1.f, 0.f);
        glVertex2f(bounds.maxs.x, bounds.maxs.y);

        glTexCoord2f(0.f, 0.f);
        glVertex2f(bounds.mins.x, bounds.maxs.y);
    }
    glEnd();
}

void Renderer::DrawTexture(const Vertex2D& lowerLeft, const Vertex2D& lowerRight, const Vertex2D& upperRight, const Vertex2D& upperLeft, const Texture& texture) {
    EnableRenderCapabilities(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);
    glBegin(GL_QUADS);
    {
        glColor4ub(lowerLeft.color.r, lowerLeft.color.g, lowerLeft.color.b, lowerLeft.color.a);
        glTexCoord2f(lowerLeft.texCoords.x, lowerLeft.texCoords.y);
        glVertex2f(lowerLeft.position.x, lowerLeft.position.y);

        glColor4ub(lowerRight.color.r, lowerRight.color.g, lowerRight.color.b, lowerRight.color.a);
        glTexCoord2f(lowerRight.texCoords.x, lowerRight.texCoords.y);
        glVertex2f(lowerRight.position.x, lowerRight.position.y);

        glColor4ub(upperRight.color.r, upperRight.color.g, upperRight.color.b, upperRight.color.a);
        glTexCoord2f(upperRight.texCoords.x, upperRight.texCoords.y);
        glVertex2f(upperRight.position.x, upperRight.position.y);

        glColor4ub(upperLeft.color.r, upperLeft.color.g, upperLeft.color.b, upperLeft.color.a);
        glTexCoord2f(upperLeft.texCoords.x, upperLeft.texCoords.y);
        glVertex2f(upperLeft.position.x, upperLeft.position.y);
    }
    glEnd();
}

void Renderer::DrawTexture(const Vertex2D& lowerLeft, const Vertex2D& upperRight, const Texture& texture) {
    EnableRenderCapabilities(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);
    glBegin(GL_QUADS);
    {
        glColor4ub(lowerLeft.color.r, lowerLeft.color.g, lowerLeft.color.b, lowerLeft.color.a);
        glTexCoord2f(lowerLeft.texCoords.x, upperRight.texCoords.y);
        glVertex2f(lowerLeft.position.x, lowerLeft.position.y);

        glColor4ub(lowerLeft.color.r, lowerLeft.color.g, lowerLeft.color.b, lowerLeft.color.a);
        glTexCoord2f(upperRight.texCoords.x, upperRight.texCoords.y);
        glVertex2f(upperRight.position.x, lowerLeft.position.y);

        glColor4ub(upperRight.color.r, upperRight.color.g, upperRight.color.b, upperRight.color.a);
        glTexCoord2f(upperRight.texCoords.x, lowerLeft.texCoords.y);
        glVertex2f(upperRight.position.x, upperRight.position.y);

        glColor4ub(lowerLeft.color.r, lowerLeft.color.g, lowerLeft.color.b, lowerLeft.color.a);
        glTexCoord2f(lowerLeft.texCoords.x, lowerLeft.texCoords.y);
        glVertex2f(lowerLeft.position.x, upperRight.position.y);
    }
    glEnd();
}

void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint) {
    DrawTexture(Vertex2D(bounds.mins, tint, texCoordsAtMins), Vertex2D(bounds.maxs, tint, texCoordsAtMaxs), texture);
}

void Renderer::DrawAxes(float axisLength, float lineWidth, float alpha) {
    
    unsigned char ubAlpha = static_cast<unsigned char>(255.0f * MathUtils::Clamp(alpha, 0.0f, 1.0f));
    BindTexture(nullptr);
    SetLineWidth(lineWidth);
    glBegin(GL_LINES);
    {
        glColor4ub(255, 0, 0, ubAlpha);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(axisLength, 0.0f, 0.0f);

        glColor4ub(0, 255, 0, ubAlpha);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, axisLength, 0.0f);

        glColor4ub(0, 0, 255, ubAlpha);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, axisLength);

    }
    glEnd();
}

void Renderer::DrawVertexVbo(int vboId, int numVertex, const PrimitiveDrawMode& primativeDrawMode) {
    BindBuffers(Renderer::BufferBindingTarget::ARRAY_BUFFER, vboId);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D), (const GLvoid*)offsetof(Vertex3D, position));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D), (const GLvoid*)offsetof(Vertex3D, color));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), (const GLvoid*)offsetof(Vertex3D, texCoords));

    glDrawArrays(static_cast<int>(primativeDrawMode), 0, numVertex);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    BindBuffers(Renderer::BufferBindingTarget::ARRAY_BUFFER, 0);

}
void Renderer::DrawVertexes(const Vertex3D* vertexArray, std::size_t numVertex, const Renderer::PrimitiveDrawMode& primitiveDrawMode) {

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    constexpr int stride = sizeof(Vertex3D);
    glVertexPointer(3, GL_FLOAT, stride, &vertexArray[0].position);
    glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexArray[0].color);
    glTexCoordPointer(2, GL_FLOAT, stride, &vertexArray[0].texCoords);
    
    glDrawArrays(static_cast<int>(primitiveDrawMode), 0, static_cast<int>(numVertex));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

Texture* Renderer::CreateTextureFromFile(const std::string& imageFilePath) {
    // Load image data
    int width = 0;
    int height = 0;
    int bytesPerTexel = 0;
    unsigned char* imageTexelBytes = stbi_load(imageFilePath.c_str(), &width, &height, &bytesPerTexel, 0);
    GUARANTEE_OR_DIE(imageTexelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));
    GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", imageFilePath.c_str(), bytesPerTexel));

    // Create texture on video card, and send image (texel) data
    unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
    stbi_image_free(imageTexelBytes);

    // Create (new) a Texture object
    Texture* texture = new Texture();
    texture->m_textureID = openGLTextureID;
    texture->m_imageFilePath = imageFilePath;
    texture->m_texelDimensions.SetXY(width, height);

    m_alreadyLoadedTextures.push_back(texture);
    return texture;
}

Texture* Renderer::GetTexture(const std::string& imageFilePath) {
    for(int textureIndex = 0; textureIndex < (int)m_alreadyLoadedTextures.size(); ++textureIndex) {
        Texture* texture = m_alreadyLoadedTextures[textureIndex];
        if(imageFilePath == texture->m_imageFilePath)
            return texture;
    }

    return nullptr;
}

unsigned int Renderer::CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel) {
    // Create a texture ID (or "name" as OpenGL calls it) for this new texture
    unsigned int openGLTextureID = 0xFFFFFFFF;
    glGenTextures(1, &openGLTextureID);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Tell OpenGL that our pixel data is single-byte aligned
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Tell OpenGL to bind (set) this as the currently active texture
    glBindTexture(GL_TEXTURE_2D, openGLTextureID);

    // Set texture clamp vs. wrap (repeat)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT

                                                                  // Set magnification (texel > pixel) and minification (texel < pixel) filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

    GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
    if(bytesPerTexel == 3)
        bufferFormat = GL_RGB;

    GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

    glTexImage2D(			// Upload this pixel data to our new OpenGL texture
                 GL_TEXTURE_2D,		// Creating this as a 2d texture
                 0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
                 internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
                 width,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
                 height,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
                 0,					// Border size, in texels (must be 0 or 1)
                 bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
                 GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
                 imageTexelBytes);	// Location of the actual pixel data bytes/buffer

    return openGLTextureID;
}

void Renderer::SetMatrixMode(const MatrixMode& mode) {
    glMatrixMode(GetGlMatrixMode(mode));
}
void Renderer::EnableBackFaceCulling(bool isNowBackfaceCulling /*= true*/) {
    if(isNowBackfaceCulling) {
        EnableRenderCapabilities(GL_CULL_FACE);
    } else {
        DisableRenderCapabilities(GL_CULL_FACE);
    }
}
void Renderer::EnableDepthTesting(bool isNowDepthTesting /*= true*/) {
    if(isNowDepthTesting) {
        EnableRenderCapabilities(GL_DEPTH_TEST);
        SetDepthFunction(AlphaFunction::LESS);
    } else {
        DisableRenderCapabilities(GL_DEPTH_TEST);
        SetDepthFunction(AlphaFunction::ALWAYS);
    }
}

void Renderer::EnableDepthWriting(bool isNowDepthWriting /*= true*/) {
    glDepthMask(isNowDepthWriting);
}
Matrix4 Renderer::GetCurrentModelViewMatrix() {
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    return  Matrix4(Vector4(m[0],  m[1],  m[2],  m[3]),
                    Vector4(m[4],  m[5],  m[6],  m[7]),
                    Vector4(m[8],  m[9],  m[10], m[11]),
                    Vector4(m[12], m[13], m[14], m[15]));
}

Matrix4 Renderer::GetCurrentProjectionMatrix() {
    float m[16];
    glGetFloatv(GL_PROJECTION_MATRIX, m);
    return  Matrix4(Vector4(m[0], m[1], m[2], m[3]),
                    Vector4(m[4], m[5], m[6], m[7]),
                    Vector4(m[8], m[9], m[10], m[11]),
                    Vector4(m[12], m[13], m[14], m[15]));
}

void Renderer::LoadIdentity() {
    glLoadIdentity();
}

void Renderer::CreateRenderContext(void* hWnd) {

    PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
    memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
    pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
    pixelFormatDescriptor.nVersion = 1;
    pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDescriptor.cColorBits = 24;
    pixelFormatDescriptor.cDepthBits = 24;
    pixelFormatDescriptor.cAccumBits = 0;
    pixelFormatDescriptor.cStencilBits = 8;
    
    m_deviceContext = GetDC(reinterpret_cast<HWND>(hWnd));

    auto hdc = reinterpret_cast<HDC>(m_deviceContext);
    int pixelFormatCode = ChoosePixelFormat(hdc, &pixelFormatDescriptor);
    SetPixelFormat(hdc, pixelFormatCode, &pixelFormatDescriptor);
    m_openGLRenderingContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, reinterpret_cast<HGLRC>(m_openGLRenderingContext));

    HookUpOpenGlExtensions();
}

void Renderer::DrawPoint(const Vertex2D& vertex) {
    DrawPoint(Vertex3D(vertex));
}

void Renderer::DrawLine(const Vertex2D& startVertex, const Vertex2D& endVertex, float lineThickness) {
    DrawLine(Vertex3D(startVertex), Vertex3D(endVertex), lineThickness);
}

void Renderer::DrawTriangle(const Vertex2D& vertexOne, const Vertex2D& vertexTwo, const Vertex2D& vertexThree, float lineThickness /*= 2.0f*/) {
    DrawTriangle(Vertex3D(vertexOne), Vertex3D(vertexTwo), Vertex3D(vertexThree), lineThickness);
}

void Renderer::DrawRectangle(const Vector2& mins, const Vector2& maxs, const Rgba& color, float lineThickness) {
    DrawRectangle(Vertex2D(mins, color), Vertex2D(Vector2(mins.x, maxs.y), color), Vertex2D(maxs, color), Vertex2D(Vector2(maxs.x, mins.y), color), lineThickness);
}

void Renderer::DrawRectangle(const Vertex2D& lowerRightVertex, const Vertex2D& upperRightVertex, const Vertex2D& upperLeftVertex, const Vertex2D& lowerLeftVertex, float lineThickness /*= 2.0f*/) {

#pragma region vertex data

    std::vector<Vertex3D> verts;
    verts.reserve(4);
    verts.push_back(lowerRightVertex);
    verts.push_back(upperRightVertex);
    verts.push_back(upperLeftVertex);
    verts.push_back(lowerLeftVertex);
#pragma endregion
    SetLineWidth(lineThickness);
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::QUADS);
}
void Renderer::DrawRegularPolygonOutline(const Vertex2D& centerPositionEdgeColor, float numSides, float radius) {

#pragma region vertex data
    std::vector<Vertex3D> verts;
    verts.reserve(static_cast<std::size_t>(numSides));
    float anglePerVertex = 360.0f / numSides;
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float centerX = radius * std::cosf(radians) + centerPositionEdgeColor.position.x;
        float centerY = radius * std::sinf(radians) + centerPositionEdgeColor.position.y;
        verts.push_back(Vertex3D(Vector3(centerX, centerY, 0.0f), centerPositionEdgeColor.color));
    }
#pragma endregion

    DrawPolygonOutline(verts.data(), verts.size());
}

void Renderer::DrawPolygonOutline(const Vertex2D* vertecies, int vertexCount) {

#pragma region vertex data
    std::vector<Vertex3D> verts;
    verts.reserve(vertexCount);
    for(std::size_t v = 0; v < static_cast<std::size_t>(vertexCount); ++v) {
        verts.push_back(vertecies[v]);
    }
#pragma endregion

    DrawPolygonOutline(verts.data(), verts.size());
}

void Renderer::DrawCircle(const Vertex2D& centerPositionEdgeColor, float numSides, float radius) {
    DrawRegularPolygonOutline(centerPositionEdgeColor, numSides, radius);
}

void Renderer::DrawPolygonOutline(const Vertex3D* vertecies, std::size_t vertexCount) {
    BindTexture(nullptr);
    DrawVertexes(vertecies, vertexCount, PrimitiveDrawMode::LINE_LOOP);
}
void Renderer::DrawPoint(const Vertex3D& vertex) {

#pragma region vertex data
    std::vector<Vertex3D> verts;
    verts.reserve(1);
    verts.push_back(vertex);
#pragma endregion

    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::POINTS);
}
void Renderer::DrawLine(const Vertex3D& startVertex, const Vertex3D& endVertex, float lineThickness /* = 2.0f */) {

#pragma region vertex data
    std::vector<Vertex3D> verts;
    verts.reserve(2);
    verts.push_back(startVertex);
    verts.push_back(endVertex);
#pragma endregion

    SetLineWidth(lineThickness);
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::LINES);
}
void Renderer::DrawTriangle(const Vertex3D& vertexOne, const Vertex3D& vertexTwo, const Vertex3D& vertexThree, float lineThickness /*= 2.0f*/) {

#pragma region vertex data
    std::vector<Vertex3D> verts;
    verts.reserve(3);
    verts.push_back(vertexOne);
    verts.push_back(vertexTwo);
    verts.push_back(vertexThree);
#pragma endregion

    SetLineWidth(lineThickness);
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::LINE_LOOP);
}
void Renderer::DrawCube(const Vector3& mins, const Vector3& maxs, const Rgba& color, float lineWidth) {

#pragma region vertex data
    std::vector<Vertex3D> verts;
    verts.reserve(24);
    verts.push_back(Vertex3D(Vector3(maxs.x, mins.y, mins.z), color, Vector2::ZERO));//EAST
    verts.push_back(Vertex3D(Vector3(maxs.x, maxs.y, mins.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, maxs.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, mins.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, mins.y, mins.z), color, Vector2::ZERO));//WEST
    verts.push_back(Vertex3D(Vector3(mins.x, mins.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, maxs.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, maxs.y, mins.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, maxs.y, mins.z), color, Vector2::ZERO));//NORTH
    verts.push_back(Vertex3D(Vector3(mins.x, maxs.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, maxs.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, maxs.y, mins.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, mins.y, mins.z), color, Vector2::ZERO));//SOUTH
    verts.push_back(Vertex3D(Vector3(maxs.x, mins.y, mins.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, mins.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, mins.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, mins.y, maxs.z), color, Vector2::ZERO));//TOP
    verts.push_back(Vertex3D(Vector3(maxs.x, mins.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, maxs.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, maxs.y, maxs.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(mins.x, mins.y, mins.z), color, Vector2::ZERO));//BOTTOM
    verts.push_back(Vertex3D(Vector3(mins.x, maxs.y, mins.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, maxs.y, mins.z), color, Vector2::ZERO));
    verts.push_back(Vertex3D(Vector3(maxs.x, mins.y, mins.z), color, Vector2::ZERO));
#pragma endregion

    DrawCube(verts.data(), static_cast<int>(verts.size()), lineWidth);
}

void Renderer::DrawCube(const Vertex3D* vertexArray, int numVertexes, float lineWidth /*= 2.0f*/) {
    SetLineWidth(lineWidth);
    BindTexture(nullptr);
    DrawVertexes(vertexArray, numVertexes, PrimitiveDrawMode::LINE_LOOP);
}

void Renderer::DrawSphere(const Renderer::SphereType& type, const Vertex3D& centerPositionEdgeColor, int tessellationExponent, float radius, float lineWidth /*= 2.0f*/) {
    tessellationExponent = (std::max)(0, tessellationExponent);
    float numSides = static_cast<float>(BIT(tessellationExponent));
    switch(type) {
        case SphereType::ICOSPHERE:
            DrawIcoSphere(centerPositionEdgeColor, numSides, radius, lineWidth);
            break;
        case SphereType::UVSPHERE:
            DrawUvSphere(centerPositionEdgeColor, numSides, radius, lineWidth);
            break;
    }

}

void Renderer::DrawUvSphere(const Vertex3D& centerPositionEdgeColor, float numSides, float radius, float lineWidth /*= 2.0f*/) {
    SetLineWidth(lineWidth);
    DisableRenderCapabilities(GL_TEXTURE_2D);
    BindTexture(nullptr);
    SetPolygonMode(PolygonModeFace::FRONT_AND_BACK, PolygonModeRasterizer::LINE);
    if(numSides < 0.0) {
        return;
    }

#pragma region vertex data

    std::vector<Vertex3D> verts;
    float anglePerVertexPhi = 360.0f / numSides;
    float anglePerVertexTheta = 360.0f / numSides;
    verts.reserve(static_cast<std::size_t>(2.0f + 4.0f * anglePerVertexTheta * anglePerVertexPhi));

    float cos_phi = std::cos(0.0f);
    float cos_theta = std::cos(0.0f);
    float sin_phi = std::sin(0.0f);
    float sin_theta = std::sin(0.0f);

    float x = centerPositionEdgeColor.position.x + radius * cos_phi * cos_theta;
    float y = centerPositionEdgeColor.position.y + radius * cos_phi * sin_theta;
    float z = centerPositionEdgeColor.position.z + radius * sin_phi;

    //LONGITUDINAL
    for(float degreesTheta = 0.0f; degreesTheta < 720.0f; degreesTheta += anglePerVertexTheta) {
        float radiansTheta = MathUtils::ConvertDegreesToRadians(degreesTheta);
        for(float degreesPhi = -180.0f; degreesPhi < 180.0f; degreesPhi += anglePerVertexPhi) {
            float radiansPhi = MathUtils::ConvertDegreesToRadians(degreesPhi);

            cos_phi = std::cos(radiansPhi);
            cos_theta = std::cos(radiansTheta);
            sin_phi = std::sin(radiansPhi);
            sin_theta = std::sin(radiansTheta);

            x = centerPositionEdgeColor.position.x + radius * cos_phi * cos_theta;
            y = centerPositionEdgeColor.position.y + radius * cos_phi * sin_theta;
            z = centerPositionEdgeColor.position.z + radius * sin_phi;

            verts.push_back(Vertex3D(Vector3(x, y, z), centerPositionEdgeColor.color));
        }
    }
    cos_phi = std::cos(MathUtils::ConvertDegreesToRadians(-180.0f));
    cos_theta = std::cos(MathUtils::ConvertDegreesToRadians(0.0f));
    sin_phi = std::sin(MathUtils::ConvertDegreesToRadians(-180.0f));
    sin_theta = std::sin(MathUtils::ConvertDegreesToRadians(0.0f));

    x = centerPositionEdgeColor.position.x + radius * cos_phi * cos_theta;
    y = centerPositionEdgeColor.position.y + radius * cos_phi * sin_theta;
    z = centerPositionEdgeColor.position.z + radius * sin_phi;

    verts.push_back(Vertex3D(Vector3(x, y, z), centerPositionEdgeColor.color));
    //LATITUDINAL
    for(float degreesPhi = -180.0f; degreesPhi < 180.0f + anglePerVertexPhi; degreesPhi += anglePerVertexPhi) {
        float radiansPhi = MathUtils::ConvertDegreesToRadians(degreesPhi);
        for(float degreesTheta = 0.0f; degreesTheta < 720.0f + anglePerVertexTheta; degreesTheta += anglePerVertexTheta) {
            float radiansTheta = MathUtils::ConvertDegreesToRadians(degreesTheta);

            cos_phi = std::cos(radiansPhi);
            cos_theta = std::cos(radiansTheta);
            sin_phi = std::sin(radiansPhi);
            sin_theta = std::sin(radiansTheta);

            x = centerPositionEdgeColor.position.x + radius * cos_phi * cos_theta;
            y = centerPositionEdgeColor.position.y + radius * cos_phi * sin_theta;
            z = centerPositionEdgeColor.position.z + radius * sin_phi;

            verts.push_back(Vertex3D(Vector3(x, y, z), centerPositionEdgeColor.color));
            verts.push_back(verts.back());
        }
    }
#pragma endregion

    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::LINES);
}
void Renderer::DrawIcoSphere(const Vertex3D& /*centerPositionEdgeColor*/, float /*numSides*/, float /*radius*/, float /*lineWidth*/ /*= 2.0f*/) {
    /* DO NOTHING */
}

void Renderer::DrawTeapot(const Vertex3D& /*centerPositionEdgeColor*/, float /*size = 1.0f*/, float /*lineWidth = 2.0f*/) {
    /* DO NOTHING */
}

void Renderer::DrawFilledTriangle(const Vertex2D& vertexOne, const Vertex2D& vertexTwo, const Vertex2D& vertexThree) {

#pragma region vertex data

    std::vector<Vertex3D> verts;
    verts.reserve(3);

    verts.push_back(vertexOne);
    verts.push_back(vertexTwo);
    verts.push_back(vertexThree);

#pragma endregion
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), Renderer::PrimitiveDrawMode::TRIANGLES);
}

void Renderer::DrawFilledRectangle(const Vertex2D& lowerLeft, const Vertex2D& upperRight) {
    DrawFilledRectangle(Vertex2D(Vector2(upperRight.position.x, lowerLeft.position.y), lowerLeft.color), upperRight, Vertex2D(Vector2(lowerLeft.position.x, upperRight.position.y), upperRight.color), lowerLeft);
}

void Renderer::DrawFilledRectangle(const Vertex2D& lowerRightVertex, const Vertex2D& upperRightVertex, const Vertex2D& upperLeftVertex, const Vertex2D& lowerLeftVertex) {

#pragma region vertex data

    std::vector<Vertex3D> verts;
    verts.reserve(4);

    verts.push_back(lowerRightVertex);
    verts.push_back(upperRightVertex);
    verts.push_back(upperLeftVertex);
    verts.push_back(lowerLeftVertex);

#pragma endregion
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::QUADS);

}

void Renderer::DrawFilledCircle(const Vertex2D& center, const Vertex2D& edge, float numSides) {

#pragma region vertex data

    std::vector<Vertex3D> verts;
    verts.reserve(static_cast<std::size_t>(numSides));
    verts.push_back(center);

    float anglePerVertex = 360.0f / numSides;
    float radius = CalcDistance(edge.position, center.position);
    for(float degrees = 0.0f; degrees <= 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float centerX = radius * std::cos(radians) + center.position.x;
        float centerY = radius * std::sin(radians) + center.position.y;
        verts.push_back(Vertex3D(Vector3(centerX, centerY, 0.0f), edge.color));
    }

#pragma endregion
    BindTexture(nullptr);
    DrawVertexes(verts.data(), verts.size(), PrimitiveDrawMode::TRIANGLE_FAN);

}

void Renderer::DrawAABB(const AABB2& bounds, const Rgba& color) {
    DrawFilledRectangle(Vertex2D(bounds.mins, color), Vertex2D(bounds.maxs, color));
}

Vector2 Renderer::GetWindowDimensions() {
    RECT rect;
    ::GetClientRect(reinterpret_cast<HWND>(m_deviceContext), &rect);
    return Vector2(IntVector2(rect.right - rect.left, rect.bottom - rect.top));
}

void Renderer::ApplyMatrix(const Matrix4& mat) {
    glMultMatrixf(mat.GetAsFloatArray());
}
void Renderer::LoadMatrix(const Matrix4& mat) {
    Matrix4 matcopy = mat;
    matcopy.Transpose();
    glLoadMatrixf(matcopy.GetAsFloatArray());
}
void Renderer::ClearScreen(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::ClearScreen(const Rgba& color) {
    ClearScreen(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

void Renderer::PushMatrix() {
    glPushMatrix();
}

void Renderer::Translate2D(const Vector2& translate) {
    Translate(translate.x, translate.y, 0.0f);
}

void Renderer::Rotate2D(float angleDegrees) {
    Rotate(angleDegrees, 0.0f, 0.0f, 1.0f);
}

void Renderer::Scale2D(const Vector2& scale) {
    Scale(scale.x, scale.y, 1.0f);
}

void Renderer::RotateAroundPosition2D(const Vector2& pos, float angleDegrees) {
    Translate2D(pos);
    Rotate2D(angleDegrees);
    Translate2D(-pos);
}

void Renderer::Translate(float x, float y, float z) {
    glTranslatef(x, y, z);
}

void Renderer::Translate(const Vector3& translate) {
    Translate(translate.x, translate.y, translate.z);
}
void Renderer::Rotate(float angleDegrees, float x, float y, float z) {
    glRotatef(angleDegrees, x, y, z);
}

void Renderer::Rotate(float angleDegrees, const Vector3& rotationAxis) {
    Rotate(angleDegrees, rotationAxis.x, rotationAxis.y, rotationAxis.z);
}
void Renderer::Scale(float x, float y, float z) {
    glScalef(x, y, z);
}

void Renderer::Scale(const Vector3& scale) {
    Scale(scale.x, scale.y, scale.z);
}
void Renderer::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}
void Renderer::PopMatrix() {
    glPopMatrix();
}

void Renderer::Ortho2D(const Vector2& lowerLeft, const Vector2& upperRight) {
    Ortho(lowerLeft, upperRight, Vector2(0.0f, 1.0f));
}

void Renderer::Ortho(const Vector2& lowerLeft, const Vector2& upperRight, const Vector2& zNearFar) {
    glOrtho(lowerLeft.x, upperRight.x, lowerLeft.y, upperRight.y, zNearFar.x, zNearFar.y);
}

void Renderer::SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance) {
    glMatrixMode(GL_PROJECTION);
    LoadIdentity();
    gluPerspective(fovVerticalDegrees, aspectRatio, nearDistance, farDistance);
}
void Renderer::SetOrthographicProjection(const Vector2& lowerLeft, const Vector2& upperRight, const Vector2& zNearFar) {
    glMatrixMode(GL_PROJECTION);
    LoadIdentity();
    Ortho(lowerLeft,upperRight, zNearFar);
}

void Renderer::SetOrthographic2DProjection(const Vector2& lowerLeft, const Vector2& upperRight) {
    SetOrthographicProjection(lowerLeft, upperRight, Vector2(0.0f, 1.0f));
}

void Renderer::EnableRenderCapabilities(int cap) {
    glEnable(cap);
}

void Renderer::DisableRenderCapabilities(int cap) {
    glDisable(cap);
}

void Renderer::SetPolygonMode(const PolygonModeFace& face, const PolygonModeRasterizer& mode) {
    glPolygonMode(static_cast<int>(face), static_cast<int>(mode));
}
void Renderer::BindTexture(const Texture* texture) {
    if(texture) {
        EnableRenderCapabilities(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture->m_textureID);
        m_currentTexture = const_cast<Texture*>(texture);
    } else {
        DisableRenderCapabilities(GL_TEXTURE_2D);
        auto t = GetTexture("white");
        glBindTexture(GL_TEXTURE_2D, t->m_textureID);
        m_currentTexture = nullptr;
    }
}
void Renderer::SetDepthFunction(const Renderer::AlphaFunction& depthFunction) {
    glDepthFunc(static_cast<int>(depthFunction));
}
void Renderer::SetBlendFunction(const Renderer::SourceBlendFactor& sourceBlendFactor, const Renderer::DestinationBlendFactor& destinationBlendFactor) {
    glBlendFunc(static_cast<int>(sourceBlendFactor), static_cast<int>(destinationBlendFactor));
}

void Renderer::SetLineWidth(float lineWidth) {
    glLineWidth(lineWidth);
}

GLenum GetGlMatrixMode(const Renderer::MatrixMode& mode) {
    switch(mode) {
        case Renderer::MatrixMode::MODELVIEW: return GL_MODELVIEW;
        case Renderer::MatrixMode::PROJECTION: return GL_PROJECTION;
        case Renderer::MatrixMode::TEXTURE: return GL_TEXTURE;
        default: ERROR_AND_DIE("GetGlRenderMode: mode value not in enum");
    }
}

GLenum GetGlBufferDataUsage(const Renderer::BufferDataUsage& usage) {
    switch(usage) {
        case Renderer::BufferDataUsage::DYNAMIC_COPY: return GL_DYNAMIC_COPY;
        case Renderer::BufferDataUsage::DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
        case Renderer::BufferDataUsage::DYNAMIC_READ: return GL_DYNAMIC_READ;
        case Renderer::BufferDataUsage::STATIC_COPY: return GL_STATIC_COPY;
        case Renderer::BufferDataUsage::STATIC_DRAW: return GL_STATIC_DRAW;
        case Renderer::BufferDataUsage::STATIC_READ: return GL_STATIC_READ;
        case Renderer::BufferDataUsage::STREAM_COPY: return GL_STREAM_COPY;
        case Renderer::BufferDataUsage::STREAM_DRAW: return GL_STREAM_DRAW;
        case Renderer::BufferDataUsage::STREAM_READ: return GL_STREAM_READ;
        default: ERROR_AND_DIE("GetGlBufferDataUsage: usage value not in enum");
    }
}

GLenum GetGlBufferBindingTarget(const Renderer::BufferBindingTarget& target) {
    switch(target) {
        case Renderer::BufferBindingTarget::ARRAY_BUFFER: return GL_ARRAY_BUFFER;
        default: ERROR_AND_DIE("GetGlBufferDataUsage: usage value not in enum");
    }
}

GLenum GetGlMipMapTarget(const Renderer::GenerateMipMapTarget& target) {
    switch(target) {
        case Renderer::GenerateMipMapTarget::TEXTURE_1D:             return GL_TEXTURE_1D;
        case Renderer::GenerateMipMapTarget::TEXTURE_2D:             return GL_TEXTURE_2D;
        case Renderer::GenerateMipMapTarget::TEXTURE_3D:             return GL_TEXTURE_3D;
        case Renderer::GenerateMipMapTarget::TEXTURE_1D_ARRAY:       return GL_TEXTURE_1D_ARRAY;
        case Renderer::GenerateMipMapTarget::TEXTURE_2D_ARRAY:       return GL_TEXTURE_2D_ARRAY;
        case Renderer::GenerateMipMapTarget::TEXTURE_CUBE_MAP:       return GL_TEXTURE_CUBE_MAP;
        case Renderer::GenerateMipMapTarget::TEXTURE_CUBE_MAP_ARRAY: return GL_TEXTURE_CUBE_MAP_ARRAY;
        default: ERROR_AND_DIE("GetGlMipMapTarget: target value not in enum");
    }
}
