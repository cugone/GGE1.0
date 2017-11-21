#include "Engine/Renderer/RasterState.hpp"

#include <algorithm>
#include <locale>

#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHI.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/core/ErrorWarningAssert.hpp"

D3D11_FILL_MODE FillModeToD3DFillMode(const FillMode& fillmode);
D3D11_CULL_MODE CullModeToD3DCullMode(const CullMode& fillmode);
FillMode FillModeFromString(std::string str);
CullMode CullModeFromString(std::string str);

RasterState::RasterState(RHIDevice* device, const FillMode& fillmode /*= FillMode::SOLID*/, const CullMode& cullmode /*= CullMode::BACK*/, bool antiAliasing /*= false*/) {
    if(!CreateRasterState(device, fillmode, cullmode, antiAliasing)) {
        _dx_rasterizer->Release();
        _dx_rasterizer = nullptr;
        ERROR_AND_DIE("RasterState: dx Rasterizer failed to create.\n");
    }
}
RasterState::RasterState(RHIDevice* device, const XMLElement& element) {
    if(!LoadFromXML(device, element)) {
        _dx_rasterizer->Release();
        _dx_rasterizer = nullptr;
        ERROR_AND_DIE("RasterState: Load from XML failed.\n");
    }
}
RasterState::~RasterState() {
    _dx_rasterizer->Release();
    _dx_rasterizer = nullptr;
}
ID3D11RasterizerState* RasterState::GetDxRasterizer() {
    return _dx_rasterizer;
}

bool RasterState::LoadFromXML(RHIDevice* device, const XMLElement& element) {

    RasterDesc desc;
    desc.fillmode = FillMode::SOLID;
    desc.cullmode = CullMode::BACK;
    desc.antialiasedLineEnable = false;
    desc.depthClipEnable = true;
    desc.scissorEnable = false;

    auto xml_raster = element.FirstChildElement("raster");
    if(xml_raster != nullptr) {

        DataUtils::ValidateXmlElement(*xml_raster, "fill,cull", "", "antialiasing,depthbias,depthclip,scissor,msaa");
        auto xml_fill = xml_raster->FirstChildElement("fill");
        std::string fill_str = "solid";
        fill_str = DataUtils::ParseXmlElementText(*xml_fill, fill_str);
        desc.fillmode = FillModeFromString(fill_str);

        auto xml_cull = xml_raster->FirstChildElement("cull");
        std::string cull_str = "back";
        cull_str = DataUtils::ParseXmlElementText(*xml_cull, cull_str);
        desc.cullmode = CullModeFromString(cull_str);

        desc.antialiasedLineEnable = false;
        auto xml_antialiasing = xml_raster->FirstChildElement("antialiasing");
        if(xml_antialiasing != nullptr) {
            DataUtils::ValidateXmlElement(*xml_antialiasing, "", "");
            desc.antialiasedLineEnable = DataUtils::ParseXmlElementText(*xml_antialiasing, desc.antialiasedLineEnable);
        }

        desc.depthBias = 0;
        desc.depthBiasClamp = 0.0f;
        desc.slopeScaledDepthBias = 0.0f;
        auto xml_depthbias = xml_raster->FirstChildElement("depthbias");
        if(xml_depthbias != nullptr) {
            DataUtils::ValidateXmlElement(*xml_depthbias, "", "value,clamp,slopescaled");
            desc.depthBias = DataUtils::ParseXmlAttribute(*xml_depthbias, "value", desc.depthBias);
            desc.depthBiasClamp = DataUtils::ParseXmlAttribute(*xml_depthbias, "clamp", desc.depthBiasClamp);
            desc.slopeScaledDepthBias = DataUtils::ParseXmlAttribute(*xml_depthbias, "slopescaled", desc.slopeScaledDepthBias);
        }

        desc.depthClipEnable = true;
        auto xml_depthclip = xml_raster->FirstChildElement("depthclip");
        if(xml_depthclip != nullptr) {
            DataUtils::ValidateXmlElement(*xml_depthclip, "", "");
            desc.depthClipEnable = DataUtils::ParseXmlElementText(*xml_depthclip, desc.depthClipEnable);
        }

        desc.scissorEnable = false;
        auto xml_scissor = xml_raster->FirstChildElement("scissor");
        if(xml_scissor != nullptr) {
            DataUtils::ValidateXmlElement(*xml_scissor, "", "");
            desc.scissorEnable = DataUtils::ParseXmlElementText(*xml_scissor, desc.scissorEnable);
        }

        desc.multisampleEnable = false;
        auto xml_msaa = xml_raster->FirstChildElement("scissor");
        if(xml_msaa != nullptr) {
            DataUtils::ValidateXmlElement(*xml_msaa, "", "");
            desc.multisampleEnable = DataUtils::ParseXmlElementText(*xml_msaa, desc.multisampleEnable);
        }
    }

    return CreateRasterState(device, desc);
}
bool RasterState::CreateRasterState(RHIDevice* device, const RasterDesc& raster_desc /*= RasterDesc()*/) {

    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.FillMode = FillModeToD3DFillMode(raster_desc.fillmode);
    desc.CullMode = CullModeToD3DCullMode(raster_desc.cullmode);
    desc.FrontCounterClockwise = true;
    desc.AntialiasedLineEnable = raster_desc.antialiasedLineEnable;
    desc.DepthBias = raster_desc.depthBias;
    desc.DepthBiasClamp = raster_desc.depthBiasClamp;
    desc.SlopeScaledDepthBias = raster_desc.slopeScaledDepthBias;
    desc.DepthClipEnable = raster_desc.depthClipEnable;
    desc.ScissorEnable = raster_desc.scissorEnable;

    HRESULT hr = device->GetDxDevice()->CreateRasterizerState(&desc, &_dx_rasterizer);
    return SUCCEEDED(hr);
}

bool RasterState::CreateRasterState(RHIDevice* device, const FillMode& fillmode /*= FillMode::SOLID*/, const CullMode& cullmode /*= CullMode::BACK*/, bool antiAliasing /*= false*/) {

    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.FillMode = FillModeToD3DFillMode(fillmode);
    desc.CullMode = CullModeToD3DCullMode(cullmode);
    desc.FrontCounterClockwise = true;
    desc.AntialiasedLineEnable = antiAliasing;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0.0f;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;

    HRESULT hr = device->GetDxDevice()->CreateRasterizerState(&desc, &_dx_rasterizer);
    return SUCCEEDED(hr);
}

D3D11_FILL_MODE FillModeToD3DFillMode(const FillMode& fillmode) {
    switch(fillmode) {
        case FillMode::SOLID: return D3D11_FILL_SOLID;
        case FillMode::WIREFRAME:  return D3D11_FILL_WIREFRAME;
        default: return D3D11_FILL_SOLID;
    }
}

D3D11_CULL_MODE CullModeToD3DCullMode(const CullMode& cullmode) {
    switch(cullmode) {
        case CullMode::NONE: return D3D11_CULL_NONE;
        case CullMode::FRONT: return D3D11_CULL_FRONT;
        case CullMode::BACK: return D3D11_CULL_BACK;
        default: return D3D11_CULL_BACK;
    }
}

FillMode FillModeFromString(std::string str) {

    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)->unsigned char { return std::tolower(c, std::locale("")); });
    if(str == "solid") {
        return FillMode::SOLID;
    } else if(str == "wire" || str == "wireframe") {
        return FillMode::WIREFRAME;
    } else {
        return FillMode::SOLID;
    }
}

CullMode CullModeFromString(std::string str) {

    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)->unsigned char { return std::tolower(c, std::locale("")); });
    if(str == "none") {
        return CullMode::NONE;
    } else if(str == "front") {
        return CullMode::FRONT;
    } else if(str == "back") {
        return CullMode::BACK;
    } else {
        return CullMode::BACK;
    }
}
