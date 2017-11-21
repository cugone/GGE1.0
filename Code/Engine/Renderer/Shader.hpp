#pragma once

#include <string>
#include <vector>

#include "Engine/Core/DataUtils.hpp"

class RHIDevice;
class RasterState;
class Sampler;
class BlendState;
class DepthStencilState;
class ShaderProgram;
class SimpleRenderer;

class Shader {
public:
	Shader(SimpleRenderer* renderer);
    Shader(SimpleRenderer* renderer, const XMLElement& element);
    ~Shader();

    void SetName(const std::string& name);
    void SetSampler(Sampler* sampler);
    void SetBlendState(BlendState* blendstate);
    void SetRasterState(RasterState* rasterstate);
    void SetDepthStencilState(DepthStencilState* depthstencilstate);
    void SetShaderProgram(ShaderProgram* shaderProgram);

    const std::string& GetName() const;
    ShaderProgram* GetShaderProgram() const;
    Sampler* GetSampler() const;
    BlendState* GetBlendState() const;
    DepthStencilState* GetDepthStencilState() const;
    RasterState* GetRasterState() const;

protected:
    bool LoadFromXML(const XMLElement& element);
private:
    std::string _name;
    SimpleRenderer* _renderer;
    ShaderProgram* _shaderProgram;
    Sampler* _sampler;
    DepthStencilState* _depthStencilState;
    BlendState* _blendState;
    RasterState* _rasterState;
};