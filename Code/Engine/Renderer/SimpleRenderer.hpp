#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Math/Matrix4.hpp"

#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/BlendState.hpp"

#include "Engine/Renderer/Model.hpp"

#include <map>

class AABB2;

#define MATRIX_BUFFER_INDEX 0
#define TIME_BUFFER_INDEX 1
#define LIGHTING_BUFFER_INDEX 2
#define MAX_LIGHT_COUNT 8

struct time_buffer_t {
    float game_time;
    float system_time;
    float game_frame_time;
    float system_frame_time;
};

struct matrix_buffer_t {
    Matrix4 model;
    Matrix4 view;
    Matrix4 projection;
};

struct point_light_t
{
    Vector4 position;
    Vector4 color;
    Vector4 attenuation;
    Vector4 specAttenuation;
};

struct directional_light_t {
    point_light_t light;
    Vector4 direction;
};

struct lighting_buffer_t {
    point_light_t point_lights[MAX_LIGHT_COUNT];
    directional_light_t directional_lights[MAX_LIGHT_COUNT];
    Vector4 ambient;
    Vector4 specular_factor_power;
    Vector4 eye_position;
};

struct blend_state_t {
    bool enabled;
    BlendFactor source_factor;
    BlendFactor dest_factor;
};

struct depthstencil_state_t {
    bool depth_enabled;
    bool depth_write;
    bool stencil_enabled;
    bool stencil_read;
    bool stencil_write;
    StencilOperation stencil_failFrontOp;
    StencilOperation stencil_failBackOp;
    StencilOperation stencil_failDepthFrontOp;
    StencilOperation stencil_failDepthBackOp;
    StencilOperation stencil_passFrontOp;
    StencilOperation stencil_passBackOp;
    ComparisonFunction stencil_testFront;
    ComparisonFunction stencil_testBack;
};

class BitmapFont;
class ConstantBuffer;
class DepthStencilState;
class KerningFont;
class Image;
class IndexBuffer;
class Mesh;
class MeshMotion;
class MeshSkeleton;
class MeshSkeletonInstance;
class MeshBuilder;
class Rgba;
class RasterState;
class Sampler;
class Material;
class Shader;
class ShaderProgram;
class SpriteSheet;
class VertexBuffer;

enum class FontJustification {
    LEFT = 0x00,
    CENTERH = 0x01,
    RIGHT = 0x02,
    TOP = 0x04,
    CENTERV = 0x08,
    BOTTOM = 0x10,
};

class SimpleRenderer : public EngineSubsystem {
public:

    SimpleRenderer(unsigned int width, unsigned int height);
    virtual ~SimpleRenderer() override;

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

    void SetTitle(const std::string& title);

    void SetRenderTarget(Texture2D* color_target = nullptr, Texture2D* depthstencil_target = nullptr);

    void SetViewport( unsigned int x, unsigned int y, unsigned int width, unsigned int height );

    void SetViewportAsPercent( float x, float y, float w, float h );


    void ClearColor( const Rgba& color );

    void ClearTargetColor( Texture2D *target, const Rgba& color );

    void ClearDepthStencilBuffer();

    void Present(bool vsync = false); 
      
    Texture2D* CreateOrGetTexture(const std::string& filepath);

    bool RegisterTexture(const std::string& name, Texture2D *texture);
    void SetTexture(Texture2D* texture, unsigned int registerIndex = 0);

    void RegisterSampler(const std::string& name, Sampler* sampler);

    Sampler* GetSampler(const std::string& name);
    void SetSampler(Sampler* sampler, unsigned int registerIndex = 0);

    Material* CreateOrGetMaterial(const std::string& material_str);
    Material* GetMaterial(const std::string& name);
    void SetMaterial(Material* mat = nullptr);

    Shader* GetShader(const std::string& name);
    void SetShader(Shader* shader = nullptr);

    void DrawTextLine( const PrimitiveType& topology, VertexBuffer* vbo, unsigned int const vertex_count);
    void DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, const std::size_t vertex_count);
    void Draw(const PrimitiveType& topology, VertexBuffer* vbo, const std::size_t vertex_count);

    void SetShaderProgram(ShaderProgram* program = nullptr);
    void SetComputeShader(ComputeShader* shader = nullptr);
    void SetRasterState(RasterState* raster_state);

    void SetConstantBuffer(unsigned int index, ConstantBuffer* constant_buffer);

    void DrawQuad(const Vector3& leftBottomNear, const Vector3& rightTopFar, const Rgba& color, const Vector4& lbrtUV = Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    void DrawQuad(float b, float l, float n, float t, float r, float f, const Rgba& color);
    void DrawQuad(const Vector4& tlbrPosition = Vector4(0.0f, 0.0f, 1.0f, 1.0f), float z = 0.0f, const Rgba& color = Rgba::WHITE, const Vector4& tlbrUV = Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    void DrawTexturedQuad(Texture2D* texture, const Vector3& position, const Vector3& half_extents, const Rgba& color = Rgba::WHITE, const Vector4& tlbrUV = Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    void DrawQuadTwoSided(const Vector4& tlbrPosition = Vector4(0.0f, 0.0f, 1.0f, 1.0f), float z = 0.0f, const Rgba& color = Rgba::WHITE, const Vector4& tlbrUV = Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    void DrawQuadTwoSided(const Vector3& position, const Vector3& half_extents, const Rgba& color = Rgba::WHITE, const Vector4& tlbrUV = Vector4(0.0f, 0.0f, 1.0f, 1.0f));

    void DrawPoint(const Vertex3D& point);
    void DrawLine(const Vertex3D& start, const Vertex3D& end);

    void DrawCube(const Vector3& position, const Vector3& half_extents, const Rgba& color = Rgba::WHITE);

    void DrawUvSphere(const Vector3& position, const Rgba& color, float radius, unsigned int slices, unsigned int stacks);
    void DrawOctahedron(const Vector3& position, const Rgba& color, float radius);
    void DrawIcoSphere(const Vector3& position, const Rgba& color, float radius, unsigned int iterations);
    void DrawTextLine(KerningFont* f, const std::string& text, const Rgba& color = Rgba::WHITE, float sx = 0.0f, float sy = 0.0f, float scale = 1.0f);
    void DrawTextLine(KerningFont* f, const std::string& text, const Vector2& bottomLeftStartPos = Vector2::ZERO, const Rgba& color = Rgba::WHITE, float scale = 1.0f);

    void DrawTextLine(const BitmapFont& font, const std::string& text,
                                const Vector2& bottomLeftStartPos, float fontHeight,
                                float fontAspect, const Rgba& tint = Rgba::WHITE,
                                const FontJustification& justification = FontJustification::LEFT);

    void DrawMultilineText(const BitmapFont& font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect, const Rgba& tint = Rgba::WHITE, const FontJustification& justification = FontJustification::LEFT);
    void DrawMultilineText(KerningFont* font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect = 1.0f, const Rgba& tint = Rgba::WHITE, const FontJustification& justification = FontJustification::LEFT);

    void ReloadTextures();
    const std::map<std::string, Texture2D*>& GetLoadedTextures() const;

    const Matrix4& GetProjectionMatrix() const;
    void SetProjectionMatrix(const Matrix4& matrix);

    void SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far);
    void SetPerspectiveProjection(const Vector2& vfov_aspect, const Vector2& nz_fz);
    void SetPerspectiveProjectionFromCamera(const Camera3D& camera);
    void SetViewMatrix(const Matrix4& viewMatrix);
    void SetModelMatrix(const Matrix4& modelMatrix);
    void AppendModelMatrix(const Matrix4& modelMatrix);
    void SetAmbientLight(const Rgba& ambient);
    void SetPointLight(unsigned int index, const Vector3& camera_position, const Vector3& light_position, const Rgba& color = Rgba::WHITE, float intensity = 1.0f, const Vector3& attenuation = Vector3(0.0f, 0.0f, 1.0f), const Vector2& specular_factor_power = Vector2(1.0f, 8.0f));
    void SetSpotlight(unsigned int index, const Vector3& camera_position, const Vector3& light_position, const Vector3& light_direction = Vector3(1.0f, 0.0f, 0.0f), const Rgba& color = Rgba::WHITE, float intensity = 1.0f, const Vector3& attenuation = Vector3(0.0f, 0.0f, 1.0f), const Vector2& specular_factor_power = Vector2(1.0f, 8.0f), const Vector2& inner_outer_angleDegrees = Vector2(0.0f, 1.0f));
    void SetDirectionalLight(unsigned int index, const Vector3& camera_position, const Vector3& light_position, const Vector3& light_direction = Vector3(1.0f, 0.0f, 0.0f), const Rgba& color = Rgba::WHITE, float intensity = 1.0f, const Vector3& attenuation = Vector3(1.0f, 0.0f, 0.0f), const Vector2& specular_factor_power = Vector2(1.0f, 8.0f));

    Matrix4 CreateBillboardView(const Matrix4& cameraViewMatrix);

    bool RegisterFontsFromFolder(const std::string& folderpath, bool recursive = false);
    KerningFont* CreateOrGetFont(const std::string& name);
    BitmapFont* CreateBitmapFont(const std::string& filepath, int glyphWidth, int glyphHeight);
    SpriteSheet* CreateSpriteSheet(const std::string& filepath, int tileWidth, int tileHeight);
    SpriteSheet* CreateSpriteSheet(const XMLElement& elem);

    ShaderProgram* CreateOrGetShaderProgram(const std::string& name);
    ComputeShader* CreateOrGetComputeShader(const std::string& name);


    MeshMotion* CreateOrGetMotion(const std::string& fbx_path, MeshSkeleton& meshSkeleton, MeshSkeletonInstance& meshSkeletonInstance, const Matrix4& initialTransform = Matrix4::GetIdentity());

    MeshMotion* CreateMotionFromEngineAsset(const std::string& asset_path);
    MeshSkeleton* CreateOrGetSkeleton(const std::string& fbx_path, const Matrix4& initialTransform = Matrix4::GetIdentity());

    MeshSkeleton* CreateSkeletonFromEngineAsset(const std::string& asset_path);

    Mesh* CreateOrGetMesh(const Model::Type& type, const std::string& fbx_path, const Matrix4& initialTransform = Matrix4::GetIdentity(), MeshSkeleton* skeleton = nullptr);
    Mesh* CreateMeshFromEngineAsset(const std::string& asset_path);

    Model* CreateOrGetModel(const Model::Type& type, const std::string& folderpath, const Matrix4& initialTransform = Matrix4::GetIdentity());

    void ExportFBXToEngineAsset(const std::string& fbx_path, const Mesh& mesh, const MeshMotion& motion, const MeshSkeleton& skeleton);
    void ExportFBXMotionToEngineAsset(const std::string& fbx_path, const MeshMotion& motion);
    void ExportFBXSkeletonToEngineAsset(const std::string& fbx_path, const MeshSkeleton& skeleton);
    void ExportFBXMeshToEngineAsset(const std::string& fbx_path, const Mesh& mesh);

    void EnableBlend(const BlendFactor& source, const BlendFactor& dest);
    void DisableBlend();

    void EnableDepthStencil(bool depth = true
                            , bool stencil = false
                            , bool depth_write = true
                            , bool stencil_read = false
                            , bool stencil_write = false
                            , const ComparisonFunction& depthPredicate = ComparisonFunction::LESS
                            , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
                            , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
                            , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp = std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)
                            , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilPredicateFrontBack = std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS)
    );
    void DisableDepthStencil();

    void ReloadShaderPrograms(RHIDevice* device, const std::string& shaderprogram_path);
    
    bool RegisterShaderProgram(const std::string& name, ShaderProgram* program);
    bool RegisterShaderProgramFromFile(RHIDevice* device, const std::string& filepath, ShaderProgram*& output_program);
    bool RegisterShaderProgramsFromFolder(RHIDevice* device, const std::string& folderpath, bool recursive = false);
    
    bool RegisterRasterState(const std::string& name, RasterState* raster);
    
    bool RegisterTexturesFromFolder(const std::string& folderpath, bool recursive = false);
    
    bool RegisterComputeShaderFromFile(RHIDevice* device, const std::string& filepath, ComputeShader*& output_program);
    bool RegisterComputeShadersFromFolder(RHIDevice* device, const std::string& folderpath, bool recursive = false);
    
    bool RegisterShader(const std::string& name, Shader* shader);
    bool RegisterShaderFromFile(const std::string& filepath);
    bool RegisterShadersFromFolder(const std::string& folderpath, bool recursive = false);

    bool RegisterMaterial(const std::string& name, Material* mat);
    bool RegisterMaterialFromFile(const std::string& filepath);
    bool RegisterMaterialsFromFolder(const std::string& folderpath, bool recursive = false);

    bool RegisterMotion(const std::string& fbx_path, MeshMotion* motion);
    bool RegisterSkeleton(const std::string& fbx_path, MeshSkeleton* skeleton);
    bool RegisterMesh(const std::string& fbx_path, Mesh* mesh);

    bool RegisterModel(const std::string& folderpath, Model* model);

    RasterState* GetRasterState(const std::string& name);

    Texture2D* CreateTexture2DFromMemory(const unsigned char* data,
                                        unsigned int width = 1,
                                        unsigned int height = 1,
                                        const BufferUsage& bufferUsage = BufferUsage::STATIC,
                                        const BufferBindUsage& bindUsage = BufferBindUsage::SHADER_RESOURCE,
                                        const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNORM);

    Texture2D* CreateTexture2DFromMemory(const std::vector<Rgba>& data,
                                         unsigned int width = 1,
                                         unsigned int height = 1,
                                         const BufferUsage& bufferUsage = BufferUsage::STATIC,
                                         const BufferBindUsage& bindUsage = BufferBindUsage::SHADER_RESOURCE,
                                         const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNORM);


    Texture2D* CreateTexture2DFromImage(const Image* image,
                                        const BufferUsage& bufferUsage = BufferUsage::STATIC,
                                        const BufferBindUsage& bindUsage = BufferBindUsage::SHADER_RESOURCE,
                                        const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNORM);

    bool FastCopyTexture2D(Texture2D* dest, Texture2D* source);

    bool IsSupportedImageType(std::string extension);

    void RenderMesh(const Mesh& mesh, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo);

    void FontToAABB2(const BitmapFont& f, const std::vector<std::string>& textLines, const AABB2& box);
    void AABB2ToFont(const BitmapFont& f, const std::vector<std::string>& textLines, const AABB2& box);

    void DrawDebugLine2D(const Vector2& start, const Vector2& end, float thickness, const Rgba& startColor, const Rgba& endColor);
    void DrawDebugAABB2(const AABB2& box, const Rgba& edgeColor, const Rgba& fillColor);

    //edgeAlignment: 0: center; 1: inner; -1: outer; default: 1
    void DrawDebugBox2D(const Vector2& center, const Vector2& halfExtents, float thickness, const Rgba& fillColor, const Rgba& edgeColor, int edgeAlignment = 1);
    void DrawDebugBox2D(const AABB2& box, float edgeThickness, const Rgba& fillColor, const Rgba& edgeColor, int edgeAlignment = 1);
    void DrawDebugCircle2D(const Vector2& center, float radius, float thickness, const Rgba& fillColor, const Rgba& edgeColor, int edgeAlignment = 1);
    void DrawDebugX2D(const Vector2& center, float radius, float thickness, const Rgba& color);
    void DrawFilledCircle(const Vector2& center, float radius, const Rgba& fillColor = Rgba::WHITE);
    void DrawCircle(const Vector2& center, float radius, const Rgba& edgeColor = Rgba::WHITE);
public:
    RHIDeviceContext* _rhi_context;
    RHIDevice* _rhi_device;
    RHIOutput* _rhi_output;
    RHIInstance* _rhi_instance;
    Texture2D* _current_target;
    Texture2D* _current_depthstencil;
    Texture2D* _default_depthstencil;
    std::map<std::string, RasterState*> _rasters;
    std::map<std::string, Texture2D*> _textures;
    std::map<std::string, Sampler*> _samplers;
    std::map<std::string, KerningFont*> _fonts;
    std::map<std::string, ShaderProgram*> _shaderPrograms;
    std::map<std::string, Shader*> _shaders;
    std::map<std::string, ComputeShader*> _compute_shaders;
    std::map<std::string, Material*> _materials;
    std::map<std::string, Mesh*> _meshes;
    std::map<std::string, MeshMotion*> _motions;
    std::map<std::string, MeshSkeleton*> _skeletons;
    std::map<std::string, Model*> _models;

    VertexBuffer* _temp_vbo;
    IndexBuffer* _temp_ibo;
    std::size_t _current_vbo_size;
    std::size_t _current_ibo_size;
    time_buffer_t _time_data;
    ConstantBuffer* _time_cb;
    matrix_buffer_t _matrix_data;
    ConstantBuffer* _matrix_cb;
    Material* _current_material;
    Shader* _current_shader;
    blend_state_t _blend_state;
    BlendState* _current_blend_state;
    depthstencil_state_t _depthstencil_state;
    DepthStencilState* _current_depthstencil_state;
    ConstantBuffer* _lighting_cb;
    lighting_buffer_t _lighting_data;

private:
    KerningFont* GetFont(const std::string& name);
    KerningFont* CreateFontFromXML(const std::string& filepath);

    void CreateMaterialFromFont(const std::string& name, KerningFont* f);
    void CreateAndRegisterDefaultTextures();
    Texture2D* GetTexture(const std::string& filepath);
    Texture2D* CreateTexture(const std::string& filepath,
                             const BufferUsage& bufferUsage = BufferUsage::STATIC,
                             const BufferBindUsage& bindUsage = BufferBindUsage::SHADER_RESOURCE,
                             const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNORM);

    Texture2D* CreateDepthStencil(RHIDevice* owner, unsigned int width, unsigned int height);
    Texture2D* CreateDefaultTexture();
    Texture2D* CreateInvalidTexture();
    Texture2D* CreateDefaultDiffuseTexture();
    Texture2D* CreateDefaultSpecularTexture();
    Texture2D* CreateDefaultEmissiveTexture();
    Texture2D* CreateDefaultNormalTexture();
    Texture2D* CreateDefaultOcclusionTexture();
    Texture2D* CreateDefaultHeightTexture();
    Texture2D* CreateDefaultLightingTexture();

    void CreateAndRegisterDefaultRasterState();
    RasterState* CreateDefaultRasterState();

    void CreateAndRegisterDefaultShaderPrograms();
    ShaderProgram* GetShaderProgram(const std::string& name);
    ShaderProgram* CreateShaderProgram(const std::string& filepath);
    ShaderProgram* CreateDefaultShaderProgram();
    ShaderProgram* CreateUnlitShaderProgram();
    ShaderProgram* CreateColorShaderProgram();
    ShaderProgram* CreateNormalShaderProgram();
    ShaderProgram* CreateSimpleLightingShaderProgram();

    bool RegisterComputeShader(const std::string& name, ComputeShader* computeShader);
    ComputeShader* GetComputeShader(const std::string& name);
    ComputeShader* CreateComputeShader(const std::string& filepath);

    void CreateAndRegisterDefaultSamplers();
    Sampler* CreateDefaultSampler();

    void CreateAndRegisterDefaultShaders();
    Shader* CreateNormalShader();
    Shader* CreateColorShader();
    Shader* CreateDefaultShader();
    Shader* CreateSimpleLightingShader();
    Shader* CreateUnlitShader();
    Shader* Create2dShader();

    void CreateAndRegisterDefaultMaterials();
    Material* CreateMaterial(const std::string& filepath);
    Material* CreateDefaultMaterial();
    Material* CreateInvalidMaterial();
    Material* CreateConsoleMaterial();
    Material* CreateColorMaterial();
    Material* CreateUnlitMaterial();
    Material* CreateNormalMaterial();
    Material* Create2DMaterial();

    MeshSkeleton* CreateSkeleton(const std::string& fbx_path, const Matrix4& initialTransform);

    MeshMotion* CreateMotion(const std::string& fbx_path, MeshSkeleton& meshSkeleton, MeshSkeletonInstance& meshSkeletonInstance, const Matrix4& initialTransform = Matrix4::GetIdentity());
    MeshMotion* GetMotion(const std::string& fbx_path);

    MeshSkeleton* GetSkeleton(const std::string& fbx_path);

    Mesh* GetMesh(const std::string& fbx_path);
    Mesh* CreateMesh(const Model::Type& type, const std::string& fbx_path, const Matrix4& initialTransform = Matrix4::GetIdentity(), MeshSkeleton* skeleton = nullptr);

    Model* GetModel(const std::string& folderpath);
    Model* CreateModel(const Model::Type& type, const std::string& folderpath, const Matrix4& initialTransform = Matrix4::GetIdentity());

    void CalculateUvSphereBuffers(const Vector3& position, const Rgba& color, float radius, unsigned int slices, unsigned int stacks, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo);

    void UpdateVbo(const std::vector<Vertex3D>& new_vbo);
    void UpdateIbo(const std::vector<unsigned int>& new_ibo);

    unsigned int m_windowWidth;
    unsigned int m_windowHeight;

    friend class KerningFont;
    friend class Shader;
    friend class Material;

};

FontJustification operator|(const FontJustification& lhs, const FontJustification& rhs);
FontJustification operator&(const FontJustification& lhs, const FontJustification& rhs);