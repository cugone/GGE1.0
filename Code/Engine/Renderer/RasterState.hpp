#pragma once

#include "Engine/Core/DataUtils.hpp"

class RHIDevice;
struct ID3D11RasterizerState;

enum class FillMode {
    SOLID,
    WIREFRAME,
};

enum class CullMode {
    NONE,
    FRONT,
    BACK,
    BOTH,
};

struct RasterDesc {
    FillMode fillmode;
    CullMode cullmode;
    float depthBiasClamp;
    float slopeScaledDepthBias;
    int depthBias;
    bool depthClipEnable;
    bool scissorEnable;
    bool multisampleEnable;
    bool antialiasedLineEnable;
    RasterDesc()
    : fillmode(FillMode::SOLID)
    , cullmode(CullMode::BACK)
    , depthBiasClamp(0.0f)
    , slopeScaledDepthBias(0.0f)
    , depthBias(0)
    , depthClipEnable(true)
    , scissorEnable(false)
    , multisampleEnable(false)
    , antialiasedLineEnable(false)
    {/* DO NOTHING */}
};

class RasterState {
public:
    RasterState(RHIDevice* device, const FillMode& fillmode = FillMode::SOLID, const CullMode& cullmode = CullMode::BACK, bool antiAliasing = false);
    RasterState(RHIDevice* device, const XMLElement& element);
	~RasterState();

    ID3D11RasterizerState* GetDxRasterizer();
protected:
    bool LoadFromXML(RHIDevice* device, const XMLElement& element);
    bool CreateRasterState(RHIDevice* device, const FillMode& fillmode = FillMode::SOLID, const CullMode& cullmode = CullMode::BACK, bool antiAliasing = false);
    bool CreateRasterState(RHIDevice* device, const RasterDesc& raster_desc = RasterDesc());
private:
    ID3D11RasterizerState* _dx_rasterizer;
};