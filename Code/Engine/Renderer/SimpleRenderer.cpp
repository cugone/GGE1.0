#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/EngineConfig.hpp"
#include "Engine/Display.hpp"
#include "Engine/Window.hpp"

#include "Engine/Core/BitmapFont.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/ComputeShader.hpp"
#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshMotion.hpp"
#include "Engine/Renderer/MeshSkeleton.hpp"
#include "Engine/Renderer/MeshSkeletonInstance.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include "Thirdparty/FBX/fbx.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <locale>
#include <numeric>
#include <sstream>
#include <regex>
#include <type_traits>
#include <utility>

SimpleRenderer::SimpleRenderer(unsigned int width, unsigned int height)
    : _rhi_instance(nullptr)
    , _rhi_context(nullptr)
    , _rhi_device(nullptr)
    , _rhi_output(nullptr)
    , _current_target(nullptr)
    , _current_depthstencil(nullptr)
    , _default_depthstencil(nullptr)
    , _rasters{}
    , _textures{}
    , _samplers{}
    , _fonts{}
    , _shaderPrograms{}
    , _shaders{}
    , _compute_shaders{}
    , _materials{}
    , _meshes{}
    , _motions{}
    , _skeletons{}
    , _models{}
    , _temp_vbo(nullptr)
    , _temp_ibo(nullptr)
    , _current_vbo_size(0)
    , _current_ibo_size(0)
    , _current_material(nullptr)
    , _current_shader(nullptr)
    , _blend_state{}
    , _current_blend_state(nullptr)
    , _depthstencil_state{}
    , _current_depthstencil_state(nullptr)
    , m_windowWidth(width)
    , m_windowHeight(height)
{
    /* DO NOTHING */
}

SimpleRenderer::~SimpleRenderer() {

    delete _current_depthstencil_state;
    _current_depthstencil_state = nullptr;

    delete _current_blend_state;
    _current_blend_state = nullptr;

    _current_target = nullptr;
    _current_shader = nullptr;
    _current_material = nullptr;

    for (auto& f : _fonts) {
        delete f.second;
        f.second = nullptr;
    }
    _fonts.clear();

    for (auto& s : _shaderPrograms) {
        delete s.second;
        s.second = nullptr;
    }
    _shaderPrograms.clear();

    for(auto& cs : _compute_shaders) {
        delete cs.second;
        cs.second = nullptr;
    }
    _compute_shaders.clear();

    for(auto& t : _textures) {
        delete t.second;
        t.second = nullptr;
    }
    _textures.clear();

    for(auto& s : _samplers) {
        delete s.second;
        s.second = nullptr;
    }
    _samplers.clear();

    for (auto& r : _rasters) {
        delete r.second;
        r.second = nullptr;
    }
    _rasters.clear();

    for(auto& m : _materials) {
        delete m.second;
        m.second = nullptr;
    }
    _materials.clear();

    for(auto& s : _shaders) {
        delete s.second;
        s.second = nullptr;
    }
    _shaders.clear();

    for(auto& model : _models) {
        delete model.second;
        model.second = nullptr;
    }
    _models.clear();

    for(auto& motion : _motions) {
        delete motion.second;
        motion.second = nullptr;
    }
    _motions.clear();

    for(auto& skeleton : _skeletons) {
        delete skeleton.second;
        skeleton.second = nullptr;
    }
    _skeletons.clear();

    for(auto& mesh : _meshes) {
        delete mesh.second;
        mesh.second = nullptr;
    }
    _meshes.clear();

    delete _lighting_cb;
    _lighting_cb = nullptr;

    delete _time_cb;
    _time_cb = nullptr;

    delete _matrix_cb;
    _matrix_cb = nullptr;

    delete _temp_vbo;
    _temp_vbo = nullptr;

    delete _temp_ibo;
    _temp_ibo = nullptr;

    delete _default_depthstencil;
    _default_depthstencil = nullptr;

    delete _rhi_output;
    _rhi_output = nullptr;

    RHIInstance::DestroyInstance();
    _rhi_instance = nullptr;

}
void SimpleRenderer::Initialize() {
    _rhi_instance = RHIInstance::CreateInstance();
    _rhi_output = _rhi_instance->CreateOutput(IntVector2(m_windowWidth, m_windowHeight));
    _rhi_device = _rhi_output->GetParentDevice();
    _rhi_context = _rhi_device->GetImmediateContext();

    _default_depthstencil = CreateDepthStencil(_rhi_device, m_windowWidth, m_windowHeight);
    SetRenderTarget(nullptr, _default_depthstencil);

    std::vector<Vertex3D> verts(1024);
    std::vector<unsigned int> indicies(1024);
    _current_vbo_size = 1024;
    _current_ibo_size = 1024;
    _temp_vbo = _rhi_device->CreateVertexBuffer(verts, BufferUsage::DYNAMIC, BufferBindUsage::VERTEX_BUFFER);
    _temp_ibo = _rhi_device->CreateIndexBuffer(indicies, BufferUsage::DYNAMIC, BufferBindUsage::INDEX_BUFFER);
    
    _time_data.game_frame_time = 0.0f;
    _time_data.game_time = 0.0f;
    _time_data.system_frame_time = 0.0f;
    _time_data.system_time = 0.0f;

    _matrix_cb = _rhi_device->CreateConstantBuffer(&_matrix_data, sizeof(_matrix_data), BufferUsage::DYNAMIC, BufferBindUsage::CONSTANT_BUFFER);
    _rhi_context->SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);

    _time_cb = _rhi_device->CreateConstantBuffer(&_time_data, sizeof(_time_data), BufferUsage::DYNAMIC, BufferBindUsage::CONSTANT_BUFFER);
    _rhi_context->SetConstantBuffer(TIME_BUFFER_INDEX, _time_cb);

    _lighting_cb = _rhi_device->CreateConstantBuffer(&_lighting_data, sizeof(_lighting_data), BufferUsage::DYNAMIC, BufferBindUsage::CONSTANT_BUFFER);
    _rhi_context->SetConstantBuffer(LIGHTING_BUFFER_INDEX, _lighting_cb);

    CreateAndRegisterDefaultRasterState();
    CreateAndRegisterDefaultSamplers();
    CreateAndRegisterDefaultTextures();
    CreateAndRegisterDefaultShaderPrograms();
    CreateAndRegisterDefaultShaders();
    CreateAndRegisterDefaultMaterials();
}

void SimpleRenderer::CreateAndRegisterDefaultRasterState() {
    RasterState* default_raster_state = CreateDefaultRasterState();
    RegisterRasterState("__default", default_raster_state);
    SetRasterState(default_raster_state);

    RasterState* wireframe_raster_state = new RasterState(_rhi_device, FillMode::WIREFRAME, CullMode::BACK, true);
    RegisterRasterState("__wireframe", wireframe_raster_state);

    RasterState* solidnoculling_raster_state = new RasterState(_rhi_device, FillMode::SOLID, CullMode::NONE, true);
    RegisterRasterState("__noculling", solidnoculling_raster_state);

    RasterState* wireframenoculling_raster_state = new RasterState(_rhi_device, FillMode::WIREFRAME, CullMode::NONE, true);
    RegisterRasterState("__nocullingwireframe", wireframenoculling_raster_state);
}

RasterState* SimpleRenderer::CreateDefaultRasterState() {
    return new RasterState(_rhi_device, FillMode::SOLID, CullMode::BACK, true);
}

void SimpleRenderer::CreateAndRegisterDefaultSamplers() {
    Sampler* default_sampler = CreateDefaultSampler();
    RegisterSampler("__default", default_sampler);

    SetSampler(default_sampler);
}

void SimpleRenderer::CreateAndRegisterDefaultShaderPrograms() {
    ShaderProgram* default_shader = CreateDefaultShaderProgram();
    RegisterShaderProgram(default_shader->GetName(), default_shader);
    SetShaderProgram(nullptr); //Defaults to "__default"

    ShaderProgram* color_shader = CreateColorShaderProgram();
    RegisterShaderProgram(color_shader->GetName(), color_shader);

    ShaderProgram* unlit_shader = CreateUnlitShaderProgram();
    RegisterShaderProgram(unlit_shader->GetName(), unlit_shader);

    ShaderProgram* normal_shader = CreateNormalShaderProgram();
    RegisterShaderProgram(normal_shader->GetName(), normal_shader);

    ShaderProgram* simplelighting_shader = CreateSimpleLightingShaderProgram();
    RegisterShaderProgram(simplelighting_shader->GetName(), simplelighting_shader);

}

void SimpleRenderer::CreateAndRegisterDefaultTextures() {
    Texture2D* default_texture = CreateDefaultTexture();
    RegisterTexture("__default", default_texture);

    Texture2D* invalid_texture = CreateInvalidTexture();
    RegisterTexture("__invalid", invalid_texture);

    Texture2D* default_diffuse = CreateDefaultDiffuseTexture();
    RegisterTexture("__diffuse", default_diffuse);

    Texture2D* default_specular = CreateDefaultSpecularTexture();
    RegisterTexture("__specular", default_specular);

    Texture2D* default_emissive = CreateDefaultEmissiveTexture();
    RegisterTexture("__emissive", default_emissive);

    Texture2D* default_normal = CreateDefaultNormalTexture();
    RegisterTexture("__normal", default_normal);

    Texture2D* default_occlusion = CreateDefaultOcclusionTexture();
    RegisterTexture("__occlusion", default_occlusion);

    Texture2D* default_height = CreateDefaultHeightTexture();
    RegisterTexture("__height", default_height);

    Texture2D* default_lighting = CreateDefaultLightingTexture();
    RegisterTexture("__lighting", default_lighting);

}

void SimpleRenderer::CreateAndRegisterDefaultShaders() {
    Shader* shader = CreateDefaultShader();
    RegisterShader(shader->GetName(), shader);

    Shader* color = CreateColorShader();
    RegisterShader(color->GetName(), color);

    Shader* unlit = CreateUnlitShader();
    RegisterShader(unlit->GetName(), unlit);

    Shader* normal = CreateNormalShader();
    RegisterShader(normal->GetName(), normal);

    Shader* simpleLighting = CreateSimpleLightingShader();
    RegisterShader(simpleLighting->GetName(), simpleLighting);

    Shader* shader2D = Create2dShader();
    RegisterShader(shader2D->GetName(), shader2D);

}

Shader* SimpleRenderer::CreateNormalShader() {
    std::string str =
        R"(
<shader name="__normal">
    <shaderprogram src="__normal" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed shader file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Shader(this, *doc.RootElement());
}

Shader* SimpleRenderer::CreateColorShader() {
std::string str =
R"(
<shader name="__color">
    <shaderprogram src="__color" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed shader file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Shader(this, *doc.RootElement());
}

Shader* SimpleRenderer::CreateDefaultShader() {
    std::string str =
        R"(
<shader name="__default">
    <shaderprogram src="__default" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed shader file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Shader(this, *doc.RootElement());
}

Shader* SimpleRenderer::CreateSimpleLightingShader() {
std::string str =
R"(
<shader name="__simpleLighting">
    <shaderprogram src="__simpleLighting" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed shader file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Shader(this, *doc.RootElement());
}

Shader* SimpleRenderer::CreateUnlitShader() {
std::string str =
R"(
<shader name="__unlit">
    <shaderprogram src="__unlit" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed shader file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Shader(this, *doc.RootElement());
}

Shader* SimpleRenderer::Create2dShader() {
    std::string str =
        R"(
<shader name="__2D">
    <shaderprogram src="__unlit" />
    <raster>
        <fill>solid</fill>
        <cull>none</cull>
        <antialiasing>true</antialiasing>
    </raster>
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
    <depth enable="false" writable="false" />
    <stencil enable="false" readable="false" writable="false" />
</shader>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed shader file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Shader(this, *doc.RootElement());
}


bool SimpleRenderer::RegisterShader(const std::string& name, Shader* shader) {
    auto shader_iter = _shaders.find(name);
    bool shader_exists = shader_iter != _shaders.end();
    if(shader_exists) {
        delete shader_iter->second;
        shader_iter->second = nullptr;
        if(shader == nullptr) {
            return false;
        }
    }
    _shaders.insert_or_assign(name, shader);
    return true;
}

bool SimpleRenderer::RegisterShaderFromFile(const std::string& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);

    tinyxml2::XMLDocument doc;
    auto load_result = doc.LoadFile(p.string().c_str());
    bool success = load_result == tinyxml2::XML_SUCCESS;
    if(success) {
        Shader* shader = new Shader(this, *doc.RootElement());
        RegisterShader(p.string(), shader);
        return success;
    }
    return success;
}

bool SimpleRenderer::RegisterShadersFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    { //Avoid pollution by error code.
        std::error_code ec;
        if(!FS::exists(folderpath) || !FS::is_directory(p, ec)) {
            std::ostringstream ss;
            ss << "Shader::RegisterShadersFromFolder: \"" << folderpath << "\" does not exist or is not a directory. Filesystem reported the following error: " << ec.message() << "\n";
            g_theFileLogger->LogErrorf(ss.str().c_str());
            g_theFileLogger->LogFlush();
            ERROR_AND_DIE(ss.str().c_str());
        }
    }
    if(!recursive) {
        for(FS::directory_iterator dir_iter(p); dir_iter != FS::directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(FS::is_directory(current_path)) continue;
            if(current_path.extension() != ".shader") continue;
            if(!RegisterShaderFromFile(current_path.string())) {
                return false;
            }
        }
        return true;
    } else {
        for(FS::recursive_directory_iterator dir_iter(p); dir_iter != FS::recursive_directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(current_path.extension() != ".shader") continue;
            if(!RegisterShaderFromFile(current_path.string())) {
                return false;
            }
        }
        return true;
    }
}

void SimpleRenderer::CreateAndRegisterDefaultMaterials() {
    Material* mat = CreateDefaultMaterial();
    RegisterMaterial(mat->GetName(), mat);

    Material* invalidMaterial = CreateInvalidMaterial();
    RegisterMaterial(invalidMaterial->GetName(), invalidMaterial);

    Material* colorMaterial = CreateColorMaterial();
    RegisterMaterial(colorMaterial->GetName(), colorMaterial);

    Material* unlitMaterial = CreateUnlitMaterial();
    RegisterMaterial(unlitMaterial->GetName(), unlitMaterial);

    Material* normalMaterial = CreateNormalMaterial();
    RegisterMaterial(normalMaterial->GetName(), normalMaterial);

    Material* consoleMaterial = CreateConsoleMaterial();
    RegisterMaterial(consoleMaterial->GetName(), consoleMaterial);

    Material* material2D = Create2DMaterial();
    RegisterMaterial(material2D->GetName(), material2D);

}

Material* SimpleRenderer::CreateDefaultMaterial() {
    std::string str =
        R"(
<material name="__default">
    <shader src="__default" />
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());

}

Material* SimpleRenderer::CreateNormalMaterial() {
    std::string str =
        R"(
<material name="__normal">
    <shader src="__normal" />
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());

}

Material* SimpleRenderer::CreateInvalidMaterial() {
    std::string str =
        R"(
<material name="__invalid">
    <shader src="__unlit" />
    <textures>
        <diffuse src="__invalid" />
    </textures>
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());
}

Material* SimpleRenderer::CreateConsoleMaterial() {
    std::string str =
        R"(
<material name="__console">
    <shader src="__2D" />
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());
}

Material* SimpleRenderer::Create2DMaterial() {
    std::string str =
        R"(
<material name="__2D">
    <shader src="__2D" />
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());
}

Material* SimpleRenderer::CreateUnlitMaterial() {
    std::string str =
        R"(
<material name="__unlit">
    <shader src="__unlit" />
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());
}

Material* SimpleRenderer::CreateColorMaterial() {
    std::string str =
        R"(
<material name="__color">
    <shader src="__unlit" />
    <textures>
        <texture index="0" src="__default" />
    </textures>
</material>
)";

    tinyxml2::XMLDocument doc;
    if(doc.Parse(str.c_str()) != tinyxml2::XML_SUCCESS) {
        std::ostringstream ss;
        ss << "ill-formed material file. XML parser returned error: " << doc.ErrorName() << '\n';
        ERROR_AND_DIE(ss.str());
    }
    return new Material(this, *doc.RootElement());
}
void SimpleRenderer::BeginFrame() {
    /* DO NOTHING */
}
void SimpleRenderer::EndFrame() {
    /* DO NOTHING */
}
Texture2D* SimpleRenderer::CreateDepthStencil(RHIDevice* owner, unsigned int width, unsigned int height) {
    ID3D11Texture2D* dx_resource = nullptr;

    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //for read/write
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; //for read/write
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT texture_hr = owner->GetDxDevice()->CreateTexture2D(&descDepth, nullptr, &dx_resource);
    bool texture_creation_succeeded = SUCCEEDED(texture_hr);
    if (texture_creation_succeeded) {
        if (_current_depthstencil) {
            delete _current_depthstencil;
            _current_depthstencil = nullptr;
        }
        if (_current_depthstencil_state) {
            delete _current_depthstencil_state;
            _current_depthstencil_state = nullptr;
        }
        _current_depthstencil = new Texture2D(owner, dx_resource);
        _current_depthstencil_state = new DepthStencilState(owner);
        return _current_depthstencil;
    }
    else {
        g_theFileLogger->LogErrorf("Depth Buffer Resource failed to create.");
        g_theFileLogger->LogFlush();
        ERROR_AND_DIE("Depth Buffer Resource failed to create.");
    }
}


/************************************************************************/
/* DEFAULT TEXTURES                                                     */
/************************************************************************/
Texture2D* SimpleRenderer::CreateDefaultTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}

Texture2D* SimpleRenderer::CreateDefaultDiffuseTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}
Texture2D* SimpleRenderer::CreateDefaultSpecularTexture() {
    std::vector<Rgba> data = {
        Rgba::BLACK
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}
Texture2D* SimpleRenderer::CreateDefaultEmissiveTexture() {
    std::vector<Rgba> data = {
        Rgba::BLACK
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}
Texture2D* SimpleRenderer::CreateDefaultNormalTexture() {
    std::vector<Rgba> data = {
        Rgba::NORMAL_Z
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}
Texture2D* SimpleRenderer::CreateDefaultOcclusionTexture() {
    std::vector<Rgba> data = {
        Rgba::WHITE
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}
Texture2D* SimpleRenderer::CreateDefaultHeightTexture() {
    std::vector<Rgba> data = {
        Rgba::GREY
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}

Texture2D* SimpleRenderer::CreateDefaultLightingTexture() {
    std::vector<Rgba> data = {
        Rgba(0, 0, 0, 255)
    };
    return CreateTexture2DFromMemory(data, 1, 1);
}

/************************************************************************/
/* INVALID TEXTURE                                                      */
/************************************************************************/
Texture2D* SimpleRenderer::CreateInvalidTexture() {
    std::vector<Rgba> data = {
        Rgba::MAGENTA, Rgba::BLACK,
        Rgba::BLACK,   Rgba::MAGENTA,
    };
    return CreateTexture2DFromMemory(data, 2, 2);
}

/************************************************************************/
/* DEFAULT SHADER                                                       */
/************************************************************************/
ShaderProgram* SimpleRenderer::CreateDefaultShaderProgram() {
    std::string program =
        R"(

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);


SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
}

cbuffer time_cb : register(b1) {
    float g_GAME_TIME;
    float g_SYSTEM_TIME;
    float g_GAME_FRAME_TIME;
    float g_SYSTEM_FRAME_TIME;
}

struct point_light
{
    float4 position;
    float4 color;
    float4 attenuation;
    float4 specAttenuation;
};

struct directional_light
{
    point_light light;
    float4 direction;
};

cbuffer lighting_cb : register(b2) {
    point_light g_pointLights[8];
    directional_light g_directionalLights[8];
    float4 g_lightAmbient;
    float4 g_lightSpecFactorPower;
    float4 g_lightEyePosition;
}

struct vs_input_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct v2f_t {

    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 world_position : WORLD;
};

float4 NormalAsColor(float3 n) {
    return float4((n + 1.0f) * 0.5f, 1.0f);
}

v2f_t VertexFunction(vs_input_t input) {
    v2f_t v2f = (v2f_t)0;

    float4 local = float4(input.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    v2f.position = clip;
    v2f.normal = mul(input.normal, (float3x3)g_MODEL).xyz;
    v2f.color = input.color;
    v2f.uv = input.uv;
    v2f.tangent = mul(input.tangent, (float3x3)g_MODEL).xyz;
    v2f.bitangent = mul(input.bitangent, (float3x3)g_MODEL).xyz;
    v2f.world_position = world.xyz;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{

    float4 albedo = tImage.Sample(sSampler, input.uv);
    float4 diffuse = albedo * input.color;
    
    float3 normal_as_color = tNormal.Sample(sSampler, input.uv).xyz;
    float3 pixel_normal = normalize(input.normal);

    float3 tangent = input.tangent;
    float3 bitangent = input.bitangent;
    tangent = normalize(cross(pixel_normal, bitangent));

    float3x3 tbn = float3x3(tangent, bitangent, pixel_normal);

    float3 surface_normal = normal_as_color * float3(2.0f, 2.0f, 1.0) - float3(1.0f, 1.0f, 0.0f);
    float3 world_normal = mul(surface_normal, tbn);
    world_normal = normalize(world_normal);

    float3 vector_to_eye = g_lightEyePosition.xyz - input.world_position;
    float3 eye_vector = -normalize(vector_to_eye);

    float ambient_occlusion_map_factor = tLighting.Sample(sSampler, input.uv).a;
    float4 ambient_factor = float4(g_lightAmbient.xyz * g_lightAmbient.w, 1.0f) * ambient_occlusion_map_factor;
    
    float4 light_factor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular_color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int point_index = 0; point_index < 8; point_index += 1) {

        float4 pos = g_pointLights[point_index].position;
        float4 color = g_pointLights[point_index].color;
        float4 att = g_pointLights[point_index].attenuation;
        float4 specAtt = g_pointLights[point_index].specAttenuation;

        float3 vector_to_light = pos.xyz - input.world_position.xyz;
    
        float distance_to_light = length(vector_to_light);
        float3 dir_to_light = vector_to_light / distance_to_light;
        float dot3 = saturate(dot(dir_to_light, world_normal));
        float attenuation = color.w / (att.x +
                        distance_to_light * att.y +
                        distance_to_light * distance_to_light * att.z);
    
        attenuation = saturate(attenuation);

        float4 light_color = float4(color.xyz, 1.0f);
        light_factor += light_color * dot3 * attenuation;
    
        float spec_attenuation = color.w / (specAtt.x + distance_to_light * specAtt.y + distance_to_light * distance_to_light * specAtt.z);
        float3 ref_light_dir = reflect(dir_to_light, world_normal);
        float spec_dot3 = saturate(dot(ref_light_dir, eye_vector));
        float spec_factor = spec_attenuation * g_lightSpecFactorPower.x * pow(spec_dot3, g_lightSpecFactorPower.y);
        float4 spec_color = spec_factor * light_color;
        specular_color += spec_color;
    }

    for (int directional_index = 0; directional_index < 8; directional_index += 1) {

        float4 pos = g_directionalLights[directional_index].light.position;
        float4 color = g_directionalLights[directional_index].light.color;
        float4 att = g_directionalLights[directional_index].light.attenuation;
        float4 specAtt = g_directionalLights[directional_index].light.specAttenuation;
        float4 direction = g_directionalLights[directional_index].direction;
        direction = normalize(direction);
        float3 vector_to_light = pos.xyz - input.world_position;
    
        float distance_to_light = length(vector_to_light);
        float3 dir_to_light = -(direction.xyz);
        float dot3 = saturate(dot(dir_to_light, world_normal));
        float attenuation = color.w / (att.x +
                        distance_to_light * att.y +
                        distance_to_light * distance_to_light * att.z);
    
        attenuation = saturate(attenuation);

        float4 light_color = float4(color.xyz, 1.0f);
        light_factor += light_color * dot3 * attenuation;
    
        float spec_attenuation = color.w / (specAtt.x + distance_to_light * specAtt.y + distance_to_light * distance_to_light * specAtt.z);
        float3 ref_light_dir = reflect(dir_to_light, world_normal);
        float spec_dot3 = saturate(dot(ref_light_dir, eye_vector));
        float spec_factor = spec_attenuation * g_lightSpecFactorPower.x * pow(spec_dot3, g_lightSpecFactorPower.y);
        float4 spec_color = spec_factor * light_color;
        specular_color += spec_color;
    }

    float4 diffuse_factor = saturate(ambient_factor + light_factor);
    float4 emissive_color = tEmissive.Sample(sSampler, input.uv);
    float4 specular_map_factor = tLighting.Sample(sSampler, input.uv).r;
    float4 final_color = (diffuse_factor * diffuse) + (specular_color * specular_map_factor) + emissive_color;
    return final_color;
}

)";
    return _rhi_device->CreateShaderFromHlslString("__default", program);
}

/************************************************************************/
/* UNLIT SHADER                                                         */
/************************************************************************/
ShaderProgram* SimpleRenderer::CreateUnlitShaderProgram() {
    std::string program =
        R"(

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);

SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
}

cbuffer time_cb : register(b1) {
    float g_GAME_TIME;
    float g_SYSTEM_TIME;
    float g_GAME_FRAME_TIME;
    float g_SYSTEM_FRAME_TIME;
}

cbuffer lighting_cb : register(b2) {
    float4 g_lightPos;
    float4 g_lightDir;
    float4 g_lightAmbient;
    float4 g_lightColor;
    float4 g_lightAttenuation;
}

struct vs_input_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
};

struct v2f_t {

    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
};

float4 NormalAsColor(float3 n) {
    return float4((n + 1.0f) * 0.5f, 1.0f);
}

v2f_t VertexFunction(vs_input_t input) {
    v2f_t v2f = (v2f_t)0;

    float4 local = float4(input.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    v2f.position = clip;
    v2f.color = input.color;
    v2f.uv = input.uv;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{
    float4 albedo = tImage.Sample(sSampler, input.uv);
    return albedo * input.color;
}
)";
    return _rhi_device->CreateShaderFromHlslString("__unlit", program);
}

/************************************************************************/
/* SIMPLE SHADER                                                        */
/************************************************************************/
ShaderProgram* SimpleRenderer::CreateSimpleLightingShaderProgram() {
    std::string program =
        R"(

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);

SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
}

cbuffer time_cb : register(b1) {
    float g_GAME_TIME;
    float g_SYSTEM_TIME;
    float g_GAME_FRAME_TIME;
    float g_SYSTEM_FRAME_TIME;
}

struct point_light
{
    float4 position;
    float4 color;
    float4 attenuation;
    float4 specAttenuation;
};

struct directional_light
{
    point_light light;
    float4 direction;
};

cbuffer lighting_cb : register(b2) {
    point_light g_pointLights[8];
    directional_light g_directionalLights[8];
    float4 g_lightAmbient;
    float4 g_lightSpecFactorPower;
    float4 g_lightEyePosition;
}

struct vs_input_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct v2f_t {

    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 world_position : WORLD;
};

float4 NormalAsColor(float3 n) {
    return float4((n + 1.0f) * 0.5f, 1.0f);
}

v2f_t VertexFunction(vs_input_t input) {
    v2f_t v2f = (v2f_t)0;

    float4 local = float4(input.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    v2f.position = clip;
    v2f.normal = mul(input.normal, (float3x3)g_MODEL).xyz;
    v2f.color = input.color;
    v2f.uv = input.uv;
    v2f.tangent = mul(input.tangent, (float3x3)g_MODEL).xyz;
    v2f.bitangent = mul(input.bitangent, (float3x3)g_MODEL).xyz;
    v2f.world_position = world.xyz;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{

    float4 albedo = tImage.Sample(sSampler, input.uv);
    float4 diffuse = albedo * input.color;
    
    //float3 normal_as_color = tNormal.Sample(sSampler, input.uv).xyz;
    float3 pixel_normal = normalize(input.normal);

    float3 tangent = input.tangent;
    float3 bitangent = input.bitangent;
    tangent = normalize(cross(pixel_normal, bitangent));

    //float3x3 tbn = float3x3(tangent, bitangent, pixel_normal);

    float3 surface_normal = input.normal;// * float3(2.0f, 2.0f, 1.0) - float3(1.0f, 1.0f, 0.0f);
    float3 world_normal = surface_normal; //mul(surface_normal, input.normal);
    world_normal = normalize(world_normal);

    float3 vector_to_eye = g_lightEyePosition.xyz - input.world_position;
    float3 eye_vector = -normalize(vector_to_eye);

    //float ambient_occlusion_map_factor = tLighting.Sample(sSampler, input.uv).a;
    float4 ambient_factor = float4(g_lightAmbient.xyz * g_lightAmbient.w, 1.0f);// * ambient_occlusion_map_factor;
    
    float4 light_factor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular_color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int point_index = 0; point_index < 8; point_index += 1) {

        float4 pos = g_pointLights[point_index].position;
        float4 color = g_pointLights[point_index].color;
        float4 att = g_pointLights[point_index].attenuation;
        float4 specAtt = g_pointLights[point_index].specAttenuation;

        float3 vector_to_light = pos.xyz - input.world_position.xyz;
    
        float distance_to_light = length(vector_to_light);
        float3 dir_to_light = vector_to_light / distance_to_light;
        float dot3 = saturate(dot(dir_to_light, world_normal));
        float attenuation = color.w / (att.x +
                        distance_to_light * att.y +
                        distance_to_light * distance_to_light * att.z);
    
        attenuation = saturate(attenuation);

        float4 light_color = float4(color.xyz, 1.0f);
        light_factor += light_color * dot3 * attenuation;
    
        float spec_attenuation = color.w / (specAtt.x + distance_to_light * specAtt.y + distance_to_light * distance_to_light * specAtt.z);
        float3 ref_light_dir = reflect(dir_to_light, world_normal);
        float spec_dot3 = saturate(dot(ref_light_dir, eye_vector));
        float spec_factor = spec_attenuation * g_lightSpecFactorPower.x * pow(spec_dot3, g_lightSpecFactorPower.y);
        float4 spec_color = spec_factor * light_color;
        specular_color += spec_color;
    }

    for (int directional_index = 0; directional_index < 8; directional_index += 1) {

        float4 pos = g_directionalLights[directional_index].light.position;
        float4 color = g_directionalLights[directional_index].light.color;
        float4 att = g_directionalLights[directional_index].light.attenuation;
        float4 specAtt = g_directionalLights[directional_index].light.specAttenuation;
        float4 direction = g_directionalLights[directional_index].direction;
        direction = normalize(direction);
        float3 vector_to_light = pos.xyz - input.world_position;
    
        float distance_to_light = length(vector_to_light);
        float3 dir_to_light = -(direction.xyz);
        float dot3 = saturate(dot(dir_to_light, world_normal));
        float attenuation = color.w / (att.x +
                        distance_to_light * att.y +
                        distance_to_light * distance_to_light * att.z);
    
        attenuation = saturate(attenuation);

        float4 light_color = float4(color.xyz, 1.0f);
        light_factor += light_color * dot3 * attenuation;
    
        float spec_attenuation = color.w / (specAtt.x + distance_to_light * specAtt.y + distance_to_light * distance_to_light * specAtt.z);
        float3 ref_light_dir = reflect(dir_to_light, world_normal);
        float spec_dot3 = saturate(dot(ref_light_dir, eye_vector));
        float spec_factor = spec_attenuation * g_lightSpecFactorPower.x * pow(spec_dot3, g_lightSpecFactorPower.y);
        float4 spec_color = spec_factor * light_color;
        specular_color += spec_color;
    }

    float4 diffuse_factor = saturate(ambient_factor + light_factor);
    float4 emissive_color = tEmissive.Sample(sSampler, input.uv);
    float4 specular_map_factor = tLighting.Sample(sSampler, input.uv).r;
    float4 final_color = (diffuse_factor * diffuse) + (specular_color * specular_map_factor) + emissive_color;
    return final_color;
}
)";
    return _rhi_device->CreateShaderFromHlslString("__simpleLighting", program);
}

/************************************************************************/
/* COLOR SHADER                                                         */
/************************************************************************/
ShaderProgram* SimpleRenderer::CreateColorShaderProgram() {
    std::string program =
        R"(

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);

SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
}

cbuffer time_cb : register(b1) {
    float g_GAME_TIME;
    float g_SYSTEM_TIME;
    float g_GAME_FRAME_TIME;
    float g_SYSTEM_FRAME_TIME;
}

cbuffer lighting_cb : register(b2) {
    float4 g_lightPos;
    float4 g_lightDir;
    float4 g_lightAmbient;
    float4 g_lightColor;
    float4 g_lightAttenuation;
}

struct vs_input_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
};

struct v2f_t {
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
};

v2f_t VertexFunction(vs_input_t input) {
    v2f_t v2f = (v2f_t)0;

    float4 local = float4(input.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    v2f.position = clip;
    v2f.color = input.color;
    v2f.uv = input.uv;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{
    return input.color;
}
)";
    return _rhi_device->CreateShaderFromHlslString("__color", program);
}

/************************************************************************/
/* NORMAL SHADER                                                        */
/************************************************************************/
ShaderProgram* SimpleRenderer::CreateNormalShaderProgram() {
    std::string program =
        R"(

Texture2D<float4> tImage    : register(t0);
Texture2D<float4> tNormal   : register(t1);
Texture2D<float4> tLighting : register(t2);
Texture2D<float4> tSpecular : register(t3);
Texture2D<float4> tOcclusion : register(t4);
Texture2D<float4> tEmissive : register(t5);

SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
    float4x4 g_MODEL;
    float4x4 g_VIEW;
    float4x4 g_PROJECTION;
}

cbuffer time_cb : register(b1) {
    float g_GAME_TIME;
    float g_SYSTEM_TIME;
    float g_GAME_FRAME_TIME;
    float g_SYSTEM_FRAME_TIME;
}

cbuffer lighting_cb : register(b2) {
    float4 g_lightPos;
    float4 g_lightDir;
    float4 g_lightAmbient;
    float4 g_lightColor;
    float4 g_lightAttenuation;
}

struct vs_input_t {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
};

struct v2f_t {
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
};

v2f_t VertexFunction(vs_input_t input) {
    v2f_t v2f = (v2f_t)0;

    float4 local = float4(input.position, 1.0f);
    float4 world = mul(local, g_MODEL);
    float4 view = mul(world, g_VIEW);
    float4 clip = mul(view, g_PROJECTION);

    v2f.position = clip;
    v2f.color = input.color;
    v2f.uv = input.uv;
    v2f.normal = input.normal;

    return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{
    return (input.normal + float4(1.0f, 1.0f, 1.0f, 1.0f)) * 0.5f;
}
)";
    return _rhi_device->CreateShaderFromHlslString("__normal", program);
}

Sampler* SimpleRenderer::CreateDefaultSampler() {
    return new Sampler(_rhi_device);
}

Material* SimpleRenderer::CreateOrGetMaterial(const std::string& material_str) {
    auto material_iter = _materials.find(material_str);
    auto material_exists = material_iter != _materials.end();
    if(material_exists) {
        return GetMaterial(material_str);
    } else {
        return CreateMaterial(material_str);
    }
}

Material* SimpleRenderer::CreateMaterial(const std::string& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(p.extension() != ".material") {
        return nullptr;
    }
    tinyxml2::XMLDocument doc;
    auto load_result = doc.LoadFile(p.string().c_str());
    if(load_result == tinyxml2::XML_SUCCESS) {
        return new Material(this, *doc.RootElement());
    } else {
        return GetMaterial("__invalid");
    }
}

Material* SimpleRenderer::GetMaterial(const std::string& name) {
    namespace FS = std::experimental::filesystem;
    FS::path p(name);
    return _materials[p.string()];
}

void SimpleRenderer::SetMaterial(Material* mat /*= nullptr*/) {
    if(mat == nullptr) {
        mat = GetMaterial("__default");
    }
    if(_current_material == mat) {
        return;
    }
    _rhi_context->SetMaterial(mat);
    _current_material = mat;
}

void SimpleRenderer::SetTitle(const std::string& title) {
    _rhi_output->GetWindow()->SetTitle(title);
}

void SimpleRenderer::SetRenderTarget(Texture2D* color_target, Texture2D* depthstencil_target) {
    if(color_target != nullptr) {
        _current_target = color_target;
    } else {
        _current_target = _rhi_output->GetBackBuffer();
    }
    
    ID3D11DepthStencilView* dsv = nullptr;
    if (depthstencil_target != nullptr) {
        _current_depthstencil = _default_depthstencil;
    }
    dsv = _current_depthstencil->GetDepthStencilView();
    ID3D11RenderTargetView* rtv = _current_target->GetRenderTargetView();

    _rhi_context->GetDxContext()->OMSetRenderTargets(1, &rtv, dsv);
}

void SimpleRenderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    D3D11_VIEWPORT viewport;
    memset(&viewport, 0, sizeof(viewport));

    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    _rhi_context->GetDxContext()->RSSetViewports(1, &viewport);
}

void SimpleRenderer::SetViewportAsPercent(float /*x*/, float /*y*/, float /*width*/, float /*height*/) {
    /* DO NOTHING */
}

void SimpleRenderer::ClearColor(const Rgba& color) {
    _rhi_context->ClearColorTarget(_current_target, color);
}
void SimpleRenderer::ClearTargetColor(Texture2D *target, const Rgba& color) {
    _rhi_context->ClearColorTarget(target, color);
}

void SimpleRenderer::ClearDepthStencilBuffer() {
    _rhi_context->ClearDepthStencilTarget(_current_depthstencil);
}

void SimpleRenderer::Present(bool vsync /*= false*/) {
    _rhi_output->Present(vsync);
}

void SimpleRenderer::SetShaderProgram(ShaderProgram* program /*= nullptr*/) {
    if(program == nullptr) {
        program = GetShaderProgram("__default");
    }
    _rhi_context->SetShaderProgram(program);
}

ShaderProgram* SimpleRenderer::GetShaderProgram(const std::string& name) {
    return _shaderPrograms[name];
}

void SimpleRenderer::SetShader(Shader* shader /*= nullptr*/) {
    if(shader == nullptr) {
        shader = GetShader("__default");
    }
    if(_current_shader == shader) {
        return;
    }
    _current_shader = shader;
    _rhi_context->SetShader(_current_shader);
}

Shader* SimpleRenderer::GetShader(const std::string& name) {
    namespace FS = std::experimental::filesystem;
    FS::path p(name);
    return _shaders[p.string()];
}

void SimpleRenderer::SetComputeShader(ComputeShader* shader /*= nullptr*/) {
    if(shader == nullptr) {
        shader = GetComputeShader("__default");
    }
    _rhi_context->SetComputeShader(shader);
}
void SimpleRenderer::SetConstantBuffer(unsigned int index, ConstantBuffer* constant_buffer) {
    ID3D11Buffer* const b = constant_buffer->GetDxBuffer();
    _rhi_context->GetDxContext()->VSSetConstantBuffers(index, 1, &b);
    _rhi_context->GetDxContext()->PSSetConstantBuffers(index, 1, &b);
}

void SimpleRenderer::DrawDebugAABB2(const AABB2& box, const Rgba& edgeColor, const Rgba& fillColor) {

    //Verts
    Vector3 bl{ box.mins.x, box.maxs.y, 0.0f };
    Vector3 tl{ box.mins, 0.0f };
    Vector3 tr{ box.maxs.x, box.mins.y, 0.0f };
    Vector3 br{ box.maxs, 0.0f };

    DrawQuad(bl, Vector3{ { tr }, 1.0f }, fillColor);

    DrawDebugLine2D(bl, tl, 1.0f, edgeColor, edgeColor);
    DrawDebugLine2D(tl, tr, 1.0f, edgeColor, edgeColor);
    DrawDebugLine2D(tr, br, 1.0f, edgeColor, edgeColor);
    DrawDebugLine2D(br, bl, 1.0f, edgeColor, edgeColor);


}

void SimpleRenderer::DrawQuad(const Vector3& leftBottomNear, const Vector3& rightTopFar, const Rgba& color, const Vector4& lbrtUV /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/) {

    //Components
    float l = leftBottomNear.x;
    float b = leftBottomNear.y;
    float n = leftBottomNear.z;
    float r = rightTopFar.x;
    float t = rightTopFar.y;
    float f = rightTopFar.z;

    //Verts
    //Vector3 leftBottomNear = Vector3(l, b, n);
    Vector3 leftTopNear = Vector3(l, t, n);
    //Vector3 rightTopFar = Vector3(r, t, f);
    Vector3 rightBottomFar = Vector3(r, b, f);

    //Edges
    Vector3 bottom = Vector3(rightBottomFar - leftBottomNear);
    Vector3 left = Vector3(leftTopNear - leftBottomNear);
    Vector3 top = Vector3(rightTopFar - leftTopNear);
    Vector3 right = Vector3(rightBottomFar - rightTopFar);

    //TBN
    Vector3 tangent = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 bitangent = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 normal = CrossProduct(bottom, left);

    //UV components
    Vector2 lbnUV = Vector2(lbrtUV.x, lbrtUV.y);
    Vector2 ltnUV = Vector2(lbrtUV.x, lbrtUV.w);
    Vector2 rtfUV = Vector2(lbrtUV.z, lbrtUV.w);
    Vector2 rbfUV = Vector2(lbrtUV.z, lbrtUV.y);

    std::vector<Vertex3D> vbo = {
        Vertex3D(Vector3(l, b, n), color, lbnUV, normal, tangent, bitangent),
        Vertex3D(Vector3(l, t, n), color, ltnUV, normal, tangent, bitangent),
        Vertex3D(Vector3(r, t, f), color, rtfUV, normal, tangent, bitangent),
        Vertex3D(Vector3(r, b, f), color, rbfUV, normal, tangent, bitangent),
    };

    std::vector<unsigned int> ibo = {
        0, 2, 1, 0, 3, 2,
    };

    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());

}

void SimpleRenderer::DrawQuad(float b, float l, float n, float t, float r, float f, const Rgba& color) {
    DrawQuad(Vector3(l, b, n), Vector3(r, t, f), color, Vector4(0.0f, 0.0, 1.0f, 1.0f));
}

void SimpleRenderer::DrawQuad(const Vector4& tlbrPosition /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/, float z /*= 0.0f*/, const Rgba& color /*= Rgba::WHITE*/,
    const Vector4& tlbrUV /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/) {
    std::vector<Vertex3D> vbo = {
        Vertex3D(Vector3(tlbrPosition.y, tlbrPosition.z, z), color, Vector2(tlbrUV.y, tlbrUV.z)),
        Vertex3D(Vector3(tlbrPosition.y, tlbrPosition.x, z), color, Vector2(tlbrUV.y, tlbrUV.x)),
        Vertex3D(Vector3(tlbrPosition.w, tlbrPosition.x, z), color, Vector2(tlbrUV.w, tlbrUV.x)),
        Vertex3D(Vector3(tlbrPosition.w, tlbrPosition.z, z), color, Vector2(tlbrUV.w, tlbrUV.z)),
    };

    std::vector<unsigned int> ibo = {
        0, 1, 2, 0, 2, 3,
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());
}

void SimpleRenderer::DrawTexturedQuad(Texture2D* texture, const Vector3& position, const Vector3& half_extents, const Rgba& color /*= Rgba::WHITE*/, const Vector4& tlbrUV /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/) {
    SetTexture(texture);
    DrawQuad(position, position + half_extents * 2.0f, color, tlbrUV);
}

void SimpleRenderer::DrawQuadTwoSided(const Vector4& tlbrPosition /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/
                                      , float z /*= 0.0f*/
                                      , const Rgba& color /*= Rgba::WHITE*/
                                      , const Vector4& tlbrUV /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/
) {
    std::vector<Vertex3D> vbo = {
        //FRONT
        Vertex3D(Vector3(tlbrPosition.y, tlbrPosition.z, z), color, Vector2(tlbrUV.y, tlbrUV.z), -Vector3::Z_AXIS),
        Vertex3D(Vector3(tlbrPosition.y, tlbrPosition.x, z), color, Vector2(tlbrUV.y, tlbrUV.x), -Vector3::Z_AXIS),
        Vertex3D(Vector3(tlbrPosition.w, tlbrPosition.x, z), color, Vector2(tlbrUV.w, tlbrUV.x), -Vector3::Z_AXIS),
        Vertex3D(Vector3(tlbrPosition.w, tlbrPosition.z, z), color, Vector2(tlbrUV.w, tlbrUV.z), -Vector3::Z_AXIS),
        //BACK
        Vertex3D(Vector3(tlbrPosition.y, tlbrPosition.z, z), color, Vector2(tlbrUV.y, tlbrUV.z), Vector3::Z_AXIS),
        Vertex3D(Vector3(tlbrPosition.y, tlbrPosition.x, z), color, Vector2(tlbrUV.y, tlbrUV.x), Vector3::Z_AXIS),
        Vertex3D(Vector3(tlbrPosition.w, tlbrPosition.x, z), color, Vector2(tlbrUV.w, tlbrUV.x), Vector3::Z_AXIS),
        Vertex3D(Vector3(tlbrPosition.w, tlbrPosition.z, z), color, Vector2(tlbrUV.w, tlbrUV.z), Vector3::Z_AXIS),
    };

    std::vector<unsigned int> ibo = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());

}
void SimpleRenderer::DrawQuadTwoSided(const Vector3& position, const Vector3& half_extents, const Rgba& color /*= Rgba::WHITE*/, const Vector4& tlbrUV /*= Vector4(0.0f, 0.0f, 1.0f, 1.0f)*/) {
    std::vector<Vertex3D> vbo = {
        //FRONT
        Vertex3D(position + Vector3(-half_extents.x, -half_extents.y, 0.0f), color, Vector2(tlbrUV.y, tlbrUV.z), -Vector3::Z_AXIS),
        Vertex3D(position + Vector3(-half_extents.x,  half_extents.y, 0.0f), color, Vector2(tlbrUV.y, tlbrUV.x), -Vector3::Z_AXIS),
        Vertex3D(position + Vector3(half_extents.x,  half_extents.y, 0.0f), color, Vector2(tlbrUV.w, tlbrUV.x), -Vector3::Z_AXIS),
        Vertex3D(position + Vector3(half_extents.x, -half_extents.y, 0.0f), color, Vector2(tlbrUV.w, tlbrUV.z), -Vector3::Z_AXIS),
        //BACK
        Vertex3D(position + Vector3(-half_extents.x, -half_extents.y, 0.0f), color, Vector2(tlbrUV.y, tlbrUV.z), Vector3::Z_AXIS),
        Vertex3D(position + Vector3(-half_extents.x,  half_extents.y, 0.0f), color, Vector2(tlbrUV.y, tlbrUV.x), Vector3::Z_AXIS),
        Vertex3D(position + Vector3(half_extents.x,  half_extents.y, 0.0f), color, Vector2(tlbrUV.w, tlbrUV.x), Vector3::Z_AXIS),
        Vertex3D(position + Vector3(half_extents.x, -half_extents.y, 0.0f), color, Vector2(tlbrUV.w, tlbrUV.z), Vector3::Z_AXIS),
    };

    std::vector<unsigned int> ibo = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());

}
void SimpleRenderer::DrawLine(const Vertex3D& start, const Vertex3D& end) {

    std::vector<Vertex3D> vbo = {
        start, end,
    };

    std::vector<unsigned int> ibo = {
        0, 1,
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::LINES, _temp_vbo, _temp_ibo, ibo.size());

}

void SimpleRenderer::DrawPoint(const Vertex3D& point) {

    std::vector<Vertex3D> vbo = {
        point,
    };

    std::vector<unsigned int> ibo = {
        0,
    };

    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::POINTS, _temp_vbo, _temp_ibo, ibo.size());
}
void SimpleRenderer::DrawCube(const Vector3& position, const Vector3& half_extents, const Rgba& color /*= Rgba::WHITE*/) {
    
    //Verts
    Vector3 lower_left_front_vert = position + Vector3(-half_extents.x, -half_extents.y, -half_extents.z);
    Vector3 upper_left_front_vert = position + Vector3(-half_extents.x, half_extents.y, -half_extents.z);
    Vector3 upper_right_front_vert = position + Vector3(half_extents.x, half_extents.y, -half_extents.z);
    Vector3 lower_right_front_vert = position + Vector3(half_extents.x, -half_extents.y, -half_extents.z);

    Vector3 lower_left_back_vert = position + Vector3(-half_extents.x, -half_extents.y, half_extents.z);
    Vector3 upper_left_back_vert = position + Vector3(-half_extents.x, half_extents.y,  half_extents.z);
    Vector3 upper_right_back_vert = position + Vector3(half_extents.x, half_extents.y,  half_extents.z);
    Vector3 lower_right_back_vert = position + Vector3(half_extents.x, -half_extents.y, half_extents.z);

    //Edges
    Vector3 front_bottom_edge = (lower_right_front_vert - lower_left_front_vert).GetNormalize();
    Vector3 front_left_edge = (upper_left_front_vert - lower_left_front_vert).GetNormalize();
    Vector3 front_top_edge = (upper_right_front_vert - upper_left_front_vert).GetNormalize();
    Vector3 front_right_edge = (upper_right_front_vert - lower_right_front_vert).GetNormalize();

    Vector3 back_bottom_edge = (lower_left_back_vert - lower_right_back_vert).GetNormalize();
    Vector3 back_left_edge = (upper_left_back_vert - lower_left_back_vert).GetNormalize();
    Vector3 back_top_edge = (upper_left_back_vert - upper_right_back_vert).GetNormalize();
    Vector3 back_right_edge = (upper_right_back_vert - lower_right_back_vert).GetNormalize();

    Vector3 right_bottom_edge = (lower_right_back_vert - lower_right_front_vert).GetNormalize();
    Vector3 right_top_edge = (upper_right_back_vert - upper_right_front_vert).GetNormalize();

    Vector3 left_bottom_edge = (lower_left_back_vert - lower_left_front_vert).GetNormalize();
    Vector3 left_top_edge = (upper_left_back_vert - upper_left_front_vert).GetNormalize();

    //Normals
    Vector3 back_normal = -CrossProduct(front_bottom_edge, front_left_edge).GetNormalize();
    Vector3 front_normal = -back_normal;
    Vector3 left_normal = -CrossProduct(right_bottom_edge, front_right_edge).GetNormalize();
    Vector3 right_normal = -left_normal;
    Vector3 bottom_normal = -CrossProduct(front_bottom_edge, left_bottom_edge).GetNormalize();
    Vector3 top_normal = -bottom_normal;

    //Tangents
    Vector3 front_tangent = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 back_tangent = -front_tangent;
    Vector3 right_tangent = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 left_tangent = -right_tangent;
    Vector3 bottom_tangent = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 top_tangent = Vector3(1.0f, 0.0f, 0.0f);

    //Bitangents
    Vector3 front_bitangent = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 back_bitangent = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 right_bitangent = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 left_bitangent = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 bottom_bitangent = Vector3(0.0f, 0.0f, -1.0f);
    Vector3 top_bitangent = Vector3(0.0f, 0.0f, 1.0f);

    std::vector<Vertex3D> vbo = {
        //FRONT
        Vertex3D(lower_left_front_vert, color, Vector2(0.0f, 1.0f),  front_normal, front_tangent, front_bitangent),
        Vertex3D(upper_left_front_vert, color, Vector2(0.0f, 0.0f),  front_normal, front_tangent, front_bitangent),
        Vertex3D(upper_right_front_vert, color, Vector2(1.0f, 0.0f), front_normal, front_tangent, front_bitangent),
        Vertex3D(lower_right_front_vert, color, Vector2(1.0f, 1.0f), front_normal, front_tangent, front_bitangent),
        //BACK
        Vertex3D(lower_left_back_vert, color, Vector2(1.0f, 1.0f),  back_normal, back_tangent, back_bitangent), //1 4
        Vertex3D(upper_left_back_vert, color, Vector2(1.0f, 0.0f),  back_normal, back_tangent, back_bitangent), //2 5
        Vertex3D(upper_right_back_vert, color, Vector2(0.0f, 0.0f), back_normal, back_tangent, back_bitangent), //3
        Vertex3D(lower_right_back_vert, color, Vector2(0.0f, 1.0f), back_normal, back_tangent, back_bitangent), //6
        //LEFT
        Vertex3D(upper_left_front_vert, color, Vector2(1.0f, 0.0f), left_normal, left_tangent, left_bitangent),
        Vertex3D(upper_left_back_vert, color, Vector2(0.0f, 0.0f),  left_normal, left_tangent, left_bitangent),
        Vertex3D(lower_left_back_vert, color, Vector2(0.0f, 1.0f),  left_normal, left_tangent, left_bitangent),
        Vertex3D(lower_left_front_vert, color, Vector2(1.0f, 1.0f), left_normal, left_tangent, left_bitangent),
        //RIGHT
        Vertex3D(lower_right_front_vert, color, Vector2(0.0f, 1.0f), right_normal, right_tangent, right_bitangent),
        Vertex3D(upper_right_front_vert, color, Vector2(0.0f, 0.0f), right_normal, right_tangent, right_bitangent),
        Vertex3D(upper_right_back_vert, color, Vector2(1.0f, 0.0f),  right_normal, right_tangent, right_bitangent),
        Vertex3D(lower_right_back_vert, color, Vector2(1.0f, 1.0f),  right_normal, right_tangent, right_bitangent),
        //TOP
        Vertex3D(upper_right_back_vert, color, Vector2(1.0f, 0.0f),  top_normal, top_tangent, top_bitangent),
        Vertex3D(upper_right_front_vert, color, Vector2(1.0f, 1.0f), top_normal, top_tangent, top_bitangent),
        Vertex3D(upper_left_front_vert, color, Vector2(0.0f, 1.0f),  top_normal, top_tangent, top_bitangent),
        Vertex3D(upper_left_back_vert, color, Vector2(0.0f, 0.0f),   top_normal, top_tangent, top_bitangent),
        //BOTTOM
        Vertex3D(lower_right_front_vert, color, Vector2(1.0f, 0.0f),bottom_normal, bottom_tangent, bottom_bitangent),
        Vertex3D(lower_left_front_vert, color, Vector2(0.0f, 0.0f), bottom_normal, bottom_tangent, bottom_bitangent),
        Vertex3D(lower_left_back_vert, color, Vector2(0.0f, 1.0f),  bottom_normal, bottom_tangent, bottom_bitangent),
        Vertex3D(lower_right_back_vert, color, Vector2(1.0f, 1.0f), bottom_normal, bottom_tangent, bottom_bitangent),
    };

    std::vector<unsigned int> ibo = {
        0,  2,   1,  0,  3,  2, //FRONT
        4,  5,   6,  4,  6,  7, //BACK
        8,  9,  10,  8, 10, 11, //LEFT
        12, 14, 13, 12, 15, 14, //RIGHT
        16, 18, 17, 16, 19, 18, //TOP
        20, 21, 22, 20, 22, 23, //BOTTOM
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());
}

void SimpleRenderer::DrawOctahedron(const Vector3& position, const Rgba& color, float radius) {
    DrawUvSphere(position, color, radius, 4, 2);
}
void SimpleRenderer::DrawIcoSphere(const Vector3& position, const Rgba& color, float radius, unsigned int /*iterations*/) {

    std::vector<Vertex3D> vbo;
    std::vector<unsigned int> ibo;
    CalculateUvSphereBuffers(position, color, radius, 4, 2, vbo, ibo);

    //Positions
    std::vector<Vector3> verts;
    verts.reserve(vbo.size());
    for (auto & i : vbo) {
        verts.push_back(i.position);
    }

    std::array<Vector3, 3> face0 = { vbo[0].position, vbo[1].position, vbo[2].position };
    std::array<Vector3, 3> face1 = { vbo[0].position, vbo[2].position, vbo[3].position };
    std::array<Vector3, 3> face2 = { vbo[0].position, vbo[3].position, vbo[4].position };
    std::array<Vector3, 3> face3 = { vbo[0].position, vbo[4].position, vbo[5].position };
    std::array<Vector3, 3> face4 = { vbo[6].position, vbo[1].position, vbo[2].position };
    std::array<Vector3, 3> face5 = { vbo[6].position, vbo[2].position, vbo[3].position };
    std::array<Vector3, 3> face6 = { vbo[6].position, vbo[3].position, vbo[4].position };
    std::array<Vector3, 3> face7 = { vbo[6].position, vbo[4].position, vbo[5].position };

    std::array<std::array<Vector3, 3>, 8> faces = { face0, face1, face2, face3, face4, face5, face6, face7 };
    for (auto & face : faces) {
        verts.push_back((face[0] + face[1] + face[2]) / 3);
    }

    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());
}
void SimpleRenderer::CalculateUvSphereBuffers(const Vector3& position, const Rgba& color, float radius, unsigned int slices, unsigned int stacks, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo) {
    float phiStep = MathUtils::M_PI / stacks;
    float thetaStep = 2.0f * MathUtils::M_PI / slices;

    //Verts
    for (unsigned int i = 0; i <= stacks; ++i) {
        float phi = i * phiStep;
        for (unsigned int j = 0; j <= slices; ++j) {
            float theta = j * thetaStep;

            float s_phi = std::sin(phi);
            float c_phi = std::cos(phi);

            float s_theta = std::sin(theta);
            float c_theta = std::cos(theta);

            float x = radius * s_phi * c_theta;
            float y = radius * c_phi;
            float z = radius * s_phi * s_theta;

            Vector3 p = position + Vector3(x, y, z);

            float tx = -radius * s_phi;
            float ty = 0.0f;
            float tz = radius * s_phi * c_theta;
            Vector3 t(tx, ty, tz);
            t.Normalize();

            Vector3 n = p - position;
            n.Normalize();

            Vector3 bt = CrossProduct(n, t);
            bt.Normalize();

            float u = theta / (2.0f * MathUtils::M_PI);
            float v = phi / MathUtils::M_PI;
            Vector2 uv(u, v);
            vbo.emplace_back(p, color, uv, n, t, bt);
        }
    }

    //North Pole Indicies
    for (unsigned int i = 1; i <= slices; ++i) {
        ibo.push_back(slices);
        ibo.push_back(slices + i);
        ibo.push_back(slices + i + 1);
    }

    unsigned int baseIndex = 1;
    unsigned int ringVertexCount = slices + 1;
    for (unsigned int i = 0; i < stacks; i++) {
        for (unsigned int j = 0; j <= slices; j++) {
            ibo.push_back(baseIndex + i * ringVertexCount + j);
            ibo.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            ibo.push_back(baseIndex + i * ringVertexCount + j + 1);

            ibo.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            ibo.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
            ibo.push_back(baseIndex + i * ringVertexCount + j + 1);
        }
    }

    //South pole Indicies
    unsigned int southPoleIndex = vbo.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount - 1;
    for (unsigned int i = 0; i < slices; i++) {
        ibo.push_back(baseIndex);
        ibo.push_back(baseIndex + i);
        ibo.push_back(baseIndex + i + 1);
    }
}

void SimpleRenderer::DrawUvSphere(const Vector3& position, const Rgba& color, float radius, unsigned int slices, unsigned int stacks) {

    stacks = (std::min)((std::max)(2u, stacks), 64u); //parenthesis are intentional. Google Most Vexing Parse!
    slices = (std::min)((std::max)(2u, slices), 64u); //parenthesis are intentional. Google Most Vexing Parse!

    std::vector<Vertex3D> vbo;
    vbo.reserve(slices * stacks + (2 * slices));
    std::vector<unsigned int> ibo;
    ibo.reserve(slices * stacks + (2 * slices));

    Vector3 p = position;
    Rgba c = color;

    CalculateUvSphereBuffers(position, color, radius, slices, stacks, vbo, ibo);

    UpdateVbo(vbo);
    UpdateIbo(ibo);

    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());

}


void SimpleRenderer::RenderMesh(const Mesh& mesh, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo) {
    SetMaterial(mesh.GetMaterial());
    SetModelMatrix(mesh.GetLocalTransform());
    UpdateVbo(vbo);
    if(mesh.RenderVertexesOnly()) {
        Draw(PrimitiveType::TRIANGLES, _temp_vbo, vbo.size());
    } else {
        UpdateIbo(ibo);
        DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());
    }
    SetModelMatrix(Matrix4::GetIdentity());
}
bool SimpleRenderer::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}
void SimpleRenderer::UpdateVbo(const std::vector<Vertex3D>& new_vbo) {

    if (_current_vbo_size < new_vbo.size()) {
        delete _temp_vbo;
        _temp_vbo = _rhi_device->CreateVertexBuffer(new_vbo, BufferUsage::DYNAMIC, BufferBindUsage::VERTEX_BUFFER);
        _current_vbo_size = new_vbo.size();
    }
    _temp_vbo->Update(_rhi_context, new_vbo);
}

void SimpleRenderer::UpdateIbo(const std::vector<unsigned int>& new_ibo) {
    if (_current_ibo_size < new_ibo.size()) {
        delete _temp_ibo;
        _temp_ibo = _rhi_device->CreateIndexBuffer(new_ibo, BufferUsage::DYNAMIC, BufferBindUsage::INDEX_BUFFER);
        _current_ibo_size = new_ibo.size();
    }
    _temp_ibo->Update(_rhi_context, new_ibo);
}

bool SimpleRenderer::RegisterFontsFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    { //Avoid pollution by error code.
        std::error_code ec;
        if(!FS::exists(folderpath) || !FS::is_directory(p, ec)) {
            std::ostringstream ss;
            ss << "SimpleRenderer::RegisterFontsFromFolder: \"" << folderpath << "\" does not exist or is not a directory. Filesystem reported the following error: " << ec.message() << "\n";
            g_theFileLogger->LogErrorf(ss.str().c_str());
            g_theFileLogger->LogFlush();
            ERROR_AND_DIE(ss.str().c_str());
        }
    }
    if(!recursive) {
        for(FS::directory_iterator dir_iter(p); dir_iter != FS::directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(FS::is_directory(current_path)) continue;
            if(current_path.extension() != ".fnt") continue;
            CreateOrGetFont(current_path.stem().string());
        }
        return true;
    } else {
        for(FS::recursive_directory_iterator dir_iter(p); dir_iter != FS::recursive_directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(current_path.extension() != ".fnt") continue;
            CreateOrGetFont(current_path.stem().string());
        }
        return true;
    }
}

bool SimpleRenderer::RegisterMotion(const std::string& fbx_path, MeshMotion* motion) {
    auto motion_iter = _motions.find(fbx_path);
    bool motion_exists = motion_iter != _motions.end();
    if(motion_exists) {
        delete motion_iter->second;
        motion_iter->second = nullptr;
    }
    if(motion == nullptr) {
        return false;
    }
    _motions.insert_or_assign(fbx_path, motion);
    return true;
}

bool SimpleRenderer::RegisterSkeleton(const std::string& fbx_path, MeshSkeleton* skeleton) {
    auto skeleton_iter = _skeletons.find(fbx_path);
    bool skeleton_exists = skeleton_iter != _skeletons.end();
    if(skeleton_exists) {
        delete skeleton_iter->second;
        skeleton_iter->second = nullptr;
    }
    if(skeleton == nullptr) {
        return false;
    }
    _skeletons.insert_or_assign(fbx_path, skeleton);
    return true;
}

bool SimpleRenderer::RegisterMesh(const std::string& fbx_path, Mesh* mesh) {
    auto mesh_iter = _meshes.find(fbx_path);
    bool mesh_exists = mesh_iter != _meshes.end();
    if(mesh_exists) {
        delete mesh_iter->second;
        mesh_iter->second = nullptr;
    }
    if(mesh == nullptr) {
        return false;
    }
    _meshes.insert_or_assign(fbx_path, mesh);
    return true;
}

bool SimpleRenderer::RegisterModel(const std::string& folderpath, Model* model) {
    auto model_iter = _models.find(folderpath);
    bool model_exists = model_iter != _models.end();
    if(model_exists) {
        delete model_iter->second;
        model_iter->second = nullptr;
    }
    if(model == nullptr) {
        return false;
    }
    _models.insert_or_assign(folderpath, model);
    return true;
}

bool SimpleRenderer::FastCopyTexture2D(Texture2D* dest, Texture2D* source) {
    if(dest == nullptr || source == nullptr) {
        return false;
    }
    if(dest == source) {
        return false;
    }

    D3D11_TEXTURE2D_DESC dest_desc;
    dest->GetDxResource()->GetDesc(&dest_desc);

    D3D11_TEXTURE2D_DESC source_desc;
    source->GetDxResource()->GetDesc(&source_desc);

    if(dest_desc.Width != source_desc.Width
       || dest_desc.Height != source_desc.Height
       || dest_desc.ArraySize != source_desc.ArraySize
       || dest_desc.Format != source_desc.Format) {
        return false;
    }
    
    _rhi_context->GetDxContext()->CopyResource(dest->GetDxResource(), source->GetDxResource());
    return true;
}

bool SimpleRenderer::IsSupportedImageType(std::string extension) {
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c)->unsigned char { return std::tolower(c, std::locale("")); });
    bool isJpeg = extension == ".jpg" || extension == ".jpeg";
    bool isPng = extension == ".png";
    bool isTga = extension == ".tga";
    bool isBmp = extension == ".bmp";
    bool isPsd = extension == ".psd";
    bool isGif = extension == ".gif";
    bool isHdr = extension == ".hdr";
    bool isPic = extension == ".pic";
    bool isPnm = extension == ".pnm";
    bool isSupported = isJpeg || isPng || isTga || isBmp || isPsd || isGif || isHdr || isPic || isPnm;
    return isSupported;
}

bool SimpleRenderer::RegisterComputeShader(const std::string& name, ComputeShader* computeShader) {
    auto shader_iter = _compute_shaders.find(name);
    bool shader_exists = shader_iter != _compute_shaders.end();
    if(shader_exists) {
        delete shader_iter->second;
        shader_iter->second = nullptr;
        if(computeShader == nullptr) {
            return false;
        }
    }
    _compute_shaders.insert_or_assign(name, computeShader);
    return true;
}

ComputeShader* SimpleRenderer::GetComputeShader(const std::string& name) {
    return _compute_shaders[name];
}

void SimpleRenderer::FontToAABB2(const BitmapFont& f, const std::vector<std::string>& textLines, const AABB2& box) {
    auto textHeight = textLines.size() * f.GetHeight();
    if(textHeight > box.maxs.y) {
        /* DO NOTHING */
    }
}

void SimpleRenderer::AABB2ToFont(const BitmapFont& /*f*/, const std::vector<std::string>& /*textLines*/, const AABB2& /*box*/) {
    /* DO NOTHING */
}

void SimpleRenderer::DrawDebugLine2D(const Vector2& start, const Vector2& end, float thickness, const Rgba& startColor, const Rgba& endColor) {
    Vector2 direction = (end - start).GetNormalize();
    float half_thickness = thickness * 0.5f;
    Vector2 perpendicular_direction = Vector2(-direction.y, direction.x);

    std::vector<Vertex3D> vbo = {
        Vertex3D(Vector3(start +  perpendicular_direction * half_thickness, 0.0f), startColor),
        Vertex3D(Vector3(start + -perpendicular_direction * half_thickness, 0.0f), startColor),
        Vertex3D(Vector3(end   + -perpendicular_direction * half_thickness, 0.0f), endColor),
        Vertex3D(Vector3(end   +  perpendicular_direction * half_thickness, 0.0f), endColor),
    };

    std::vector<unsigned int> ibo = {
        0, 1, 2, 0, 2, 3,
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());
}

void SimpleRenderer::DrawDebugBox2D(const Vector2& center, const Vector2& halfExtents, float thickness, const Rgba& fillColor, const Rgba& edgeColor, int edgeAlignment /*= 1*/) {
    DrawDebugBox2D(AABB2(center, halfExtents.x, halfExtents.y), thickness, fillColor, edgeColor, edgeAlignment);
}

void SimpleRenderer::DrawDebugBox2D(const AABB2& box, float edgeThickness, const Rgba& fillColor, const Rgba& edgeColor, int edgeAlignment /*= 1*/) {
    
    if(MathUtils::IsEquivalent(edgeThickness, 1.0f)) {
        DrawDebugAABB2(box, fillColor, edgeColor);
        return;
    }

    //Calculate directions for edge verts
    Vector2 tl = box.mins;
    Vector2 tr = Vector2(box.maxs.x, box.mins.y);
    Vector2 br = box.maxs;
    Vector2 bl = Vector2(box.mins.x, box.maxs.y);

    Vector2 bl_to_center = (tr - bl).GetNormalize();
    Vector2 br_to_center = (tl - br).GetNormalize();
    Vector2 tr_to_center = (bl - tr).GetNormalize();
    Vector2 tl_to_center = (br - tl).GetNormalize();

    Vector2 bl_from_center = -bl_to_center;
    Vector2 br_from_center = -br_to_center;
    Vector2 tr_from_center = -tr_to_center;
    Vector2 tl_from_center = -tl_to_center;

    Vector2 bl_inner_edge = bl + (bl_to_center * edgeThickness);
    Vector2 br_inner_edge = br + (br_to_center * edgeThickness);
    Vector2 tr_inner_edge = tr + (tr_to_center * edgeThickness);
    Vector2 tl_inner_edge = tl + (tl_to_center * edgeThickness);

    Vector2 bl_outer_edge = bl + (bl_from_center * edgeThickness);
    Vector2 br_outer_edge = br + (br_from_center * edgeThickness);
    Vector2 tr_outer_edge = tr + (tr_from_center * edgeThickness);
    Vector2 tl_outer_edge = tl + (tl_from_center * edgeThickness);

    float halfEdgeThickness = edgeThickness * 0.5f;

    Vector2 bl_center_edge = bl + (bl_to_center * halfEdgeThickness);
    Vector2 br_center_edge = br + (br_to_center * halfEdgeThickness);
    Vector2 tr_center_edge = tr + (tr_to_center * halfEdgeThickness);
    Vector2 tl_center_edge = tl + (tl_to_center * halfEdgeThickness);

    //Create edge positions based on alignment request.
    Vector2 bl_edge = edgeAlignment ? (edgeAlignment == -1 ? (bl_outer_edge) : (bl_inner_edge)) : (bl_center_edge);
    Vector2 tl_edge = edgeAlignment ? (edgeAlignment == -1 ? (tl_outer_edge) : (tl_inner_edge)) : (tl_center_edge);
    Vector2 tr_edge = edgeAlignment ? (edgeAlignment == -1 ? (tr_outer_edge) : (tr_inner_edge)) : (tr_center_edge);
    Vector2 br_edge = edgeAlignment ? (edgeAlignment == -1 ? (br_outer_edge) : (br_inner_edge)) : (br_center_edge);

    //Move box edge outwards if alignment request is not inner
    if(edgeAlignment == 0) {
        bl = bl + (bl_from_center * halfEdgeThickness);
        br = br + (br_from_center * halfEdgeThickness);
        tr = tr + (tr_from_center * halfEdgeThickness);
        tl = tl + (tl_from_center * halfEdgeThickness);
    } else if(edgeAlignment == -1) {
        //Flip vert order by recalculating mins and maxs
        bl = bl_outer_edge;
        br = br_outer_edge;
        tr = tr_outer_edge;
        tl = tl_outer_edge;
        
        tl_edge = box.mins;
        tr_edge = Vector2(box.maxs.x, box.mins.y);
        br_edge = box.maxs;
        bl_edge = Vector2(box.mins.x, box.maxs.y);
    }

    std::vector<Vertex3D> vbo = {
        //Edge
        Vertex3D(Vector3(bl_edge, 0.0f), edgeColor),
        Vertex3D(Vector3(bl, 0.0f), edgeColor),
        Vertex3D(Vector3(tl, 0.0f), edgeColor),
        Vertex3D(Vector3(tl_edge, 0.0f), edgeColor),
        Vertex3D(Vector3(tr, 0.0f), edgeColor),
        Vertex3D(Vector3(tr_edge, 0.0f), edgeColor),
        Vertex3D(Vector3(br, 0.0f), edgeColor),
        Vertex3D(Vector3(br_edge, 0.0f), edgeColor),
        //Fill
        Vertex3D(Vector3(bl_edge, 0.0f), fillColor),
        Vertex3D(Vector3(tl_edge, 0.0f), fillColor),
        Vertex3D(Vector3(tr_edge, 0.0f), fillColor),
        Vertex3D(Vector3(br_edge, 0.0f), fillColor),
    };

    std::vector<unsigned int> ibo = {
        0, 1, 2, 0, 2, 3,
        3, 2, 4, 3, 4, 5,
        5, 4, 6, 5, 6, 7,
        7, 6, 1, 7, 1, 0,
        8, 9, 10, 8, 10, 11,
    };
    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, ibo.size());

}

void SimpleRenderer::DrawCircle(const Vector2& center, float radius, const Rgba& edgeColor /*= Rgba::WHITE*/) {

    float num_sides = 64.0f;
    std::vector<Vector3> verts;
    verts.reserve(static_cast<std::size_t>(num_sides));
    float anglePerVertex = 360.0f / num_sides;
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cosf(radians) + center.x;
        float pY = radius * std::sinf(radians) + center.y;
        verts.emplace_back(Vector3(Vector2(pX, pY), 0.0f));
    }

    std::vector<Vertex3D> vbo;
    vbo.resize(verts.size());
    for(std::size_t i = 0; i < vbo.size(); ++i) {
        vbo[i] = Vertex3D(verts[i], edgeColor);
    }

    std::vector<unsigned int> ibo;
    ibo.resize(static_cast<std::size_t>(num_sides + 1));
    for(std::size_t i = 0; i < ibo.size(); ++i) {
        ibo[i] = i % static_cast<std::size_t>(num_sides);
    }

    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::LINESSTRIP, _temp_vbo, _temp_ibo, ibo.size());
}

void SimpleRenderer::DrawFilledCircle(const Vector2& center, float radius, const Rgba& fillColor /*= Rgba::WHITE*/) {

    float num_sides = 64.0f;
    std::vector<Vector3> verts;
    verts.reserve(static_cast<std::size_t>(num_sides + 1.0f));
    float anglePerVertex = 360.0f / num_sides;
    verts.emplace_back(Vector3(center, 0.0f));
    for(float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        float radians = MathUtils::ConvertDegreesToRadians(degrees);
        float pX = radius * std::cosf(radians) + center.x;
        float pY = radius * std::sinf(radians) + center.y;
        verts.emplace_back(Vector3(Vector2(pX, pY), 0.0f));
    }

    std::vector<Vertex3D> vbo;
    vbo.resize(verts.size());
    for(std::size_t i = 0; i < vbo.size(); ++i) {
        vbo[i] = Vertex3D(verts[i], fillColor);
    }

    std::vector<unsigned int> ibo;
    ibo.resize(static_cast<std::size_t>(num_sides * 3));
    std::size_t ibo_s = ibo.size();
    for(std::size_t i = 0; i < ibo_s; i += 3) {
        ibo[i + 0] = 0;
        ibo[i + 1] = (i + 1) % static_cast<std::size_t>(num_sides);
        ibo[i + 2] = (i + 2) % static_cast<std::size_t>(num_sides);
    }

    UpdateVbo(vbo);
    UpdateIbo(ibo);
    DrawIndexed(PrimitiveType::TRIANGLESTRIP, _temp_vbo, _temp_ibo, ibo.size());
}

void SimpleRenderer::DrawDebugCircle2D(const Vector2& center, float radius, float /*thickness*/, const Rgba& fillColor, const Rgba& /*edgeColor*/, int /*edgeAlignment*/ /*= 1*/) {
    DrawFilledCircle(center, radius, fillColor);
}

void SimpleRenderer::DrawDebugX2D(const Vector2& center, float radius, float thickness, const Rgba& color) {
    DrawDebugLine2D(center + Vector2(-radius, +radius), center + Vector2(+radius, -radius), thickness, color, color);
    DrawDebugLine2D(center + Vector2(-radius, -radius), center + Vector2(+radius, +radius), thickness, color, color);
}

ComputeShader* SimpleRenderer::CreateComputeShader(const std::string& filepath) {

    namespace FS = std::experimental::filesystem;

    FS::path p = filepath;
    if(FS::exists(p) == false) {
        return nullptr;
    }

    std::string name = p.filename().string();

    ComputeShader* prog = _rhi_device->CreateComputeShaderFromHlslFile(filepath);
    return prog;

}

void SimpleRenderer::SetRasterState(RasterState* raster_state) {
    if (raster_state == nullptr) {
        raster_state = GetRasterState("__default");
    }
    _rhi_context->SetRasterState(raster_state);
}

bool SimpleRenderer::RegisterRasterState(const std::string& name, RasterState* raster) {
    auto raster_iter = _rasters.find(name);
    bool raster_exists = raster_iter != _rasters.end();
    if (raster_exists) {
        delete raster_iter->second;
        raster_iter->second = nullptr;
        if (raster == nullptr) {
            return false;
        }
    }
    _rasters.insert_or_assign(name, raster);
    return true;

}

RasterState* SimpleRenderer::GetRasterState(const std::string& name) {
    auto raster_iter = this->_rasters.find(name);
    if (raster_iter == _rasters.end()) {
        return GetRasterState("__default");
    } else {
        return _rasters[name];
    }
}

void SimpleRenderer::ReloadTextures() {
    std::vector<std::string> texture_paths;
    for(auto& t : _textures) {
        texture_paths.push_back(t.first);
        delete t.second;
    }
    for(auto& p : texture_paths) {
        CreateTexture(p);
    }
}

const std::map<std::string, Texture2D*>& SimpleRenderer::GetLoadedTextures() const {
    return _textures;
}

void SimpleRenderer::SetProjectionMatrix(const Matrix4& matrix) {
    _matrix_data.projection = matrix;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

const Matrix4& SimpleRenderer::GetProjectionMatrix() const {
    return _matrix_data.projection;
}

void SimpleRenderer::SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far) {
    Matrix4 proj = Matrix4::CreateDXOrthographicProjection(leftBottom.x, rightTop.x, leftBottom.y, rightTop.y, near_far.x, near_far.y);
    SetProjectionMatrix(proj);
}

//void SimpleRenderer::SetOrthoProjection(const Vector2& dimensions, const Vector2& origin) {
//    Vector2 leftBottom = Vector2(origin.x - dimensions.x, origin.y + dimensions.y);
//    Vector2 rightTop = Vector2(origin.x + dimensions.x, origin.y - dimensions.y);
//    SetOrthoProjection(leftBottom, rightTop, Vector2(0.0f, 1.0f));
//}

void SimpleRenderer::SetPerspectiveProjection(const Vector2& vfovDegrees_aspect, const Vector2& nz_fz) {
    Matrix4 proj = Matrix4::CreateDXPerspectiveProjection(vfovDegrees_aspect.x, vfovDegrees_aspect.y, nz_fz.x, nz_fz.y);
    SetProjectionMatrix(proj);
}

void SimpleRenderer::SetViewMatrix(const Matrix4& viewMatrix) {
    _matrix_data.view = viewMatrix;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void SimpleRenderer::SetModelMatrix(const Matrix4& modelMatrix) {
    _matrix_data.model = modelMatrix;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void SimpleRenderer::AppendModelMatrix(const Matrix4& modelMatrix) {
    _matrix_data.model = modelMatrix * _matrix_data.model;
    _matrix_cb->Update(_rhi_context, &_matrix_data);
    SetConstantBuffer(MATRIX_BUFFER_INDEX, _matrix_cb);
}

void SimpleRenderer::SetAmbientLight(const Rgba& ambient) {
    float ambientAsFloats[4];
    ambient.GetAsFloats(ambientAsFloats[0], ambientAsFloats[1], ambientAsFloats[2], ambientAsFloats[3]);
    _lighting_data.ambient.x = ambientAsFloats[0];
    _lighting_data.ambient.y = ambientAsFloats[1];
    _lighting_data.ambient.z = ambientAsFloats[2];
    _lighting_data.ambient.w = ambientAsFloats[3];
    _lighting_cb->Update(_rhi_context, &_lighting_data);
    SetConstantBuffer(LIGHTING_BUFFER_INDEX, _lighting_cb);
}

void SimpleRenderer::SetPointLight(unsigned int index, const Vector3& camera_position, const Vector3& light_position, const Rgba& color, float intensity, const Vector3& attenuation, const Vector2& specular_factor_power) {
    if (index > MAX_LIGHT_COUNT) {
        g_theFileLogger->LogErrorf("SetPointLight: index exceeds max light count.");
        g_theFileLogger->LogFlush();
        ERROR_AND_DIE("SetPointLight: index exceeds max light count.");
    }
    _lighting_data.eye_position = Vector4(camera_position, 1.0f);
    _lighting_data.specular_factor_power = Vector4(specular_factor_power.x, specular_factor_power.y, 0.0f, 0.0f);
    _lighting_data.point_lights[index].attenuation = Vector4(attenuation, 1.0f);
    _lighting_data.point_lights[index].specAttenuation = _lighting_data.point_lights[index].attenuation;
    float colorAsFloats[4];
    color.GetAsFloats(colorAsFloats[0], colorAsFloats[1], colorAsFloats[2], colorAsFloats[3]);
    colorAsFloats[3] = intensity;
    _lighting_data.point_lights[index].color.x = colorAsFloats[0];
    _lighting_data.point_lights[index].color.y = colorAsFloats[1];
    _lighting_data.point_lights[index].color.z = colorAsFloats[2];
    _lighting_data.point_lights[index].color.w = colorAsFloats[3];
    _lighting_data.point_lights[index].position = Vector4(light_position.x, light_position.y, light_position.z, 1.0f);
    _lighting_cb->Update(_rhi_context, &_lighting_data);
    SetConstantBuffer(LIGHTING_BUFFER_INDEX, _lighting_cb);
}

void SimpleRenderer::SetSpotlight(unsigned int index, const Vector3& camera_position, const Vector3& light_position, const Vector3& light_direction, const Rgba& color, float intensity, const Vector3& attenuation, const Vector2& specular_factor_power, const Vector2& /*inner_outer_angleDegrees*/) {
    if (index > MAX_LIGHT_COUNT) {
        g_theFileLogger->LogErrorf("SetSpotLight: index exceeds max light count.");
        g_theFileLogger->LogFlush();
        ERROR_AND_DIE("SetSpotLight: index exceeds max light count.");
    }
    _lighting_data.eye_position = Vector4(camera_position, 1.0f);
    _lighting_data.specular_factor_power = Vector4(specular_factor_power.x, specular_factor_power.y, 0.0f, 0.0f);
    _lighting_data.directional_lights[index].light.position = Vector4(light_position, 1.0f);
    float colorAsFloats[4];
    color.GetAsFloats(colorAsFloats[0], colorAsFloats[1], colorAsFloats[2], colorAsFloats[3]);
    colorAsFloats[3] = intensity;
    _lighting_data.directional_lights[index].light.color.x = colorAsFloats[0];
    _lighting_data.directional_lights[index].light.color.y = colorAsFloats[1];
    _lighting_data.directional_lights[index].light.color.z = colorAsFloats[2];
    _lighting_data.directional_lights[index].light.color.w = colorAsFloats[3];
    _lighting_data.directional_lights[index].light.attenuation = Vector4(attenuation, 1.0f);
    _lighting_data.directional_lights[index].light.specAttenuation = _lighting_data.point_lights[index].attenuation;
    _lighting_data.directional_lights[index].direction = Vector4(light_direction, 0.0f);

    _lighting_cb->Update(_rhi_context, &_lighting_data);
    SetConstantBuffer(LIGHTING_BUFFER_INDEX, _lighting_cb);

}

void SimpleRenderer::SetDirectionalLight(unsigned int index, const Vector3& camera_position, const Vector3& light_position, const Vector3& light_direction, const Rgba& color, float intensity, const Vector3& attenuation, const Vector2& specular_factor_power) {
    if (index > MAX_LIGHT_COUNT) {
        g_theFileLogger->LogErrorf("SetDirectionalLight: index exceeds max light count.");
        g_theFileLogger->LogFlush();
        ERROR_AND_DIE("SetDirectionalLight: index exceeds max light count.");
    }
    _lighting_data.eye_position = Vector4(camera_position, 1.0f);
    _lighting_data.specular_factor_power = Vector4(specular_factor_power.x, specular_factor_power.y, 0.0f, 0.0f);
    _lighting_data.directional_lights[index].light.position = Vector4(light_position, 1.0f);
    float colorAsFloats[4];
    color.GetAsFloats(colorAsFloats[0], colorAsFloats[1], colorAsFloats[2], colorAsFloats[3]);
    colorAsFloats[3] = intensity;
    _lighting_data.directional_lights[index].light.color.x = colorAsFloats[0];
    _lighting_data.directional_lights[index].light.color.y = colorAsFloats[1];
    _lighting_data.directional_lights[index].light.color.z = colorAsFloats[2];
    _lighting_data.directional_lights[index].light.color.w = colorAsFloats[3];
    _lighting_data.directional_lights[index].light.attenuation = Vector4(attenuation, 1.0f);
    _lighting_data.directional_lights[index].light.specAttenuation = _lighting_data.point_lights[index].attenuation;
    _lighting_data.directional_lights[index].direction = Vector4(light_direction, 0.0f);

    _lighting_cb->Update(_rhi_context, &_lighting_data);
    SetConstantBuffer(LIGHTING_BUFFER_INDEX, _lighting_cb);

}

Matrix4 SimpleRenderer::CreateBillboardView(const Matrix4& cameraViewMatrix) {
    return Matrix4::CalculateInverse(cameraViewMatrix);
}

KerningFont* SimpleRenderer::CreateOrGetFont(const std::string& name) {
    auto font_iter = _fonts.find(name);
    if(font_iter == _fonts.end()) {
        return this->CreateFontFromXML(name);
    } else {
        return this->GetFont(name);
    }
}

BitmapFont* SimpleRenderer::CreateBitmapFont(const std::string& filepath, int glyphWidth, int glyphHeight) {
    return new BitmapFont(CreateSpriteSheet(filepath, glyphWidth, glyphHeight));
}

SpriteSheet* SimpleRenderer::CreateSpriteSheet(const std::string& filepath, int tileWidth, int tileHeight) {
    namespace FS = std::experimental::filesystem;
    Texture2D* t = nullptr;
    FS::path p(filepath);
    if(FS::exists(p) == false) {
        g_theFileLogger->LogFlush();
        ASSERT_OR_DIE(false, "SimpleRenderer::CreateSpriteSheet(...) filepath does not exist.");
        return nullptr;
    } else {
        t = CreateOrGetTexture(p.string());
    }
    return new SpriteSheet(t, tileWidth, tileHeight);
}

ShaderProgram* SimpleRenderer::CreateOrGetShaderProgram(const std::string& name) {
    auto shader_iter = this->_shaderPrograms.find(name);
    if(shader_iter == _shaderPrograms.end()) {
        return CreateShaderProgram(name);
    } else {
        return GetShaderProgram(name);
    }
}

ComputeShader* SimpleRenderer::CreateOrGetComputeShader(const std::string& name) {
    auto shader_iter = _compute_shaders.find(name);
    if(shader_iter == _compute_shaders.end()) {
        return this->CreateComputeShader(name);
    } else {
        return GetComputeShader(name);
    }
}

MeshMotion* SimpleRenderer::CreateOrGetMotion(const std::string& fbx_path, MeshSkeleton& meshSkeleton, MeshSkeletonInstance& meshSkeletonInstance, const Matrix4& initialTransform /*= Matrix4::GetIdentity()*/) {
    auto motion_iter = _motions.find(fbx_path);
    bool motion_exists = motion_iter != _motions.end();
    if(motion_exists) {
        return GetMotion(fbx_path);
    } else {
        return CreateMotion(fbx_path, meshSkeleton, meshSkeletonInstance, initialTransform);
    }
}

MeshMotion* SimpleRenderer::GetMotion(const std::string& fbx_path) {
    return _motions[fbx_path];
}

MeshMotion* SimpleRenderer::CreateMotion(const std::string& fbx_path, MeshSkeleton& meshSkeleton, MeshSkeletonInstance& meshSkeletonInstance, const Matrix4& initialTransform /*= Matrix4::GetIdentity()*/) {
    namespace FS = std::experimental::filesystem;

    FS::path file_path_load(fbx_path);
    std::string folder_path_load = file_path_load.parent_path().string() + "/";
    std::string basefilename_load = file_path_load.stem().string();
    std::string extension = ".motion";
    FS::path path_full = folder_path_load + basefilename_load + extension;

    MeshMotion* motion = nullptr;
    bool fileExists = FS::exists(path_full);
    bool foundBinaryFile = fileExists && FS::file_size(path_full) > 0;
    if(foundBinaryFile) {
        std::string p = folder_path_load + basefilename_load + extension;
        motion = CreateMotionFromEngineAsset(p);
        meshSkeletonInstance.skeleton = &meshSkeleton;
        meshSkeletonInstance.current_pose = motion->get_pose(0);
        meshSkeletonInstance.InitializeSkinTransforms();
        meshSkeletonInstance.SetLocalTransform(initialTransform);
        RegisterMotion(fbx_path, motion);
    } else {
        motion = new MeshMotion;
        if(!FbxLoadMotion(motion, &meshSkeleton, fbx_path)) {
            delete motion;
            g_theConsole->WarnMsg("No motions found.");
            return nullptr;
        }
        meshSkeletonInstance.skeleton = &meshSkeleton;
        meshSkeletonInstance.current_pose = motion->get_pose(0);
        meshSkeletonInstance.SetLocalTransform(initialTransform);
        meshSkeletonInstance.InitializeSkinTransforms();
        ExportFBXMotionToEngineAsset(fbx_path, *motion);
        RegisterMotion(fbx_path, motion);
    }
    return motion;
}
MeshMotion* SimpleRenderer::CreateMotionFromEngineAsset(const std::string& asset_path) {
    FileUtils::FileBinaryStream fs;
    if(!fs.open_for_read(asset_path)) {
        fs.close();
        g_theConsole->WarnMsg("Motion binary failed to open.");
        return nullptr;
    }
    MeshMotion* motion = new MeshMotion;
    if(!motion->read(fs)) {
        fs.close();
        delete motion;
        g_theConsole->ErrorMsg("Motion binary failed to load.");
        return nullptr;
    }
    fs.close();

    return motion;
}
MeshSkeleton* SimpleRenderer::CreateOrGetSkeleton(const std::string& fbx_path, const Matrix4& initialTransform) {
    auto skeleton_iter = _skeletons.find(fbx_path);
    bool skeleton_exists = skeleton_iter != _skeletons.end();
    if(skeleton_exists) {
        return GetSkeleton(fbx_path);
    } else {
        return CreateSkeleton(fbx_path, initialTransform);
    }
}

MeshSkeleton* SimpleRenderer::GetSkeleton(const std::string& fbx_path) {
    return _skeletons[fbx_path];
}

MeshSkeleton* SimpleRenderer::CreateSkeleton(const std::string& fbx_path, const Matrix4& initialTransform) {
    namespace FS = std::experimental::filesystem;

    FS::path file_path_load(fbx_path);
    std::string folder_path_load = file_path_load.parent_path().string() + "\\";
    std::string basefilename_load = file_path_load.stem().string();

    MeshSkeleton* skeleton = nullptr;
    FS::path path_full = folder_path_load + basefilename_load + ".skel";
    bool fileExists = FS::exists(path_full);
    bool foundBinaryFile = fileExists && FS::file_size(path_full) > 0;
    if(foundBinaryFile) {
        std::string p = folder_path_load + basefilename_load + ".skel";
        skeleton = CreateSkeletonFromEngineAsset(p);
        skeleton->SetLocalTransform(initialTransform);
        RegisterSkeleton(fbx_path, skeleton);
    } else {
        skeleton = new MeshSkeleton;
        if(FbxLoadSkeleton(skeleton, fbx_path.c_str())) {
            skeleton->SetLocalTransform(initialTransform);
        } else {
            delete skeleton;
            skeleton = nullptr;
            g_theConsole->WarnMsg("No skeleton found.");
            return nullptr;
        }
        ExportFBXSkeletonToEngineAsset(fbx_path, *skeleton);
        RegisterSkeleton(fbx_path, skeleton);
    }
    return skeleton;
}

MeshSkeleton* SimpleRenderer::CreateSkeletonFromEngineAsset(const std::string& asset_path) {
    FileUtils::FileBinaryStream fs;
    if(!fs.open_for_read(asset_path)) {
        g_theConsole->WarnMsg("Skeleton binary failed to open.");
        return nullptr;
    }

    MeshSkeleton* skeleton = new MeshSkeleton();
    if(!skeleton->read(fs)) {
        fs.close();
        delete skeleton;
        skeleton = nullptr;
        g_theConsole->WarnMsg("Skeleton binary failed to load.");
        return nullptr;
    }
    fs.close();

    return skeleton;
}

Mesh* SimpleRenderer::CreateOrGetMesh(const Model::Type& type, const std::string& fbx_path, const Matrix4& initialTransform /*= Matrix4::GetIdentity()*/, MeshSkeleton* skeleton /*= nullptr*/) {
    auto mesh_iter = _meshes.find(fbx_path);
    bool mesh_exists = mesh_iter != _meshes.end();
    if(mesh_exists) {
        return GetMesh(fbx_path);
    } else {
        return CreateMesh(type, fbx_path, initialTransform, skeleton);
    }
}

Mesh* SimpleRenderer::GetMesh(const std::string& fbx_path) {
    return _meshes[fbx_path];
}

Mesh* SimpleRenderer::CreateMesh(const Model::Type& type, const std::string& fbx_path, const Matrix4& initialTransform /*= Matrix4::GetIdentity()*/, MeshSkeleton* skeleton /*= nullptr*/) {
    namespace FS = std::experimental::filesystem;

    FS::path file_path_load(fbx_path);
    std::string folder_path_load = file_path_load.parent_path().string() + "\\";
    std::string basefilename_load = file_path_load.stem().string();

    Mesh* mesh = nullptr;

    FS::path path_full = folder_path_load + basefilename_load + ".mesh";
    bool fileExists = FS::exists(path_full);
    bool foundBinaryFile = fileExists && FS::file_size(path_full) > 0;
    if(foundBinaryFile) {
        std::string p = folder_path_load + basefilename_load + ".mesh";
        mesh = CreateMeshFromEngineAsset(p);
        mesh->SetLocalTransform(initialTransform);
        RegisterMesh(fbx_path, mesh);
    } else {
        MeshBuilder meshbuilder;
        switch(type) {
            case Model::Type::OBJ:
            {
                mesh = new Mesh;
                if(mesh->LoadObjFromFile(fbx_path)) {
                    mesh->SetLocalTransform(initialTransform);
                    ExportFBXMeshToEngineAsset(fbx_path, *mesh);
                    RegisterMesh(fbx_path, mesh);
                } else {
                    std::ostringstream ss;
                    ss << "OBJ at " << fbx_path << " failed to load.";
                    ERROR_AND_DIE(ss.str());
                }
                break;
            }
            case Model::Type::FBX:
            {
                if(FbxLoadMesh(meshbuilder, fbx_path.c_str(), skeleton)) {
                    mesh = new Mesh(meshbuilder);
                    mesh->SetLocalTransform(initialTransform);
                    ExportFBXMeshToEngineAsset(fbx_path, *mesh);
                    RegisterMesh(fbx_path, mesh);
                } else {
                    g_theConsole->WarnMsg("No model found.");
                    return nullptr;
                }
                break;
            }
            default:
                /* DO NOTHING */;
        }
    }
    return mesh;
}

Mesh* SimpleRenderer::CreateMeshFromEngineAsset(const std::string& asset_path) {
    FileUtils::FileBinaryStream fs;
    if(!fs.open_for_read(asset_path)) {
        g_theConsole->WarnMsg("Mesh binary failed to open.");
        return nullptr;
    }

    MeshBuilder meshbuilder;
    if(!meshbuilder.read(fs)) {
        fs.close();
        g_theConsole->WarnMsg("Mesh binary failed to read.");
        return nullptr;
    }
    fs.close();

    return new Mesh(meshbuilder);
}

Model* SimpleRenderer::CreateOrGetModel(const Model::Type& type, const std::string& folderpath, const Matrix4& initialTransform /*= Matrix4::GetIdentity()*/) {
    auto model_iter = _models.find(folderpath);
    bool model_exists = model_iter != _models.end();
    if(model_exists) {
        return GetModel(folderpath);
    } else {
        return CreateModel(type, folderpath, initialTransform);
    }
}

Model* SimpleRenderer::GetModel(const std::string& folderpath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    return _models[p.string()];
}

Model* SimpleRenderer::CreateModel(const Model::Type& type, const std::string& folderpath, const Matrix4& initialTransform /*= Matrix4::GetIdentity()*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    Model* model = new Model(type, this, p.string(), initialTransform);
    RegisterModel(p.string(), model);
    return model;
}

void SimpleRenderer::ExportFBXToEngineAsset(const std::string& fbx_path, const Mesh& mesh, const MeshMotion& motion, const MeshSkeleton& skeleton) {
    ExportFBXMeshToEngineAsset(fbx_path, mesh);
    ExportFBXSkeletonToEngineAsset(fbx_path, skeleton);
    ExportFBXMotionToEngineAsset(fbx_path, motion);
}

void SimpleRenderer::ExportFBXMotionToEngineAsset(const std::string& fbx_path, const MeshMotion& motion) {

    namespace FS = std::experimental::filesystem;

    FS::path file_path(fbx_path);
    std::string folder_path = file_path.parent_path().string() + "/";

    std::string basefilename = file_path.stem().string();
    FileUtils::FileBinaryStream fs;

    std::string motion_path = folder_path + basefilename + ".motion";
    if(fs.open_for_write(motion_path)) {
        motion.write(fs);
    }
    fs.close();
}

void SimpleRenderer::ExportFBXSkeletonToEngineAsset(const std::string& fbx_path, const MeshSkeleton& skeleton) {

    namespace FS = std::experimental::filesystem;

    FS::path file_path(fbx_path);
    std::string folder_path = file_path.parent_path().string() + "/";

    std::string basefilename = file_path.stem().string();
    FileUtils::FileBinaryStream fs;
    std::string skel_path = folder_path + basefilename + ".skel";
    if(fs.open_for_write(skel_path)) {
        skeleton.write(fs);
    }
    fs.close();
}

void SimpleRenderer::ExportFBXMeshToEngineAsset(const std::string& fbx_path, const Mesh& model) {

    namespace FS = std::experimental::filesystem;

    FS::path file_path(fbx_path);
    std::string folder_path = file_path.parent_path().string() + "/";

    std::string basefilename = file_path.stem().string();
    FileUtils::FileBinaryStream fs;
    std::string mesh_path = folder_path + basefilename + ".mesh";
    if(fs.open_for_write(mesh_path)) {
        model.GetBuilder()->write(fs);
    }
    fs.close();
}

void SimpleRenderer::EnableBlend(const BlendFactor& source, const BlendFactor& dest) {
    if(_current_blend_state != nullptr) {
        bool is_same_state = _blend_state.enabled &&
                             _blend_state.source_factor == source &&
                             _blend_state.dest_factor == dest;
        if(is_same_state) {
            return;
        }
    }
    delete _current_blend_state;
    _current_blend_state = nullptr;

    BlendDesc desc;
    desc.enable = true;
    desc.source_factor = source;
    desc.dest_factor = dest;

    BlendState* bs = new BlendState(_rhi_device, false, desc);
    _rhi_context->SetBlendState(bs);

    _blend_state.enabled = true;
    _blend_state.source_factor = source;
    _blend_state.dest_factor = dest;

    _current_blend_state = bs;

}
void SimpleRenderer::DisableBlend() {
    if(!_blend_state.enabled) {
        return;
    }

    delete _current_blend_state;
    _current_blend_state = nullptr;

    BlendState* bs = new BlendState(_rhi_device, false, BlendDesc());
    _rhi_context->SetBlendState(bs);

    _blend_state.enabled = false;
    _current_blend_state = bs;

}
void SimpleRenderer::EnableDepthStencil(bool depth /*= true*/
                                        , bool stencil /*= false*/
                                        , bool depth_write /*= true*/
                                        , bool stencil_read /*= false*/
                                        , bool stencil_write /*= false*/
                                        , const ComparisonFunction& depthPredicate /*= ComparisonFunction::LESS*/
                                        , std::pair<const StencilOperation&, const StencilOperation&> failFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
                                        , std::pair<const StencilOperation&, const StencilOperation&> failDepthFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
                                        , std::pair<const StencilOperation&, const StencilOperation&> passFrontBackOp /*= std::make_pair(StencilOperation::KEEP, StencilOperation::KEEP)*/
                                        , std::pair<const ComparisonFunction&, const ComparisonFunction&> stencilPredicateFrontBack /*= std::make_pair(ComparisonFunction::ALWAYS, ComparisonFunction::ALWAYS)*/
) {
    if (_current_depthstencil_state != nullptr) {
        bool is_same_state =
               _depthstencil_state.depth_enabled == depth
            && _depthstencil_state.depth_write == depth_write
            && _depthstencil_state.stencil_enabled == stencil
            && _depthstencil_state.stencil_read == stencil_read
            && _depthstencil_state.stencil_write == stencil_write
            && _depthstencil_state.stencil_failFrontOp == failFrontBackOp.first
            && _depthstencil_state.stencil_failBackOp == failFrontBackOp.second
            && _depthstencil_state.stencil_failDepthFrontOp == failDepthFrontBackOp.first
            && _depthstencil_state.stencil_failDepthBackOp == failDepthFrontBackOp.second
            && _depthstencil_state.stencil_passFrontOp == passFrontBackOp.first
            && _depthstencil_state.stencil_passBackOp == passFrontBackOp.second
            && _depthstencil_state.stencil_testFront == stencilPredicateFrontBack.first
            && _depthstencil_state.stencil_testBack == stencilPredicateFrontBack.second;
        if (is_same_state) {
            return;
        }
    }
    delete _current_depthstencil_state;
    _current_depthstencil_state = nullptr;

    DepthStencilState* ds = new DepthStencilState(_rhi_device,
                                                  depth,
                                                  stencil,
                                                  depth_write,
                                                  stencil_read,
                                                  stencil_write,
                                                  depthPredicate,
                                                  failFrontBackOp,
                                                  failDepthFrontBackOp,
                                                  passFrontBackOp,
                                                  stencilPredicateFrontBack);
    _current_depthstencil_state = ds;
    _rhi_context->SetDepthStencilState(ds);

    _depthstencil_state.depth_enabled = depth;
    _depthstencil_state.depth_write = depth_write;
    _depthstencil_state.stencil_enabled = stencil;
    _depthstencil_state.stencil_read = stencil_read;
    _depthstencil_state.stencil_write = stencil_write;
    _depthstencil_state.stencil_failFrontOp = failFrontBackOp.first;
    _depthstencil_state.stencil_failBackOp = failFrontBackOp.second;
    _depthstencil_state.stencil_failDepthFrontOp = failDepthFrontBackOp.first;
    _depthstencil_state.stencil_failDepthBackOp = failDepthFrontBackOp.second;
    _depthstencil_state.stencil_passFrontOp = passFrontBackOp.first;
    _depthstencil_state.stencil_passBackOp = passFrontBackOp.second;
    _depthstencil_state.stencil_testFront = stencilPredicateFrontBack.first;
    _depthstencil_state.stencil_testBack = stencilPredicateFrontBack.second;

}
void SimpleRenderer::DisableDepthStencil() {
    if (!_depthstencil_state.depth_enabled && !_depthstencil_state.stencil_enabled) {
        return;
    }
    delete _current_depthstencil_state;
    _current_depthstencil_state = nullptr;

    DepthStencilState* ds = new DepthStencilState(_rhi_device, false, false, false, false, false);
    _rhi_context->SetDepthStencilState(ds);

    _depthstencil_state.depth_enabled = false;
    _depthstencil_state.depth_write = false;
    _depthstencil_state.stencil_enabled = false;
    _depthstencil_state.stencil_read = false;
    _depthstencil_state.stencil_write = false;
    _current_depthstencil_state = ds;
}
void SimpleRenderer::ReloadShaderPrograms(RHIDevice* device, const std::string& shaderprogram_path) {
    std::vector<std::string> shader_names;
    for(auto& s : _shaderPrograms) {
        shader_names.push_back(s.first);
        delete s.second;
        s.second = nullptr;
    }
    std::vector<ShaderProgram*> progs;
    for(auto& p : shader_names) {
        if (p.find_first_of("__") == 0) {
            continue;
        }
        ShaderProgram* prog = nullptr;
        std::string s_path = shaderprogram_path + p;
        RegisterShaderProgramFromFile(device, s_path, prog);
        _shaderPrograms.insert_or_assign(p, prog);
    }
    CreateAndRegisterDefaultShaderPrograms();
}
bool SimpleRenderer::RegisterShaderProgram(const std::string& name, ShaderProgram* program) {
    auto shader_iter = _shaderPrograms.find(name);
    bool shader_exists = shader_iter != _shaderPrograms.end();
    if(shader_exists) {
        delete shader_iter->second;
        shader_iter->second = nullptr;
        if(program == nullptr) {
            return false;
        }
    }
    _shaderPrograms.insert_or_assign(name, program);
    return true;
}
bool SimpleRenderer::RegisterShaderProgramFromFile(RHIDevice* device, const std::string& filepath, ShaderProgram*& output_program) {
    output_program = device->CreateShaderFromHlslFile(filepath);
    return output_program != nullptr;
}

bool SimpleRenderer::RegisterTexturesFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    { //Avoid pollution by error code.
        std::error_code ec;
        if (!FS::exists(folderpath) || !FS::is_directory(p, ec)) {
            std::ostringstream ss;
            ss << "SimpleRenderer::RegisterTexturesFromFolder: \"" << folderpath << "\" does not exist or is not a directory. Filesystem reported the following error: " << ec.message() << "\n";
            g_theFileLogger->LogErrorf(ss.str().c_str());
            g_theFileLogger->LogFlush();
            ERROR_AND_DIE(ss.str().c_str());
        }
    }
    if (!recursive) {
        for (FS::directory_iterator dir_iter(p); dir_iter != FS::directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(FS::is_directory(current_path)) continue;
            if(!IsSupportedImageType(current_path.extension().string())) continue;
            CreateOrGetTexture(current_path.string());
        }
        return true;
    }
    else {
        for (FS::recursive_directory_iterator dir_iter(p); dir_iter != FS::recursive_directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(!IsSupportedImageType(current_path.extension().string())) continue;
            CreateOrGetTexture(current_path.string());
        }
        return true;
    }
}

bool SimpleRenderer::RegisterComputeShaderFromFile(RHIDevice* device, const std::string& filepath, ComputeShader*& output_program) {
    output_program = device->CreateComputeShaderFromHlslFile(filepath);
    return output_program != nullptr;
}

bool SimpleRenderer::RegisterComputeShadersFromFolder(RHIDevice* device, const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    { //Avoid pollution by error code.
        std::error_code ec;
        if(!FS::exists(folderpath) || !FS::is_directory(p, ec)) {
            std::ostringstream ss;
            ss << "ShaderProgram::RegisterShadersFromFolder: \"" << folderpath << "\" does not exist or is not a directory. Filesystem reported the following error: " << ec.message() << "\n";
            g_theFileLogger->LogErrorf(ss.str().c_str());
            g_theFileLogger->LogFlush();
            ERROR_AND_DIE(ss.str().c_str());
        }
    }
    if(!recursive) {
        for(FS::directory_iterator dir_iter(p); dir_iter != FS::directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(FS::is_directory(current_path)) continue;
            ComputeShader* current_program = nullptr;
            if(!RegisterComputeShaderFromFile(device, current_path.string(), current_program)) {
                return false;
            }
            _compute_shaders.insert_or_assign(current_path.filename().string(), current_program);
        }
        return true;
    } else {
        for(FS::recursive_directory_iterator dir_iter(p); dir_iter != FS::recursive_directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(current_path.extension() != ".hlsl") continue;
            ComputeShader* current_program = nullptr;
            if(!RegisterComputeShaderFromFile(device, current_path.string(), current_program)) {
                return false;
            }
            _compute_shaders.insert_or_assign(current_path.filename().string(), current_program);
        }
        return true;
    }
}

bool SimpleRenderer::RegisterMaterial(const std::string& name, Material* mat) {
    auto material_iter = _materials.find(name);
    bool material_exists = material_iter != _materials.end();
    if(material_exists) {
        delete material_iter->second;
        material_iter->second = nullptr;
        if(mat == nullptr) {
            return false;
        }
    }
    _materials.insert_or_assign(name, mat);
    return true;
}

bool SimpleRenderer::RegisterMaterialFromFile(const std::string& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    tinyxml2::XMLDocument doc;
    auto load_result = doc.LoadFile(p.string().c_str());
    bool success = load_result == tinyxml2::XML_SUCCESS;
    if(success) {
        Material* mat = new Material(this, *doc.RootElement());
        RegisterMaterial(p.string(), mat);
        return success;
    }
    return success;
}

bool SimpleRenderer::RegisterMaterialsFromFolder(const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    { //Avoid pollution by error code.
        std::error_code ec;
        if(!FS::exists(folderpath) || !FS::is_directory(p, ec)) {
            std::ostringstream ss;
            ss << "Material::RegisterMaterialsFromFolder: \"" << folderpath << "\" does not exist or is not a directory. Filesystem reported the following error: " << ec.message() << "\n";
            g_theFileLogger->LogErrorf(ss.str().c_str());
            g_theFileLogger->LogFlush();
            ERROR_AND_DIE(ss.str().c_str());
        }
    }
    if(!recursive) {
        for(FS::directory_iterator dir_iter(p); dir_iter != FS::directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(FS::is_directory(current_path)) continue;
            if(current_path.extension() != ".material") continue;
            if(!RegisterMaterialFromFile(current_path.string())) {
                return false;
            }
        }
        return true;
    } else {
        for(FS::recursive_directory_iterator dir_iter(p); dir_iter != FS::recursive_directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(current_path.extension() != ".material") continue;
            if(!RegisterMaterialFromFile(current_path.string())) {
                return false;
            }
        }
        return true;
    }
}

bool SimpleRenderer::RegisterShaderProgramsFromFolder(RHIDevice* device, const std::string& folderpath, bool recursive /*= false*/) {
    namespace FS = std::experimental::filesystem;
    FS::path p(folderpath);
    { //Avoid pollution by error code.
        std::error_code ec;
        if(!FS::exists(folderpath) || !FS::is_directory(p, ec)) {
            std::ostringstream ss;
            ss << "ShaderProgram::RegisterShadersFromFolder: \"" << folderpath << "\" does not exist or is not a directory. Filesystem reported the following error: " << ec.message() << "\n";
            g_theFileLogger->LogErrorf(ss.str().c_str());
            g_theFileLogger->LogFlush();
            ERROR_AND_DIE(ss.str().c_str());
        }
    }
    if(!recursive) {
        for(FS::directory_iterator dir_iter(p); dir_iter != FS::directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(current_path.extension() != ".hlsl") continue;
            ShaderProgram* current_program = nullptr;
            if(!RegisterShaderProgramFromFile(device, current_path.string(), current_program)) {
                return false;
            }
            _shaderPrograms.insert_or_assign(current_path.string(), current_program);
        }
        return true;
    } else {
        for(FS::recursive_directory_iterator dir_iter(p); dir_iter != FS::recursive_directory_iterator{} /* defaults to end iter */; ++dir_iter) {
            FS::path current_path = dir_iter->path();
            if(current_path.extension() != ".hlsl") continue;
            ShaderProgram* current_program = nullptr;
            if(!RegisterShaderProgramFromFile(device, current_path.string(), current_program)) {
                return false;
            }
            _shaderPrograms.insert_or_assign(current_path.string(), current_program);
        }
        return true;
    }
}

void SimpleRenderer::DrawTextLine(KerningFont* f, const std::string& text, const Vector2& bottomLeftStartPos /*= Vector2::ZERO*/, const Rgba& color /*= Rgba::WHITE*/, float scale /*= 1.0f*/) {
    DrawTextLine(f, text, color, bottomLeftStartPos.x, bottomLeftStartPos.y, scale);
}

void SimpleRenderer::DrawTextLine(KerningFont* f, const std::string& text, const Rgba& color /*= Rgba::WHITE*/, float sx /*= 0.0f*/, float sy /*= 0.0f*/, float /*scale*/ /*= 1.0f*/) {
    if(f == nullptr) {
        return;
    }

    namespace FS = std::experimental::filesystem;

    float lineHeight = static_cast<float>(f->GetLineHeight());
    //float inv_lineHeight = 1.0f / lineHeight;
    float cursor_x = sx;
    float cursor_y = sy;
    float line_top = cursor_y - f->_common.base;
    float line_bottom = line_top + lineHeight;
    line_bottom; //UNUSED
    float texture_w = static_cast<float>(f->_common.scaleW);
    float texture_h = static_cast<float>(f->_common.scaleH);

    std::vector<Vertex3D> font_vbo;
    font_vbo.reserve(text.size() * 4);
    std::vector<unsigned int> font_ibo;
    font_ibo.reserve(text.size() * 6);

    Texture2D* texture = nullptr;
    for(auto char_iter = text.begin(); char_iter != text.end(); /* DO NOTHING */) {
        KerningFont::CharDef current_charDef = f->GetCharDef(*char_iter);
        if(!texture) {
            texture = GetTexture(f->_image_paths[current_charDef.page]);
        } else {
            Texture2D* texture2 = GetTexture(f->_image_paths[current_charDef.page]);
            ASSERT_OR_DIE(texture == texture2, "Font characters not on single page!");
        }

        //Get Font Texture UVs
        float char_uvl = current_charDef.x / texture_w;
        float char_uvt = current_charDef.y / texture_h;
        float char_uvr = char_uvl + (current_charDef.width / texture_w);
        float char_uvb = char_uvt + (current_charDef.height / texture_h);

        //Build Quad from cursor position
        float quad_top = line_top + current_charDef.yoffset;
        float quad_bottom = quad_top + static_cast<float>(current_charDef.height);
        float quad_left = cursor_x + static_cast<float>(current_charDef.xoffset);
        float quad_right = quad_left + current_charDef.width;

        font_vbo.emplace_back(Vector3(quad_left, quad_bottom, 0.0), color, Vector2(char_uvl, char_uvb), -Vector3::Z_AXIS, Vector3::X_AXIS, Vector3::Vector3::Y_AXIS);
        font_vbo.emplace_back(Vector3(quad_left, quad_top, 0.0), color, Vector2(char_uvl, char_uvt), -Vector3::Z_AXIS, Vector3::X_AXIS, Vector3::Vector3::Y_AXIS);
        font_vbo.emplace_back(Vector3(quad_right, quad_top, 0.0), color, Vector2(char_uvr, char_uvt), -Vector3::Z_AXIS, Vector3::X_AXIS, Vector3::Vector3::Y_AXIS);
        font_vbo.emplace_back(Vector3(quad_right, quad_bottom, 0.0), color, Vector2(char_uvr, char_uvb), -Vector3::Z_AXIS, Vector3::X_AXIS, Vector3::Vector3::Y_AXIS);

        std::size_t s = font_vbo.size();
        font_ibo.push_back(s - 4);
        font_ibo.push_back(s - 3);
        font_ibo.push_back(s - 2);
        font_ibo.push_back(s - 4);
        font_ibo.push_back(s - 2);
        font_ibo.push_back(s - 1);

        auto previous_char = char_iter;
        ++char_iter;
        if(char_iter != text.end()) {
            auto kern_iter = f->_kernmap.find(std::make_pair(*previous_char, *char_iter));
            float kern_value = 0.0f;
            if(kern_iter != f->_kernmap.end()) {
                kern_value = static_cast<float>(kern_iter->second);
            }
            cursor_x += (current_charDef.xadvance + kern_value);
        }
    }
    if(texture) {
        std::ostringstream ss;
        FS::path p(f->_filepath);
        p.replace_extension(".material");
        ss << p.string();
        SetMaterial(GetMaterial(ss.str()));
        UpdateVbo(font_vbo);
        UpdateIbo(font_ibo);
        DrawIndexed(PrimitiveType::TRIANGLES, _temp_vbo, _temp_ibo, font_ibo.size());
    }

}

void SimpleRenderer::DrawTextLine(const BitmapFont& font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect, const Rgba& tint /*= Rgba::WHITE*/, const FontJustification& justification /*= FontJustification::LEFT*/) {
    Vector2 current_glyph_position = bottomLeftStartPos;
    float glyph_width = fontHeight * fontAspect;
    float glyph_height = fontHeight;
    float draw_direction = 1.0f;
    Vector2 glyph_offset(glyph_width, 0.0f);
    Vector2 glyph_upper_right_offset(glyph_width, glyph_height);
    float text_height = glyph_height;
    float text_width = BitmapFont::CalcWidth(text, glyph_height, fontAspect);
    float half_extents_width = text_width * 0.5f;
    float half_extents_height = text_height * 0.5f;
    std::string string_copy(text.begin(), text.end());
    
    if((justification & FontJustification::LEFT) == FontJustification::LEFT) {
        draw_direction = 1.0f;
    }
    if((justification & FontJustification::CENTERH) == FontJustification::CENTERH) {
        current_glyph_position.x -= half_extents_width;
        draw_direction = 1.0f;
    }
    if((justification & FontJustification::RIGHT) == FontJustification::RIGHT) {
        draw_direction = -1.0f;
        string_copy = std::string(text.rbegin(), text.rend());
    }
    if((justification & FontJustification::TOP) == FontJustification::TOP) {
        current_glyph_position.y -= half_extents_height;
        draw_direction = 1.0f;
    }
    if((justification & FontJustification::CENTERV) == FontJustification::CENTERV) {
        current_glyph_position.y -= half_extents_height;
        draw_direction = 1.0f;
    }
    DisableDepthStencil();
    for(const auto& c : string_copy) {
        AABB2 current_glyph_bounds(current_glyph_position, current_glyph_position + glyph_upper_right_offset);
        AABB2 glyph_coords = font.GetSheet()->GetTexCoordsForSpriteIndex(static_cast<int>(c));
        Texture2D* t = &const_cast<Texture2D&>(font.GetSheet()->GetTexture2D());
        SetTexture(t);
        //DrawQuad(current_glyph_bounds.maxs.y, current_glyph_bounds.mins.x, 0.0f, current_glyph_bounds.mins.y, current_glyph_bounds.maxs.x, 0.0f, tint);
        DrawQuad(Vector3(current_glyph_bounds.mins.x + half_extents_width, current_glyph_bounds.maxs.y - half_extents_height, 0.0f), Vector3(half_extents_width, half_extents_height, 0.0f), tint, Vector4(glyph_coords.mins.y, glyph_coords.mins.x, glyph_coords.maxs.y, glyph_coords.maxs.x));
        //DrawTexturedAABB(current_glyph_bounds, font.GetSheet()->GetTexture(), glyph_coords.mins, glyph_coords.maxs, tint);
        current_glyph_position += glyph_offset * draw_direction;
    }
}

void SimpleRenderer::DrawMultilineText(const BitmapFont& font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float fontAspect, const Rgba& tint /*= Rgba::WHITE*/, const FontJustification& justification /*= FontJustification::LEFT*/) {
    std::regex textlineRegex("(.+)+");
    auto textlineBegin = std::sregex_iterator(text.begin(), text.end(), textlineRegex);
    auto textlineEnd = std::sregex_iterator();
    float lineIndex = 0.0f;
    for(auto i = textlineBegin; i != textlineEnd; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        DrawTextLine(font, match_str, bottomLeftStartPos + Vector2(0.0f, (lineIndex * fontHeight)), fontHeight, fontAspect, tint, justification);
        lineIndex += 1.0f;
    }
}

void SimpleRenderer::DrawMultilineText(KerningFont* font, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float /*fontAspect*/ /*= 1.0f*/, const Rgba& tint /*= Rgba::WHITE*/, const FontJustification& /*justification*/ /*= FontJustification::LEFT*/) {
    std::regex textlineRegex("(.+)+");
    auto textlineBegin = std::sregex_iterator(text.begin(), text.end(), textlineRegex);
    auto textlineEnd = std::sregex_iterator();
    float lineIndex = 0.0f;
    for(auto i = textlineBegin; i != textlineEnd; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        //DrawTextLine(font, match_str, bottomLeftStartPos - Vector2(0.0f, (lineIndex * fontHeight)), fontHeight, fontAspect, tint, justification);
        Vector2 pos = bottomLeftStartPos + Vector2(0.0f, (lineIndex * fontHeight));
        DrawTextLine(font, match_str, tint, pos.x, pos.y);
        lineIndex += 1.0f;
    }
}

KerningFont* SimpleRenderer::GetFont(const std::string& name) {
    return _fonts[name];
}

KerningFont* SimpleRenderer::CreateFontFromXML(const std::string& name) {
    namespace FS = std::experimental::filesystem;
    FS::path font_path("Data/Fonts/");
    font_path.append(name);
    font_path.replace_extension(".fnt");
    if(FS::exists(font_path)) {
        auto* f = new KerningFont();
        if(f->LoadFromFile(font_path.string())) {
            CreateMaterialFromFont(name, f);
            _fonts.insert_or_assign(name, f);
            return f;
        }
        delete f;
        return nullptr;
    }
    return nullptr;
}
void SimpleRenderer::CreateMaterialFromFont(const std::string& name, KerningFont* f) {
    namespace FS = std::experimental::filesystem;
    FS::path material_path = std::string("Data/Fonts/");
    material_path.append(name);
    material_path.replace_extension(".material");

    std::string material_str = "<material name=\"";
    material_str += name;
    material_str += std::string("\">");
    material_str += std::string("<shader src=\"__2D\" />");
    material_str += std::string("<textures>");
    std::ostringstream texture_ss;
    for(auto& image : f->_image_paths) {
        CreateTexture(image);
        texture_ss << "<diffuse src=\"" << image << "\" />";
    }
    material_str += texture_ss.str();
    material_str += std::string("</textures>");
    material_str += "</material>";
    {
        tinyxml2::XMLDocument doc;
        doc.Parse(material_str.c_str());
        Material* mat = new Material(this, *doc.RootElement());
        RegisterMaterial(material_path.string(), mat);
        doc.SaveFile(material_path.string().c_str());
    }
}
Texture2D* SimpleRenderer::CreateOrGetTexture(const std::string& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    auto texture_iter = _textures.find(p.string());
    if(texture_iter == _textures.end()) {
        return CreateTexture(filepath);
    } else {
        return GetTexture(filepath);
    }
}
Texture2D* SimpleRenderer::GetTexture(const std::string& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    return _textures[p.string()];
}

Texture2D* SimpleRenderer::CreateTexture2DFromImage(const Image* image,
                                                  const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/,
                                                  const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/,
                                                  const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {
    return CreateTexture2DFromMemory(image->GetData(), image->GetDimensions().x, image->GetDimensions().y, bufferUsage, bindUsage, imageFormat);
}


Texture2D* SimpleRenderer::CreateTexture2DFromMemory(const unsigned char* data,
                                                   unsigned int width /*= 1*/,
                                                   unsigned int height /*= 1*/,
                                                   const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/,
                                                   const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/,
                                                   const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {

    D3D11_TEXTURE2D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if(bindUsage == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    subresource_data.pSysMem = data;
    subresource_data.SysMemPitch = width * sizeof(unsigned char);
    subresource_data.SysMemSlicePitch = width * height * sizeof(unsigned char);

    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || !isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}


Texture2D* SimpleRenderer::CreateTexture2DFromMemory(const std::vector<Rgba>& data,
                                                   unsigned int width /*= 1*/,
                                                   unsigned int height /*= 1*/,
                                                   const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/,
                                                   const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/,
                                                   const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {

    D3D11_TEXTURE2D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = BufferUsageToD3DUsage(bufferUsage);
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    //Make every texture a target and shader resource
    tex_desc.BindFlags |= BufferBindUsageToD3DBindFlags(BufferBindUsage::SHADER_RESOURCE);
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);
    //Force specific usages for unordered access
    if((bindUsage & BufferBindUsage::UNORDERED_ACCESS) == BufferBindUsage::UNORDERED_ACCESS) {
        tex_desc.Usage = BufferUsageToD3DUsage(BufferUsage::GPU);
        tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(BufferUsage::STAGING);
    }
    tex_desc.MiscFlags = 0;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA subresource_data;
    memset(&subresource_data, 0, sizeof(subresource_data));

    subresource_data.pSysMem = data.data();
    subresource_data.SysMemPitch = width * sizeof(Rgba);
    subresource_data.SysMemSlicePitch = width * height * sizeof(Rgba);

    ID3D11Texture2D* dx_tex = nullptr;

    //If IMMUTABLE or not multi-sampled, must use initial data.
    bool isMultiSampled = tex_desc.SampleDesc.Count != 1 || tex_desc.SampleDesc.Quality != 0;
    bool isImmutable = bufferUsage == BufferUsage::STATIC;
    bool mustUseInitialData = isImmutable || isMultiSampled;

    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, (mustUseInitialData ? &subresource_data : nullptr), &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        return new Texture2D(_rhi_device, dx_tex);
    } else {
        return nullptr;
    }
}

Texture2D* SimpleRenderer::CreateTexture(const std::string& filepath,
                                         const BufferUsage& bufferUsage /*= BufferUsage::STATIC*/,
                                         const BufferBindUsage& bindUsage /*= BufferBindUsage::SHADER_RESOURCE*/,
                                         const ImageFormat& imageFormat /*= ImageFormat::R8G8B8A8_UNORM*/) {

    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(!FS::exists(p)) {
        return GetTexture("__invalid");
    }
    Image img = Image(p.string());

    D3D11_TEXTURE2D_DESC tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));

    tex_desc.Width =  img.GetDimensions().x;     // width... 
    tex_desc.Height = img.GetDimensions().y;    // ...and height of image in pixels.
    tex_desc.MipLevels = 1;    // setting to 0 means there's a full chain (or can generate a full chain) - we're immutable, so not allowed
    tex_desc.ArraySize = 1;    // only one texture (
    tex_desc.Usage =  BufferUsageToD3DUsage(bufferUsage);            // data is set at creation time and won't change
    tex_desc.Format = ImageFormatToDxgiFormat(imageFormat);      // R8G8B8A8 texture
    tex_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);   // we're going to be using this texture as a shader resource
    tex_desc.CPUAccessFlags = CPUAccessFlagFromUsage(bufferUsage);                      // Determines how I can access this resource CPU side (IMMUTABLE, So none)
    tex_desc.MiscFlags = 0;                            // Extra Flags, of note is;
                                                       // D3D11_RESOURCE_MISC_GENERATE_MIPS - if we want to use this to be able to generate mips (not compatible with IMMUTABLE)

                                                       // If Multisampling - set this up - we're not multisampling, so 1 and 0
                                                       // (MSAA as far as I know only makes sense for Render Targets, not shader resource textures)
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;

    // Setup Initial Data
    D3D11_SUBRESOURCE_DATA data;
    memset(&data, 0, sizeof(data));

    auto width = img.GetDimensions().x;
    auto height = img.GetDimensions().y;
    data.pSysMem =     img.GetData();
    data.SysMemPitch = width * sizeof(unsigned int); // pitch is byte size of a single row)
    data.SysMemSlicePitch = width * height * sizeof(unsigned int);
    tex_desc.Usage = D3D11_USAGE_IMMUTABLE;

    ID3D11Texture2D* dx_tex = nullptr;
    HRESULT hr = _rhi_device->GetDxDevice()->CreateTexture2D(&tex_desc, &data, &dx_tex);
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {
        auto* tex = new Texture2D(_rhi_device, dx_tex);
        tex->SetDebugName(p.string().c_str());
        tex->IsLoaded(true);
        if(RegisterTexture(p.string(), tex)) {
            return tex;
        } else {
            delete tex;
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

ShaderProgram* SimpleRenderer::CreateShaderProgram(const std::string& filepath) {

    namespace FS = std::experimental::filesystem;
    
    FS::path p = filepath;
    if(FS::exists(p) == false) {
        return nullptr;
    }
    ShaderProgram* prog = _rhi_device->CreateShaderFromHlslFile(p.string());
    return prog;
}

bool SimpleRenderer::RegisterTexture(const std::string& name, Texture2D *texture) {
    auto found_texture = _textures.find(name);
    if(found_texture == _textures.end()) {
        _textures.insert_or_assign(name, texture);
        return true;
    } else {
        return false;
    }
    return false;
}
void SimpleRenderer::SetTexture(Texture2D* texture, unsigned int registerIndex /*= 0*/) {
    if(texture == nullptr) {
        texture = GetTexture("__default");
    }
    if(_current_target == texture) {
        return;
    }
    _rhi_context->SetTexture(registerIndex, texture);
}
void SimpleRenderer::RegisterSampler(const std::string& name, Sampler* sampler) {
    auto found_sampler = _samplers.find(name);
    if(found_sampler == _samplers.end()) {
        _samplers.insert_or_assign(name, sampler);
    }
}
Sampler* SimpleRenderer::GetSampler(const std::string& name) {
    return _samplers[name];
}
void SimpleRenderer::SetSampler(Sampler* sampler, unsigned int registerIndex /*= 0*/) {
    if(sampler == nullptr) {
        sampler = GetSampler("__default");
    }
    ID3D11SamplerState* dx_sampler = sampler->GetDxSampler();
    _rhi_context->GetDxContext()->PSSetSamplers(registerIndex, 1, &dx_sampler);
    _rhi_context->GetDxContext()->VSSetSamplers(registerIndex, 1, &dx_sampler);
    _rhi_context->GetDxContext()->CSSetSamplers(registerIndex, 1, &dx_sampler);
}
void SimpleRenderer::DrawTextLine(const PrimitiveType& topology, VertexBuffer* vbo, unsigned int const vertex_count) {
    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(Vertex3D);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_buffer = vbo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_buffer, &stride, &offsets);

    _rhi_context->GetDxContext()->Draw(vertex_count, 0);
}

void SimpleRenderer::DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, unsigned int const vertex_count) {

    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(Vertex3D);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_vbo_buffer = vbo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_vbo_buffer, &stride, &offsets);

    ID3D11Buffer* dx_ibo_buffer = ibo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetIndexBuffer(dx_ibo_buffer, DXGI_FORMAT_R32_UINT, 0U);

    _rhi_context->GetDxContext()->DrawIndexed(vertex_count, 0, 0);
}

void SimpleRenderer::Draw(const PrimitiveType& topology, VertexBuffer* vbo, unsigned int const vertex_count) {

    D3D11_PRIMITIVE_TOPOLOGY d3d_prim = PrimitiveTypeToD3dTopology(topology);
    _rhi_context->GetDxContext()->IASetPrimitiveTopology(d3d_prim);
    unsigned int stride = sizeof(Vertex3D);
    unsigned int offsets = 0;
    ID3D11Buffer* dx_vbo_buffer = vbo->GetDxBuffer();
    _rhi_context->GetDxContext()->IASetVertexBuffers(0, 1, &dx_vbo_buffer, &stride, &offsets);

    _rhi_context->GetDxContext()->Draw(vertex_count, 0);
}
FontJustification operator|(const FontJustification& lhs, const FontJustification& rhs) {
    int lhs_value = static_cast<int>(lhs);
    int rhs_value = static_cast<int>(rhs);
    return static_cast<FontJustification>(lhs_value | rhs_value);
}
FontJustification operator&(const FontJustification& lhs, const FontJustification& rhs) {
    int lhs_value = static_cast<int>(lhs);
    int rhs_value = static_cast<int>(rhs);
    return static_cast<FontJustification>(lhs_value & rhs_value);
}