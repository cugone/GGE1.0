#include "Engine/Renderer/Shader.hpp"

#include <filesystem>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"

Shader::Shader(SimpleRenderer* renderer)
    : _name("UNNAMED_SHADER")
    , _renderer(renderer)
    , _shaderProgram(nullptr)
    , _sampler(nullptr)
    , _blendState(nullptr)
    , _depthStencilState(nullptr)
    , _rasterState(nullptr)
{
    /* DO NOTHING */
}

Shader::Shader(SimpleRenderer* renderer, const XMLElement& element)
    : _name()
    , _renderer(renderer)
    , _shaderProgram(nullptr)
    , _sampler(nullptr)
    , _blendState(nullptr)
    , _depthStencilState(nullptr)
    , _rasterState(nullptr)
{
    LoadFromXML(element);
}

Shader::~Shader() {

    if(_sampler) {
        delete _sampler;
        _sampler = nullptr;
    }

    if(_rasterState) {
        delete _rasterState;
        _rasterState = nullptr;
    }

    if(_blendState) {
        delete _blendState;
        _blendState = nullptr;
    }

    if(_depthStencilState) {
        delete _depthStencilState;
        _depthStencilState = nullptr;
    }

}
void Shader::SetName(const std::string& name) {
    _name = name;
}

void Shader::SetSampler(Sampler* sampler) {
    _sampler = sampler;
}

void Shader::SetBlendState(BlendState* blendstate) {
    _blendState = blendstate;
}

void Shader::SetRasterState(RasterState* rasterState) {
    _rasterState = rasterState;
}

void Shader::SetDepthStencilState(DepthStencilState* depthstencilstate) {
    _depthStencilState = depthstencilstate;
}

void Shader::SetShaderProgram(ShaderProgram* shaderProgram) {
    _shaderProgram = shaderProgram;
}
const std::string& Shader::GetName() const {
    return _name;
}

ShaderProgram* Shader::GetShaderProgram() const {
    return _shaderProgram;
}

Sampler* Shader::GetSampler() const {
    return _sampler;
}

BlendState* Shader::GetBlendState() const {
    return _blendState;
}

DepthStencilState* Shader::GetDepthStencilState() const {
    return _depthStencilState;
}
RasterState* Shader::GetRasterState() const {
    return _rasterState;
}

bool Shader::LoadFromXML(const XMLElement& element) {
    namespace FS = std::experimental::filesystem;

    DataUtils::ValidateXmlElement(element, "shader", "shaderprogram", "name", "depth,stencil,blends,raster,sampler");

    _name = DataUtils::ParseXmlAttribute(element, std::string("name"), "UNNAMED_SHADER");

    auto xml_SP = element.FirstChildElement("shaderprogram");
    DataUtils::ValidateXmlElement(*xml_SP, "shaderprogram", "", "src");

    std::string sp_src = DataUtils::ParseXmlAttribute(*xml_SP, "src", "");
    if(sp_src.empty()) {
        ERROR_AND_DIE("shaderprogram element has empty src attribute.");
    }

    FS::path p(sp_src);
    auto program = _renderer->GetShaderProgram(p.string());
    if(program == nullptr) {
        ERROR_AND_DIE("ShaderProgram referenced in Shader file does not already exist.");
    }
    _shaderProgram = program;

    _depthStencilState = new DepthStencilState(_renderer->_rhi_device, element);

    _blendState = new BlendState(_renderer->_rhi_device, element);

    _rasterState = new RasterState(_renderer->_rhi_device, element);

    _sampler = new Sampler(_renderer->_rhi_device, element);

    return true;
}
