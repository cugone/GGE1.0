#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/RHI/RHITypes.hpp"

class RHIDevice;
struct ID3D11SamplerState;

struct SamplerDesc {
    FilterMode min_filter;
    FilterMode mag_filter;
    FilterMode mip_filter;
    FilterComparisonMode compare_mode;
    TextureAddressMode UaddressMode;
    TextureAddressMode VaddressMode;
    TextureAddressMode WaddressMode;
    Rgba borderColor;
    ComparisonFunction compareFunc;
    unsigned int maxAnisotropicLevel;
    float mipmapLODBias;
    float minLOD;
    float maxLOD;
    SamplerDesc()
    :min_filter(FilterMode::POINT)
    ,mag_filter(FilterMode::POINT)
    ,mip_filter(FilterMode::POINT)
    ,compare_mode(FilterComparisonMode::NONE)
    ,UaddressMode(TextureAddressMode::WRAP)
    ,VaddressMode(TextureAddressMode::WRAP)
    ,WaddressMode(TextureAddressMode::WRAP)
    ,borderColor(Rgba::WHITE)
    ,compareFunc(ComparisonFunction::NEVER)
    ,maxAnisotropicLevel(1)
    ,mipmapLODBias(0.0f)
    ,minLOD(0.0f)
    ,maxLOD(0.0f)
    {/* DO NOTHING */}

};

class Sampler {
public:
	Sampler(RHIDevice* device
            ,const FilterMode& min_filter = FilterMode::POINT
            ,const FilterMode& mag_filter = FilterMode::POINT
            ,const FilterMode& mip_filter = FilterMode::POINT
            ,const FilterComparisonMode& compare_mode = FilterComparisonMode::NONE
            ,const TextureAddressMode& UaddressMode = TextureAddressMode::WRAP
            ,const TextureAddressMode& VaddressMode = TextureAddressMode::WRAP
            ,const TextureAddressMode& WaddressMode = TextureAddressMode::WRAP
            ,const Rgba& borderColor = Rgba::WHITE
            ,const ComparisonFunction& compareFunc = ComparisonFunction::NEVER
            ,unsigned int maxAnisotropicLevel = 1
            ,float mipmapLODBias = 0.0f
            ,float minLOD = 0.0f
            ,float maxLOD = 0.0f
           );

    Sampler(RHIDevice* device, const XMLElement& element);
	~Sampler();

    ID3D11SamplerState* GetDxSampler();
protected:
    bool LoadFromXml(RHIDevice* device, const XMLElement& element);
    bool CreateSamplerState(RHIDevice* device, const SamplerDesc& desc = SamplerDesc());
    bool CreateSamplerState(RHIDevice* device
                            , const FilterMode& min_filter = FilterMode::POINT
                            , const FilterMode& mag_filter = FilterMode::POINT
                            , const FilterMode& mip_filter = FilterMode::POINT
                            , const FilterComparisonMode& compare_mode = FilterComparisonMode::NONE
                            , const TextureAddressMode& UaddressMode = TextureAddressMode::WRAP
                            , const TextureAddressMode& VaddressMode = TextureAddressMode::WRAP
                            , const TextureAddressMode& WaddressMode = TextureAddressMode::WRAP
                            , const Rgba& borderColor = Rgba::WHITE
                            , const ComparisonFunction& compareFunc = ComparisonFunction::NEVER
                            , unsigned int maxAnisotropicLevel = 1
                            , float mipmapLODBias = 0.0f
                            , float minLOD = 0.0f
                            , float maxLOD = 0.0f);
private:
	ID3D11SamplerState* _dx_sampler;
};