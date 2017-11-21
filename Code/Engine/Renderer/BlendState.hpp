#pragma once

#include <vector>

#include "Engine/Core/DataUtils.hpp"

class RHIDevice;
struct ID3D11BlendState;

enum class BlendFactor {
    ZERO,
    ONE,
    SRC_COLOR,
    INV_SRC_COLOR,
    SRC_ALPHA,
    INV_SRC_ALPHA,
    DEST_ALPHA,
    INV_DEST_ALPHA,
    DEST_COLOR,
    INV_DEST_COLOR,
    SRC_ALPHA_SAT,
    BLEND_FACTOR,
    INV_BLEND_FACTOR,
    SRC1_COLOR,
    INV_SRC1_COLOR,
    SRC1_ALPHA,
    INV_SRC1_ALPHA,
};

enum class BlendOperation {
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX,
};

enum class BlendColorWriteEnable : unsigned char {
    NONE  = 0x00
    ,RED   = 0x01
    ,GREEN = 0x02
    ,BLUE  = 0x04
    ,ALPHA = 0x08
    ,ALL   = 0x0F
};

struct BlendDesc {
    bool enable;
    BlendFactor source_factor;
    BlendFactor dest_factor;
    BlendOperation blend_op;
    BlendFactor source_factor_alpha;
    BlendFactor dest_factor_alpha;
    BlendOperation blend_op_alpha;
    BlendColorWriteEnable blend_color_write_enable;

    BlendDesc()
    :enable(false)
    , source_factor(BlendFactor::ONE)
    , dest_factor(BlendFactor::ZERO)
    , blend_op(BlendOperation::ADD)
    , source_factor_alpha(BlendFactor::ONE)
    , dest_factor_alpha(BlendFactor::ZERO)
    , blend_op_alpha(BlendOperation::ADD)
    , blend_color_write_enable(BlendColorWriteEnable::ALL)
    { /* DO NOTHING */ }

};

BlendColorWriteEnable operator~(const BlendColorWriteEnable& a);
BlendColorWriteEnable operator|(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable operator&(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable operator^(const BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable& operator&=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable& operator|=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b);
BlendColorWriteEnable& operator^=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b);

class BlendState {
public:

    explicit BlendState(RHIDevice* device, const XMLElement& element);

    explicit BlendState(RHIDevice* device,
                        bool enable = false,
                        bool alphaToCoverageEnable = false,
                        const BlendFactor& source_factor = BlendFactor::ONE,
                        const BlendFactor& dest_factor = BlendFactor::ZERO,
                        const BlendOperation& blend_op = BlendOperation::ADD,
                        const BlendFactor& source_factor_alpha = BlendFactor::ONE,
                        const BlendFactor& dest_factor_alpha = BlendFactor::ZERO,
                        const BlendOperation& blend_op_alpha = BlendOperation::ADD,
                        const BlendColorWriteEnable& blend_color_write_enable = BlendColorWriteEnable::ALL);

    explicit BlendState(RHIDevice* device, bool alphaToCoverageEnable = false, BlendDesc desc = BlendDesc());
	~BlendState();

    ID3D11BlendState* GetDxBlendState();
protected:
    bool LoadFromXML(RHIDevice* device, const XMLElement& element);
    bool CreateBlendState(RHIDevice* device
                          , bool enable = false
                          , bool alphaToCoverageEnable = false
                          , const BlendFactor& source_factor = BlendFactor::ONE
                          , const BlendFactor& dest_factor = BlendFactor::ZERO
                          , const BlendOperation& blend_op = BlendOperation::ADD
                          , const BlendFactor& source_factor_alpha = BlendFactor::ONE
                          , const BlendFactor& dest_factor_alpha = BlendFactor::ONE
                          , const BlendOperation& blend_op_alpha = BlendOperation::ADD
                          , const BlendColorWriteEnable& blend_color_write_enable = BlendColorWriteEnable::ALL);
    bool CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable = false, BlendDesc render_target = BlendDesc());
    bool CreateBlendState(RHIDevice* device, bool alphaToCoverageEnable = false, const std::vector<BlendDesc>& render_targets = { BlendDesc() });
private:
    ID3D11BlendState* _dx_blendstate;
};