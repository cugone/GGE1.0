#include "Engine/Renderer/BlendState.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <algorithm>
#include <locale>
#include <string>

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHI.hpp"


BlendColorWriteEnable operator~(const BlendColorWriteEnable& a) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    return static_cast<BlendColorWriteEnable>(~underlying_a);
}

BlendColorWriteEnable operator&(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a & underlying_b);
}

BlendColorWriteEnable operator|(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a | underlying_b);
}

BlendColorWriteEnable operator^(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
}

BlendColorWriteEnable& operator&=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {

    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a & underlying_b);
    return a;
}

BlendColorWriteEnable& operator|=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {

    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a | underlying_b);
    return a;
}

BlendColorWriteEnable& operator^=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) {

    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
    return a;
}

D3D11_BLEND BlendFactorToD3DBlendFactor(const BlendFactor& factor);
D3D11_BLEND_OP BlendOpToD3DBlendOp(const BlendOperation& op);
UINT8 BlendColorWriteEnableToD3DBlendColorWriteEnable(const BlendColorWriteEnable& rt_mask);

BlendFactor BlendFactorFromString(std::string str);
BlendOperation BlendOperationFromString(std::string str);
BlendColorWriteEnable BlendColorWriteEnableFromString(std::string str);

bool BlendState::LoadFromXML(RHIDevice* device, const XMLElement& element) {

    //Default values
    bool alphaToCoverageEnable = false;
    BlendDesc default_desc;

    auto xml_blends = element.FirstChildElement("blends");
    if(xml_blends != nullptr) {
        DataUtils::ValidateXmlElement(*xml_blends, "blend", "", "", "alphacoverage");
        alphaToCoverageEnable = DataUtils::ParseXmlAttribute(*xml_blends, "alphacoverage", false);

        std::vector<BlendDesc> desc;
        desc.reserve(DataUtils::GetChildElementCount(*xml_blends, "blend"));
        for(auto xml_blend = xml_blends->FirstChildElement("blend"); xml_blend != nullptr; xml_blend = xml_blend->NextSiblingElement("blend")) {
            DataUtils::ValidateXmlElement(*xml_blend, "", "", "color,alpha,enablemask", "enable");

            BlendDesc cur_desc;

            cur_desc.enable = DataUtils::ParseXmlAttribute(*xml_blend, "enable", false);

            auto xml_color = xml_blend->FirstChildElement("color");
            if(xml_color != nullptr) {
                DataUtils::ValidateXmlElement(*xml_color, "", "src,dest,op");
                std::string source_factor_str = "one";
                source_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "src", source_factor_str);
                std::string dest_factor_str = "zero";
                dest_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "dest", dest_factor_str);
                std::string op_str = "add";
                op_str = DataUtils::ParseXmlAttribute(*xml_color, "op", op_str);

                cur_desc.source_factor = BlendFactorFromString(source_factor_str);
                cur_desc.dest_factor = BlendFactorFromString(dest_factor_str);
                cur_desc.blend_op = BlendOperationFromString(op_str);
            }

            auto xml_alpha = xml_blend->FirstChildElement("alpha");
            if(xml_alpha != nullptr) {
                DataUtils::ValidateXmlElement(*xml_alpha, "", "src,dest,op");
                std::string source_factor_str = "one";
                source_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "src", source_factor_str);
                std::string dest_factor_str = "zero";
                dest_factor_str = DataUtils::ParseXmlAttribute(*xml_color, "dest", dest_factor_str);
                std::string op_str = "add";
                op_str = DataUtils::ParseXmlAttribute(*xml_color, "op", op_str);

                cur_desc.source_factor_alpha = BlendFactorFromString(source_factor_str);
                cur_desc.dest_factor_alpha = BlendFactorFromString(dest_factor_str);
                cur_desc.blend_op_alpha = BlendOperationFromString(op_str);
            }

            auto xml_mask = xml_blend->FirstChildElement("enablemask");
            if(xml_mask != nullptr) {
                DataUtils::ValidateXmlElement(*xml_mask, "", "value");
                std::string mask_str = "all";
                mask_str = DataUtils::ParseXmlAttribute(*xml_mask, "value", mask_str);

                cur_desc.blend_color_write_enable = BlendColorWriteEnableFromString(mask_str);
            }

            desc.push_back(cur_desc);
        }
        if(desc.size() > 8) {
            desc.erase(desc.begin() + 8, desc.end());
            desc.shrink_to_fit();
        }
        if(!desc.empty()) {
            return CreateBlendState(device, alphaToCoverageEnable, desc);
        }
    }
    return CreateBlendState(device, alphaToCoverageEnable, default_desc);
}

bool BlendState::CreateBlendState(RHIDevice* device
                                  , bool enable /* = false */
                                  , bool alphaToCoverageEnable /* = false*/
                                  , const BlendFactor& source_factor /*= BlendFactor::ONE*/
                                  , const BlendFactor& dest_factor /*= BlendFactor::ZERO*/
                                  , const BlendOperation& blend_op /*= BlendOperation::ADD*/
                                  , const BlendFactor& source_factor_alpha /*= BlendFactor::ONE*/
                                  , const BlendFactor& dest_factor_alpha /*= BlendFactor::ONE*/
                                  , const BlendOperation& blend_op_alpha /*= BlendOperation::ADD*/
                                  , const BlendColorWriteEnable& blend_color_write_enable /*= BlendRenderTargetWriteMask::ALL*/)
{
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.AlphaToCoverageEnable = alphaToCoverageEnable; //multisampling
    desc.IndependentBlendEnable = false; //multiple render targets

    desc.RenderTarget[0].BlendEnable = enable;
    desc.RenderTarget[0].BlendOp = BlendOpToD3DBlendOp(blend_op);
    desc.RenderTarget[0].SrcBlend = BlendFactorToD3DBlendFactor(source_factor);
    desc.RenderTarget[0].DestBlend = BlendFactorToD3DBlendFactor(dest_factor);

    desc.RenderTarget[0].BlendOpAlpha = BlendOpToD3DBlendOp(blend_op_alpha);
    desc.RenderTarget[0].SrcBlendAlpha = BlendFactorToD3DBlendFactor(source_factor_alpha);
    desc.RenderTarget[0].DestBlendAlpha = BlendFactorToD3DBlendFactor(dest_factor_alpha);

    desc.RenderTarget[0].RenderTargetWriteMask = BlendColorWriteEnableToD3DBlendColorWriteEnable(blend_color_write_enable);

    HRESULT hr = device->GetDxDevice()->CreateBlendState(&desc, &_dx_blendstate);
    return SUCCEEDED(hr);
}

bool BlendState::CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable /*= false*/, BlendDesc render_target /*= BlendDesc()*/) {
    return CreateBlendState(device, render_target.enable, alphaToCoverageEnable,
                            render_target.source_factor, render_target.dest_factor, render_target.blend_op,
                            render_target.source_factor_alpha, render_target.dest_factor_alpha, render_target.blend_op_alpha,
                            render_target.blend_color_write_enable);
}

bool BlendState::CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable /*= false*/, const std::vector<BlendDesc>& render_targets /*= {BlendDesc()}*/) {
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.AlphaToCoverageEnable = alphaToCoverageEnable;
    desc.IndependentBlendEnable = render_targets.size() > 1;

    for(std::size_t i = 0; i < 8 && i < render_targets.size(); ++i) {
        desc.RenderTarget[i].BlendEnable = render_targets[i].enable;
        desc.RenderTarget[i].BlendOp = BlendOpToD3DBlendOp(render_targets[i].blend_op);
        desc.RenderTarget[i].SrcBlend = BlendFactorToD3DBlendFactor(render_targets[i].source_factor);
        desc.RenderTarget[i].DestBlend = BlendFactorToD3DBlendFactor(render_targets[i].dest_factor);

        desc.RenderTarget[i].BlendOpAlpha = BlendOpToD3DBlendOp(render_targets[i].blend_op_alpha);
        desc.RenderTarget[i].SrcBlendAlpha = BlendFactorToD3DBlendFactor(render_targets[i].source_factor_alpha);
        desc.RenderTarget[i].DestBlendAlpha = BlendFactorToD3DBlendFactor(render_targets[i].dest_factor_alpha);

        desc.RenderTarget[i].RenderTargetWriteMask = BlendColorWriteEnableToD3DBlendColorWriteEnable(render_targets[i].blend_color_write_enable);
    }
    HRESULT hr = device->GetDxDevice()->CreateBlendState(&desc, &_dx_blendstate);
    return SUCCEEDED(hr);
}

BlendState::BlendState(RHIDevice* device, const XMLElement& element)
    : _dx_blendstate(nullptr)
{
    if(!LoadFromXML(device, element)) {
        _dx_blendstate->Release();
        _dx_blendstate = nullptr;
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::BlendState(RHIDevice* device,
                       bool alphaToCoverageEnable /*= false*/,
                       BlendDesc desc /*= BlendDesc()*/)
    : _dx_blendstate(nullptr)
{
    if(!CreateBlendState(device, alphaToCoverageEnable, desc)) {
        _dx_blendstate->Release();
        _dx_blendstate = nullptr;
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::BlendState(RHIDevice* device,
                       bool enable /* = false */,
                       bool alphaToCoverageEnable /* = false*/,
                       const BlendFactor& source_factor /*= BlendFactor::ONE*/,
                       const BlendFactor& dest_factor /*= BlendFactor::ZERO*/,
                       const BlendOperation& blend_op /*= BlendOperation::ADD*/,
                       const BlendFactor& source_factor_alpha /*= BlendFactor::ONE*/,
                       const BlendFactor& dest_factor_alpha /*= BlendFactor::ONE*/,
                       const BlendOperation& blend_op_alpha /*= BlendOperation::ADD*/,
                       const BlendColorWriteEnable& blend_color_write_enable /*= BlendRenderTargetMask::ALL*/)
    : _dx_blendstate(nullptr)
{
    if(!CreateBlendState(device
                         , enable
                         , alphaToCoverageEnable
                         , source_factor
                         , dest_factor
                         , blend_op
                         , source_factor_alpha
                         , dest_factor_alpha
                         , blend_op_alpha
                         , blend_color_write_enable))
    {
        _dx_blendstate->Release();
        _dx_blendstate = nullptr;
        ERROR_AND_DIE("\nBlendState: Failed to create.\n");
    }
}

BlendState::~BlendState() {
    _dx_blendstate->Release();
    _dx_blendstate = nullptr;
}

ID3D11BlendState* BlendState::GetDxBlendState() {
    return _dx_blendstate;
}

D3D11_BLEND BlendFactorToD3DBlendFactor(const BlendFactor& factor) {
    switch(factor) {
        case BlendFactor::ZERO: return D3D11_BLEND_ZERO;
        case BlendFactor::ONE:  return D3D11_BLEND_ONE;
        case BlendFactor::SRC_COLOR: return D3D11_BLEND_SRC_COLOR;
        case BlendFactor::INV_SRC_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
        case BlendFactor::SRC_ALPHA: return D3D11_BLEND_SRC_ALPHA;
        case BlendFactor::INV_SRC_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DEST_ALPHA: return D3D11_BLEND_DEST_ALPHA;
        case BlendFactor::INV_DEST_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
        case BlendFactor::DEST_COLOR: return D3D11_BLEND_DEST_COLOR;
        case BlendFactor::INV_DEST_COLOR: return D3D11_BLEND_INV_DEST_COLOR;
        case BlendFactor::SRC_ALPHA_SAT: return D3D11_BLEND_SRC_ALPHA_SAT;
        case BlendFactor::BLEND_FACTOR: return D3D11_BLEND_BLEND_FACTOR;
        case BlendFactor::INV_BLEND_FACTOR: return D3D11_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::SRC1_COLOR: return D3D11_BLEND_SRC1_COLOR;
        case BlendFactor::INV_SRC1_COLOR: return D3D11_BLEND_INV_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA: return D3D11_BLEND_SRC1_ALPHA;
        case BlendFactor::INV_SRC1_ALPHA: return D3D11_BLEND_INV_SRC1_ALPHA;
        default: ERROR_AND_DIE("BlendFactor not defined.");
    }
}

D3D11_BLEND_OP BlendOpToD3DBlendOp(const BlendOperation& op) {
    switch(op) {
        case BlendOperation::ADD: return D3D11_BLEND_OP_ADD;
        case BlendOperation::SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
        case BlendOperation::REVERSE_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
        case BlendOperation::MIN: return D3D11_BLEND_OP_MIN;
        case BlendOperation::MAX: return D3D11_BLEND_OP_MAX;
        default: ERROR_AND_DIE("BlendOperation not defined.");
    }
}

UINT8 BlendColorWriteEnableToD3DBlendColorWriteEnable(const BlendColorWriteEnable& rt_mask) {
    return static_cast<UINT8>(rt_mask);
}

BlendFactor BlendFactorFromString(std::string str) {

    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c, std::locale("")); });
    if(str == "zero") {
        return BlendFactor::ZERO;
    } else if(str == "one") {
        return BlendFactor::ONE;
    } else if(str == "src_color") {
        return BlendFactor::SRC_COLOR;
    } else if(str == "inv_src_color") {
        return BlendFactor::INV_SRC_COLOR;
    } else if(str == "src_alpha") {
        return BlendFactor::SRC_ALPHA;
    } else if(str == "inv_src_alpha") {
        return BlendFactor::INV_SRC_ALPHA;
    } else if(str == "dest_alpha") {
        return BlendFactor::DEST_ALPHA;
    } else if(str == "inv_dest_alpha") {
        return BlendFactor::INV_DEST_ALPHA;
    } else if(str == "dest_color") {
        return BlendFactor::DEST_COLOR;
    } else if(str == "inv_dest_color") {
        return BlendFactor::INV_DEST_COLOR;
    } else if(str == "src_alpha_sat") {
        return BlendFactor::SRC_ALPHA_SAT;
    } else if(str == "blend_factor") {
        return BlendFactor::BLEND_FACTOR;
    } else if(str == "inv_blend_factor") {
        return BlendFactor::INV_BLEND_FACTOR;
    } else if(str == "src1_color") {
        return BlendFactor::SRC1_COLOR;
    } else if(str == "inv_src1_color") {
        return BlendFactor::INV_SRC1_COLOR;
    } else if(str == "src1_alpha") {
        return BlendFactor::SRC1_ALPHA;
    } else if(str == "inv_src1_alpha") {
        return BlendFactor::INV_SRC1_ALPHA;
    } else {
        ERROR_AND_DIE("BlendFactor not defined.");
    }
}

BlendOperation BlendOperationFromString(std::string str) {

    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c, std::locale("")); });
    if(str == "add") {
        return BlendOperation::ADD;
    } else if(str == "subtract" || str == "sub") {
        return BlendOperation::SUBTRACT;
    } else if(str == "rev_sub" || str == "rev_subtract" || str == "reverse_sub" || str == "reverse_subtract") {
        return BlendOperation::REVERSE_SUBTRACT;
    } else if(str == "min" || str == "minimum") {
        return BlendOperation::MIN;
    } else if(str == "max" || str == "maximum") {
        return BlendOperation::MAX;
    } else {
        ERROR_AND_DIE("BlendOperation not defined.");
    }

}

BlendColorWriteEnable BlendColorWriteEnableFromString(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c, std::locale("")); });

    if(str.empty()) {
        return BlendColorWriteEnable::ALL;
    }

    BlendColorWriteEnable result = BlendColorWriteEnable::NONE;
    if(str.find('r') != std::string::npos) {
        result |= BlendColorWriteEnable::RED;
    }
    if(str.find('g') != std::string::npos) {
        result |= BlendColorWriteEnable::GREEN;
    }
    if(str.find('b') != std::string::npos) {
        result |= BlendColorWriteEnable::BLUE;
    }
    if(str.find('a') != std::string::npos) {
        result |= BlendColorWriteEnable::ALPHA;
    }
    return result;
}
