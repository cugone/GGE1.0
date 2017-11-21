#include "Engine/Renderer/Sampler.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHI.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/core/ErrorWarningAssert.hpp"

Sampler::Sampler(RHIDevice* device
                 ,const FilterMode& min_filter /*= FilterMode::POINT*/
                 ,const FilterMode& mag_filter /*= FilterMode::POINT*/
                 ,const FilterMode& mip_filter /*= FilterMode::POINT*/
                 ,const FilterComparisonMode& compare_mode /*= FilterComparisonMode::NONE*/
                 ,const TextureAddressMode& UaddressMode /*= TextureAddressMode::WRAP*/
                 ,const TextureAddressMode& VaddressMode /*= TextureAddressMode::WRAP*/
                 ,const TextureAddressMode& WaddressMode /*= TextureAddressMode::WRAP*/
                 ,const Rgba& borderColor /*= Rgba::WHITE*/
                 ,const ComparisonFunction& compareFunc /*= ComparisonFunction::NEVER*/
                 ,unsigned int maxAnisotropicLevel /*= 1*/
                 ,float mipmapLODBias /*= 0.0f*/
                 ,float minLOD /*= 0.0f*/
                 ,float maxLOD /*= 0.0f*/)
    : _dx_sampler(nullptr)
{

    bool succeeded = CreateSamplerState(device,
                                        min_filter, mag_filter, mip_filter, compare_mode,
                                        UaddressMode, VaddressMode, WaddressMode, borderColor, compareFunc,
                                        maxAnisotropicLevel, mipmapLODBias, minLOD, maxLOD);
    if(!succeeded) {
        _dx_sampler->Release();
        _dx_sampler = nullptr;
        ERROR_AND_DIE("Sampler: dx Sample failed to create.\n");
    }
}

Sampler::Sampler(RHIDevice* device, const XMLElement& element)
: _dx_sampler(nullptr)
{
    if(!LoadFromXml(device, element)) {
        _dx_sampler->Release();
        _dx_sampler = nullptr;
        ERROR_AND_DIE("Sampler: Load from Xml failed.");
    }
}

Sampler::~Sampler() {
    _dx_sampler->Release();
    _dx_sampler = nullptr;
}

ID3D11SamplerState* Sampler::GetDxSampler() {
    return _dx_sampler;
}

bool Sampler::LoadFromXml(RHIDevice* device, const XMLElement& element) {

    SamplerDesc desc;
    auto xml_sampler = element.FirstChildElement("sampler");
    if(xml_sampler != nullptr) {

        DataUtils::ValidateXmlElement(*xml_sampler, "", "", "filter,textureAddress,lod", "borderColor,test,maxAF");

        desc.borderColor = DataUtils::ParseXmlAttribute(*xml_sampler, "borderColor", desc.borderColor);
        
        std::string compare_str = "never";
        compare_str = DataUtils::ParseXmlAttribute(*xml_sampler, "test", compare_str);
        desc.compareFunc = ComparisonFunctionFromString(compare_str);

        desc.maxAnisotropicLevel = DataUtils::ParseXmlAttribute(*xml_sampler, "maxAF", desc.maxAnisotropicLevel);

        auto xml_filter = xml_sampler->FirstChildElement("filter");
        if(xml_filter != nullptr) {
            
            DataUtils::ValidateXmlElement(*xml_filter, "", "min,mag,mip,mode");

            std::string filter_str = "point";
            filter_str = DataUtils::ParseXmlAttribute(*xml_filter, "min", filter_str);
            desc.min_filter = FilterModeFromString(filter_str);

            filter_str = "point";
            filter_str = DataUtils::ParseXmlAttribute(*xml_filter, "mag", filter_str);
            desc.mag_filter = FilterModeFromString(filter_str);

            filter_str = "point";
            filter_str = DataUtils::ParseXmlAttribute(*xml_filter, "mip", filter_str);
            desc.mip_filter = FilterModeFromString(filter_str);

            compare_str = "none";
            compare_str = DataUtils::ParseXmlAttribute(*xml_filter, "mode", compare_str);
            desc.compare_mode = FilterComparisonModeFromString(compare_str);
        }

        auto xml_textureAddress = xml_sampler->FirstChildElement("textureAddress");
        if(xml_textureAddress != nullptr) {
            
            DataUtils::ValidateXmlElement(*xml_textureAddress, "", "", "", "u,v,w");

            std::string str = "wrap";
            str = DataUtils::ParseXmlAttribute(*xml_textureAddress, "u", str);
            desc.UaddressMode = TextureAddressModeFromString(str);

            str = "wrap";
            str = DataUtils::ParseXmlAttribute(*xml_textureAddress, "v", str);
            desc.VaddressMode = TextureAddressModeFromString(str);

            str = "wrap";
            str = DataUtils::ParseXmlAttribute(*xml_textureAddress, "w", str);
            desc.WaddressMode = TextureAddressModeFromString(str);

        }

        auto xml_lod = xml_sampler->FirstChildElement("lod");
        if(xml_lod != nullptr) {
            DataUtils::ValidateXmlElement(*xml_lod, "", "", "", "min,max,mipmapbias");
            desc.minLOD = DataUtils::ParseXmlAttribute(*xml_lod, "min", desc.minLOD);
            desc.maxLOD = DataUtils::ParseXmlAttribute(*xml_lod, "max", desc.maxLOD);
            desc.mipmapLODBias = DataUtils::ParseXmlAttribute(*xml_lod, "mipmapbias", desc.mipmapLODBias);
        }
    }

    return CreateSamplerState(device, desc);

}

bool Sampler::CreateSamplerState(RHIDevice* device, const FilterMode& min_filter /*= FilterMode::POINT */, const FilterMode& mag_filter /*= FilterMode::POINT */, const FilterMode& mip_filter /*= FilterMode::POINT */, const FilterComparisonMode& compare_mode /*= FilterComparisonMode::NONE */, const TextureAddressMode& UaddressMode /*= TextureAddressMode::WRAP */, const TextureAddressMode& VaddressMode /*= TextureAddressMode::WRAP */, const TextureAddressMode& WaddressMode /*= TextureAddressMode::WRAP */, const Rgba& borderColor /*= Rgba::WHITE */, const ComparisonFunction& compareFunc /*= ComparisonFunction::NEVER */, unsigned int maxAnisotropicLevel /*= 1 */, float mipmapLODBias /*= 0.0f*/, float minLOD /*= 0.0f*/, float maxLOD /*= 0.0f*/) {

    D3D11_SAMPLER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.Filter = FilterModeToD3DFilter(min_filter, mag_filter, mip_filter, compare_mode);

    desc.AddressU = AddressModeToD3DAddressMode(UaddressMode);
    desc.AddressV = AddressModeToD3DAddressMode(VaddressMode);
    desc.AddressW = AddressModeToD3DAddressMode(WaddressMode);

    desc.MinLOD = minLOD;
    desc.MaxLOD = maxLOD;

    desc.MipLODBias = mipmapLODBias;

    desc.MaxAnisotropy = maxAnisotropicLevel;

    desc.ComparisonFunc = ComparisonFunctionToD3DComparisonFunction(compareFunc);

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
    borderColor.GetAsFloats(r, g, b, a);
    desc.BorderColor[0] = r;
    desc.BorderColor[1] = g;
    desc.BorderColor[2] = b;
    desc.BorderColor[3] = a;


    HRESULT hr = device->GetDxDevice()->CreateSamplerState(&desc, &_dx_sampler);
    return SUCCEEDED(hr);
}

bool Sampler::CreateSamplerState(RHIDevice* device, const SamplerDesc& desc /*= SamplerDesc()*/) {
    return CreateSamplerState(device
                            ,desc.min_filter
                            ,desc.mag_filter
                            ,desc.mip_filter
                            ,desc.compare_mode
                            ,desc.UaddressMode
                            ,desc.VaddressMode
                            ,desc.WaddressMode
                            ,desc.borderColor
                            ,desc.compareFunc
                            ,desc.maxAnisotropicLevel
                            ,desc.mipmapLODBias
                            ,desc.minLOD
                            ,desc.maxLOD);
}