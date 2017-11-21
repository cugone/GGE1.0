#pragma once

#include <string>
#include <vector>

#include "Engine/Renderer/Vertex2D.hpp"
#include "Engine/Renderer/Vertex3D.hpp"

class Texture;
class Vector2;
class Rgba;
class AABB2;
class Image;
class SpriteSheet;
class BitmapFont;
class Vector3;
class Matrix4;

class Renderer {
public:

    enum class PrimitiveDrawMode {
        POINTS         = 0x0000,
        LINES          = 0x0001,
        LINE_LOOP      = 0x0002,
        LINE_STRIP     = 0x0003,
        TRIANGLES      = 0x0004,
        TRIANGLE_STRIP = 0x0005,
        TRIANGLE_FAN   = 0x0006,
        QUADS          = 0x0007,
        QUAD_STRIP     = 0x0008,
        POLYGON        = 0x0009,
    };

    enum class AlphaFunction {
        NEVER    = 0x0200,
        LESS     = 0x0201,
        EQUAL    = 0x0202,
        LEQUAL   = 0x0203,
        GREATER  = 0x0204,
        NOTEQUAL = 0x0205,
        GEQUAL   = 0x0206,
        ALWAYS   = 0x0207,
    };

    enum class SourceBlendFactor {
        ZERO                        = 0x0000,
        ONE                         = 0x0001,
        SOURCE_ALPHA                = 0x0302,
        ONE_MINUS_SOURCE_ALPHA      = 0x0303,
        DESTINATION_ALPHA           = 0x0304,
        ONE_MINUS_DESTINATION_ALPHA = 0x0305,
        DESTINATION_COLOR           = 0x0306,
        ONE_MINUS_DESTINATION_COLOR = 0x0307,
        SOURCE_ALPHA_SATURATE       = 0x0308,
    };

    enum class DestinationBlendFactor {
        ZERO                        = 0x0000,
        ONE                         = 0x0001,
        SOURCE_COLOR                = 0x0300,
        ONE_MINUS_SOURCE_COLOR      = 0x0301,
        SOURCE_ALPHA                = 0x0302,
        ONE_MINUS_SOURCE_ALPHA      = 0x0303,
        DESTINATION_ALPHA           = 0x0304,
        ONE_MINUS_DESTINATION_ALPHA = 0x0305,
    };

    enum class BufferBindingTarget {
        ARRAY_BUFFER              = 0x8892,
        ATOMIC_COUNTER_BUFFER     = 0x92C0,
        COPY_READ_BUFFER          = 0x8F36,
        COPY_WRITE_BUFFER         = 0x8F37,
        DISPATCH_INDIRECT_BUFFER  = 0x90EE,
        DRAW_INDIRECT_BUFFER      = 0x8F3F,
        ELEMENT_ARRAY_BUFFER      = 0x8893,
        PIXEL_PACK_BUFFER         = 0x88EB,
        PIXEL_UNPACK_BUFFER       = 0x88EC,
        QUERY_BUFFER              = 0x9192,
        SHADER_STORAGE_BUFFER     = 0x90D2,
        TEXTURE_BUFFER            = 0x8C2A,
        TRANSFORM_FEEDBACK_BUFFER = 0x8C8E,
        UNIFORM_BUFFER            = 0x8A11,
    };

    enum class BufferDataUsage {
        STREAM_DRAW  = 0x88E0,
        STREAM_READ  = 0x88E1,
        STREAM_COPY  = 0x88E2,
        STATIC_DRAW  = 0x88E4,
        STATIC_READ  = 0x88E5,
        STATIC_COPY  = 0x88E6,
        DYNAMIC_DRAW = 0x88E8,
        DYNAMIC_READ = 0x88E9,
        DYNAMIC_COPY = 0x88EA,
    };

    enum class GenerateMipMapTarget {
        TEXTURE_1D             = 0x0DE0,
        TEXTURE_2D             = 0x0DE1,
        TEXTURE_3D             = 0x806F,
        TEXTURE_1D_ARRAY       = 0x8C18,
        TEXTURE_2D_ARRAY       = 0x8C1A,
        TEXTURE_CUBE_MAP       = 0x8513,
        TEXTURE_CUBE_MAP_ARRAY = 0x9009,
    };

    enum class FontJustification {
        LEFT    = 0x00,
        CENTER  = 0x01,
        RIGHT   = 0x02,
    };

    enum class PolygonModeFace {
        FRONT          = 0x0404,
        BACK           = 0x0405,
        FRONT_AND_BACK = 0x0408,
    };

    enum class PolygonModeRasterizer {
        POINT = 0x1B00,
        LINE  = 0x1B01,
        FILL  = 0x1B02,
    };

    enum class SphereType {
        ICOSPHERE,
        UVSPHERE,
    };

    enum class MatrixMode {
        PROJECTION,
        MODELVIEW,
        TEXTURE,
    };

    Renderer();
    ~Renderer();
    
    void CreateRenderContext(void* hWnd);

    void ClearScreen(float red, float green, float blue, float alpha);
    void ClearScreen(const Rgba& color);
    void ClearDepth();

    void DrawPolygonOutline(const Vertex2D* vertecies, int vertexCount);

    void DrawPoint(const Vertex2D& vertex);
    void DrawLine(const Vertex2D& startVertex, const Vertex2D& endVertex, float lineThickness = 2.0f);
    void DrawTriangle(const Vertex2D& vertexOne, const Vertex2D& vertexTwo, const Vertex2D& vertexThree, float lineThickness = 2.0f);
    void DrawRectangle(const Vector2& mins, const Vector2& maxs, const Rgba& color, float lineThickness = 2.0f);
    void DrawRectangle(const Vertex2D& lowerRightVertex, const Vertex2D& upperRightVertex, const Vertex2D& upperLeftVertex, const Vertex2D& lowerLeftVertex, float lineThickness = 2.0f);
    void DrawRegularPolygonOutline(const Vertex2D& centerPositionEdgeColor, float numSides, float radius);
    void DrawCircle(const Vertex2D& centerPositionEdgeColor, float numSides, float radius);

    void DrawPolygonOutline(const Vertex3D* vertecies, int vertexCount);

    void DrawPoint(const Vertex3D& vertex);
    void DrawLine(const Vertex3D& startVertex, const Vertex3D& endVertex, float lineThickness = 2.0f);
    void DrawTriangle(const Vertex3D& vertexOne, const Vertex3D& vertexTwo, const Vertex3D& vertexThree, float lineThickness = 2.0f);
    void DrawCube(const Vector3& mins, const Vector3& maxs, const Rgba& color, float lineWidth = 2.0f);
    void DrawCube(const Vertex3D* vertexArray, int numVertexes, float lineWidth = 2.0f);
    void DrawSphere(const Renderer::SphereType& type, const Vertex3D& centerPositionEdgeColor, int tessellationExponent, float radius, float lineWidth = 2.0f);
    void DrawTeapot(const Vertex3D& centerPositionEdgeColor, float size = 1.0f, float lineWidth = 2.0f);

    void DrawVertexes(const Vertex3D* vertexArray, int numVertex, const Renderer::PrimitiveDrawMode& primitiveDrawMode);
    void DrawVertexVbo(int vboId, int numVertex, const PrimitiveDrawMode& primitiveDrawMode);

    void DrawFilledTriangle(const Vertex2D& vertexOne, const Vertex2D& vertexTwo, const Vertex2D& vertexThree);
    void DrawFilledRectangle(const Vertex2D& lowerLeft, const Vertex2D& upperRight);
    void DrawFilledRectangle(const Vertex2D& lowerRightVertex, const Vertex2D& upperRightVertex, const Vertex2D& upperLeftVertex, const Vertex2D& lowerLeftVertex);
    void DrawFilledCircle(const Vertex2D& center, const Vertex2D& edge, float numSides);
    void DrawAABB(const AABB2& bounds, const Rgba& color);

    Vector2 GetWindowDimensions();

    void ApplyMatrix(const Matrix4& mat);
    void LoadMatrix(const Matrix4& mat);
    Matrix4 GetCurrentModelViewMatrix();

    void PushMatrix();
    void Translate(const Vector3& translate);
    void Rotate(float angleDegrees, const Vector3& rotationAxis);
    void Scale(const Vector3& scale);
    void LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

    void Translate2D(const Vector2& translate);
    void Rotate2D(float angleDegrees);
    void Scale2D(const Vector2& scale);

    void RotateAroundPosition2D(const Vector2& pos, float angleDegrees);

    void PopMatrix();

    void LoadIdentity();
    void Ortho(const Vector2& lowerLeft, const Vector2& upperRight, const Vector2& zNearFar);
    void Ortho2D(const Vector2& lowerLeft, const Vector2& upperRight);

    void SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance);
    void SetOrthographicProjection(const Vector2& lowerLeft, const Vector2& upperRight, const Vector2& zNearFar);
    void SetOrthographic2DProjection(const Vector2& lowerLeft, const Vector2& upperRight);

    void EnableRenderCapabilities(int cap);
    void DisableRenderCapabilities(int cap);
    void SetPolygonMode(const Renderer::PolygonModeFace& face, const Renderer::PolygonModeRasterizer& mode);

    void BindTexture(const Texture* texture);
    void SetDepthFunction(const Renderer::AlphaFunction& depthFunction);
    void SetBlendFunction(const Renderer::SourceBlendFactor& sourceBlendFactor, const Renderer::DestinationBlendFactor& destinationBlendFactor);
    void SetLineWidth(float lineWidth);

    void BeginFrame();
    void EndFrame();

    void DrawLineStrip(const Vertex2D* vertexes, int numVertexes);
    void DrawLineLoop(const Vertex2D* vertexes, int numVertexes);
    void DrawLines(const Vertex2D* vertexes, int numVertexes);


    SpriteSheet* CreateSpriteSheet(const std::string& filepath, int tileWidth, int tileHeight);
    Texture* CreateOrGetTexture(const std::string& imageFilePath);
    Texture* CreateOrGetTexture(const Image& image);
    BitmapFont* CreateBitmapFont(const std::string& filepath, int glyphWidth, int glyphHeight);

    void DrawTextLine(const BitmapFont& font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect, const Rgba& tint = Rgba::WHITE, const Renderer::FontJustification& justification = Renderer::FontJustification::LEFT);
    void DrawMultilineText(const BitmapFont& font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect, const Rgba& tint = Rgba::WHITE, const Renderer::FontJustification& justification = Renderer::FontJustification::LEFT);

    void DrawTexturedAABB2(const AABB2& bounds, const Texture& texture);
    void DrawTexture(const Vertex2D& lowerLeft, const Vertex2D& lowerRight, const Vertex2D& upperRight, const Vertex2D& upperLeft, const Texture& texture);
    void DrawTexture(const Vertex2D& lowerLeft, const Vertex2D& upperRight, const Texture& texture);
    void DrawTexturedAABB(const AABB2& bounds, const Texture& texture,
                                    const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint);
    void DrawAxes(float axisLength, float lineWidth, float alpha);

    void EnableBackFaceCulling(bool isNowBackfaceCulling = true);
    void EnableDepthTesting(bool isNowDepthTesting = true);
    void EnableDepthWriting(bool isNowDepthWriting = true);

    void BindBuffers(const Renderer::BufferBindingTarget& target, int vboId);
    void BufferData(const Renderer::BufferBindingTarget& target, int byteCount, const std::vector<Vertex3D>& verts);
    void GenerateMipMap(const Renderer::GenerateMipMapTarget& target);
    void DeleteVbo(unsigned int vbo);
    unsigned int CreateVbo();
    void SetMatrixMode(const Renderer::MatrixMode& mode);

protected:
    void HookUpOpenGlExtensions();
    void UnHookOpenGlExtensions();
    void DrawIcoSphere(const Vertex3D& centerPositionEdgeColor, float numSides, float radius, float lineWidth = 2.0f);
    void DrawUvSphere(const Vertex3D& centerPositionEdgeColor, float numSides, float radius, float lineWidth = 2.0f);

    void Translate(float x, float y, float z);
    void Rotate(float angleDegrees, float x, float y, float z);
    void Scale(float x, float y, float z);

    Texture* CreateTextureFromFile(const std::string& imageFilePath);
    Texture* GetTexture(const std::string& imageFilePath);
    unsigned int CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);

    void* m_deviceContext;
    void* m_openGLRenderingContext;
    std::vector<Texture*> m_alreadyLoadedTextures;
    Texture* m_currentTexture;

private:

};