#include "Engine/RHI/DX11.hpp"

#include "Engine/Core/StringUtils.hpp"

unsigned int MIP_MASK_BITS = 0b0000'0001;
unsigned int MAG_MASK_BITS = 0b0000'0010;
unsigned int MIN_MASK_BITS = 0b0000'0100;
unsigned int COMPARISON_MASK_BITS = 0b0000'1000;
unsigned int MINIMUM_MASK_BITS = 0b0001'0000;
unsigned int MAXIMUM_MASK_BITS = 0b0010'0000;
unsigned int ANISOTROPIC_MASK_BITS = 0b0100'0000;

//Dragons be here!! Look at your own risk!
D3D11_FILTER FilterModeToD3DFilter(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison) {

    unsigned int filter_mask = GetFilterMaskFromModes(minFilterMode, magFilterMode, mipFilterMode, minMaxComparison);

    //Any anisotropic setting overrides all others.
    if ((filter_mask & ANISOTROPIC_MASK_BITS) == ANISOTROPIC_MASK_BITS) {
        if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
            return D3D11_FILTER_COMPARISON_ANISOTROPIC;
        }
        else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
            return D3D11_FILTER_MINIMUM_ANISOTROPIC;
        }
        else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
            return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
        }
        else { //No comparison mode set
            return D3D11_FILTER_ANISOTROPIC;
        }
    }
    else { //Not anisotropic
        bool isMip = (filter_mask & MIP_MASK_BITS) == MIP_MASK_BITS;
        bool isMag = (filter_mask & MAG_MASK_BITS) == MAG_MASK_BITS;
        bool isMin = (filter_mask & MIN_MASK_BITS) == MIN_MASK_BITS;
        if (!isMin && !isMag && !isMip) { //000
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
            }
            else {
                return D3D11_FILTER_MIN_MAG_MIP_POINT;
            }
        }
        else if (!isMin && !isMag && isMip) { //001
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
            }
            else {
                return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            }
        }
        else if (!isMin && isMag && !isMip) { //010
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
            }
            else {
                return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            }
        }
        else if (!isMin && isMag && isMip) {  //011
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
            }
            else {
                return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            }
        }
        else if (isMin && !isMag && !isMip) { //100
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
            }
            else {
                return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            }
        }
        else if (isMin && !isMag && isMip) {  //101
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            }
            else {
                return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            }
        }
        else if (isMin && isMag && !isMip) {  //110
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
            }
            else {
                return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            }
        }
        else if (isMin && isMag && isMip) {   //111
            if ((filter_mask & COMPARISON_MASK_BITS) == COMPARISON_MASK_BITS) {
                return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            }
            else if ((filter_mask & MINIMUM_MASK_BITS) == MINIMUM_MASK_BITS) {
                return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
            }
            else if ((filter_mask & MAXIMUM_MASK_BITS) == MAXIMUM_MASK_BITS) {
                return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
            }
            else {
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            }
        }
    }
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

//Dragons be here!! Look at your own risk!
unsigned int GetFilterMaskFromModes(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison) {
    unsigned int result = 0;

    switch (minMaxComparison) {
    case FilterComparisonMode::NONE:
        result &= ~(COMPARISON_MASK_BITS | MINIMUM_MASK_BITS | MAXIMUM_MASK_BITS);
        break;
    case FilterComparisonMode::MINIMUM:
        result |= MINIMUM_MASK_BITS;
        break;
    case FilterComparisonMode::MAXIMUM:
        result |= MAXIMUM_MASK_BITS;
        break;
    case FilterComparisonMode::COMPARISON:
        result |= COMPARISON_MASK_BITS;
        break;
    default:
        /* DO NOTHING */;
    }

    if (mipFilterMode == FilterMode::ANISOTROPIC) {
        result |= (ANISOTROPIC_MASK_BITS | MIP_MASK_BITS | MAG_MASK_BITS | MIN_MASK_BITS);
        return result; //Anisotropic overrides all other options.
    }
    else {
        if (mipFilterMode == FilterMode::POINT) {
            result &= ~MIP_MASK_BITS;
        }
        else if (mipFilterMode == FilterMode::LINEAR) {
            result |= MIP_MASK_BITS;
        }
    }

    if (magFilterMode == FilterMode::ANISOTROPIC) {
        result |= (ANISOTROPIC_MASK_BITS | MIP_MASK_BITS | MAG_MASK_BITS | MIN_MASK_BITS);
        return result; //Anisotropic overrides all other options.
    }
    else {
        if (magFilterMode == FilterMode::POINT) {
            result &= ~MAG_MASK_BITS;
        }
        else if (magFilterMode == FilterMode::LINEAR) {
            result |= MAG_MASK_BITS;
        }
    }

    if (minFilterMode == FilterMode::ANISOTROPIC) {
        result |= (ANISOTROPIC_MASK_BITS | MIP_MASK_BITS | MAG_MASK_BITS | MIN_MASK_BITS);
        return result; //Anisotropic overrides all other options.
    }
    else {
        if (minFilterMode == FilterMode::POINT) {
            result &= ~MIN_MASK_BITS;
        }
        else if (minFilterMode == FilterMode::LINEAR) {
            result |= MIN_MASK_BITS;
        }
    }

    return result;
}

FilterMode FilterModeFromString(const char* str) {
    return FilterModeFromString(std::string(str ? str : ""));
}

FilterMode FilterModeFromString(std::string str) {
    str = ToLowerCase(str);
    if(str == "point" || str == "pt") {
        return FilterMode::POINT;
    } else if(str == "linear" || str == "ln") {
        return FilterMode::LINEAR;
    } else if(str == "anisotropic" || str == "af") {
        return FilterMode::ANISOTROPIC;
    } else {
        return FilterMode::POINT;
    }
}

FilterComparisonMode FilterComparisonModeFromString(const char* str) {
    return FilterComparisonModeFromString(std::string(str ? str : ""));
}

FilterComparisonMode FilterComparisonModeFromString(std::string str) {
    str = ToLowerCase(str);
    if(str == "none") {
        return FilterComparisonMode::NONE;
    } else if(str == "min" || str == "minimum") {
        return FilterComparisonMode::MINIMUM;
    } else if(str == "max" || str == "maximum") {
        return FilterComparisonMode::MAXIMUM;
    } else if(str == "comp" || str == "comparison") {
        return FilterComparisonMode::COMPARISON;
    } else {
        return FilterComparisonMode::NONE;
    }
}

D3D11_TEXTURE_ADDRESS_MODE AddressModeToD3DAddressMode(const TextureAddressMode& address_mode) {
    switch (address_mode) {
    case TextureAddressMode::WRAP: return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureAddressMode::MIRROR: return D3D11_TEXTURE_ADDRESS_MIRROR;
    case TextureAddressMode::CLAMP: return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TextureAddressMode::BORDER: return D3D11_TEXTURE_ADDRESS_BORDER;
    case TextureAddressMode::MIRROR_ONCE: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    default: return D3D11_TEXTURE_ADDRESS_WRAP;
    }
}

TextureAddressMode TextureAddressModeFromString(const char* str) {
    return TextureAddressModeFromString(std::string(str ? str : ""));
}

TextureAddressMode TextureAddressModeFromString(std::string str) {
    str = ToLowerCase(str);
    if(str == "wrap") {
        return TextureAddressMode::WRAP;
    } else if(str == "mirror") {
        return TextureAddressMode::MIRROR;
    } else if(str == "clamp") {
        return TextureAddressMode::CLAMP;
    } else if(str == "border") {
        return TextureAddressMode::BORDER;
    } else if(str == "once" || str == "mirror_once") {
        return TextureAddressMode::MIRROR_ONCE;
    } else {
        return TextureAddressMode::WRAP;
    }

}

D3D11_COMPARISON_FUNC ComparisonFunctionToD3DComparisonFunction(const ComparisonFunction& compareFunc) {
    switch (compareFunc) {
    case ComparisonFunction::NEVER: return D3D11_COMPARISON_NEVER;
    case ComparisonFunction::LESS: return D3D11_COMPARISON_LESS;
    case ComparisonFunction::EQUAL: return D3D11_COMPARISON_EQUAL;
    case ComparisonFunction::LESS_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
    case ComparisonFunction::GREATER: return D3D11_COMPARISON_GREATER;
    case ComparisonFunction::NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
    case ComparisonFunction::GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
    case ComparisonFunction::ALWAYS: return D3D11_COMPARISON_ALWAYS;
    default: return D3D11_COMPARISON_NEVER;
    }
}

ComparisonFunction ComparisonFunctionFromString(const char* str) {
    return ComparisonFunctionFromString(std::string(str ? str : ""));
}

ComparisonFunction ComparisonFunctionFromString(std::string str) {
    str = ToLowerCase(str);
    if(str == "never") {
        return ComparisonFunction::NEVER;
    } else if(str == "less") {
        return ComparisonFunction::LESS;
    } else if(str == "equal") {
        return ComparisonFunction::EQUAL;
    } else if(str == "lessequal" || str == "le" || str == "leq") {
        return ComparisonFunction::LESS_EQUAL;
    } else if(str == "greater") {
        return ComparisonFunction::GREATER;
    } else if(str == "notequal" || str == "ne" || str == "neq") {
        return ComparisonFunction::NOT_EQUAL;
    } else if(str == "greaterequal" || str == "ge" || str == "geq") {
        return ComparisonFunction::GREATER_EQUAL;
    } else if(str == "always") {
        return ComparisonFunction::LESS;
    } else {
        return ComparisonFunction::NEVER;
    }
}

D3D11_STENCIL_OP StencilOperationToD3DStencilOperation(const StencilOperation& stencil_operation) {
    switch (stencil_operation) {
    case StencilOperation::KEEP: return D3D11_STENCIL_OP_KEEP;
    case StencilOperation::ZERO: return D3D11_STENCIL_OP_ZERO;
    case StencilOperation::REPLACE: return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation::INCREMENT_CLAMP: return D3D11_STENCIL_OP_INCR_SAT;
    case StencilOperation::DECREMENT_CLAMP: return D3D11_STENCIL_OP_DECR_SAT;
    case StencilOperation::INVERT: return D3D11_STENCIL_OP_INVERT;
    case StencilOperation::INCREMENT_WRAP: return D3D11_STENCIL_OP_INCR;
    case StencilOperation::DECREMENT_WRAP: return D3D11_STENCIL_OP_DECR;
    default: return D3D11_STENCIL_OP_KEEP;
    }
}

StencilOperation StencilOperationFromString(const char* str) {
    return StencilOperationFromString(std::string(str ? str : ""));
}

StencilOperation StencilOperationFromString(std::string str) {
    str = ToLowerCase(str);
    if(str == "keep") {
        return StencilOperation::KEEP;
    } else if(str == "zero") {
        return StencilOperation::ZERO;
    } else if(str == "replace") {
        return StencilOperation::REPLACE;
    } else if(str == "incrementandclamp" || str == "ic" || str == "incc" || str == "inc") {
        return StencilOperation::INCREMENT_CLAMP;
    } else if(str == "decrementandclamp" || str == "dc" || str == "decc" || str == "dec") {
        return StencilOperation::DECREMENT_CLAMP;
    } else if(str == "invert" || str == "inv") {
        return StencilOperation::INVERT;
    } else if(str == "incrementandwrap" || str == "iw" || str == "incw" || str == "inw") {
        return StencilOperation::INCREMENT_WRAP;
    } else if(str == "decrementandwrap" || str == "dw" || str == "decw" || str == "dew") {
        return StencilOperation::DECREMENT_WRAP;
    } else {
        return StencilOperation::KEEP;
    }
}

D3D11_USAGE BufferUsageToD3DUsage(const BufferUsage& usage) {
    switch (usage) {
    case BufferUsage::GPU: return D3D11_USAGE_DEFAULT;
    case BufferUsage::STATIC: return D3D11_USAGE_IMMUTABLE;
    case BufferUsage::DYNAMIC: return D3D11_USAGE_DYNAMIC;
    case BufferUsage::STAGING: return D3D11_USAGE_STAGING;
    default: return D3D11_USAGE_DEFAULT;
    }
}

D3D11_BIND_FLAG BufferBindUsageToD3DBindFlags(const BufferBindUsage& bindFlags) {
    return static_cast<D3D11_BIND_FLAG>(bindFlags);
}

D3D11_CPU_ACCESS_FLAG CPUAccessFlagFromUsage(const BufferUsage& usage) {
    switch (usage) {
    case BufferUsage::GPU: return D3D11_CPU_ACCESS_READ;
    case BufferUsage::DYNAMIC: return D3D11_CPU_ACCESS_WRITE;
    case BufferUsage::STATIC: return static_cast<D3D11_CPU_ACCESS_FLAG>(0U);
    case BufferUsage::STAGING: return static_cast<D3D11_CPU_ACCESS_FLAG>(D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE);
    default: return static_cast<D3D11_CPU_ACCESS_FLAG>(0U);
    }
}


D3D11_PRIMITIVE_TOPOLOGY PrimitiveTypeToD3dTopology(const PrimitiveType& topology) {
    switch (topology) {
    case PrimitiveType::NONE:                      return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    case PrimitiveType::POINTS:                    return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case PrimitiveType::LINES:                     return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveType::LINES_ADJ:                 return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
    case PrimitiveType::LINESSTRIP:                return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case PrimitiveType::LINESSTRIP_ADJ:            return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
    case PrimitiveType::TRIANGLES:                 return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveType::TRIANGLES_ADJ:             return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
    case PrimitiveType::TRIANGLESTRIP:             return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PrimitiveType::TRIANGLESTRIP_ADJ:         return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_1: return D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_2: return D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_3: return D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_4: return D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_5: return D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_6: return D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_7: return D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_8: return D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_9: return D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_10: return D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_11: return D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_12: return D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_13: return D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_14: return D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_15: return D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_16: return D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_17: return D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_18: return D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_19: return D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_20: return D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_21: return D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_22: return D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_23: return D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_24: return D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_25: return D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_26: return D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_27: return D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_28: return D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_29: return D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_30: return D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_31: return D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
    case PrimitiveType::CONTROL_POINT_PATCHLIST_32: return D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
    default: return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}
DXGI_FORMAT ImageFormatToDxgiFormat(const ImageFormat& format) {
    switch(format) {
        case ImageFormat::UNKNOWN: return DXGI_FORMAT_UNKNOWN;
        case ImageFormat::R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
        case ImageFormat::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ImageFormat::R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
        case ImageFormat::R32G32B32A32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;
        case ImageFormat::R32G32B32_TYPELESS: return DXGI_FORMAT_R32G32B32_TYPELESS;
        case ImageFormat::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
        case ImageFormat::R32G32B32_UINT: return DXGI_FORMAT_R32G32B32_UINT;
        case ImageFormat::R32G32B32_SINT: return DXGI_FORMAT_R32G32B32_SINT;
        case ImageFormat::R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_TYPELESS;
        case ImageFormat::R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case ImageFormat::R16G16B16A16_UNORM: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case ImageFormat::R16G16B16A16_UINT: return DXGI_FORMAT_R16G16B16A16_UINT;
        case ImageFormat::R16G16B16A16_SNORM: return DXGI_FORMAT_R16G16B16A16_SNORM;
        case ImageFormat::R16G16B16A16_SINT: return DXGI_FORMAT_R16G16B16A16_SINT;
        case ImageFormat::R32G32_TYPELESS: return DXGI_FORMAT_R32G32_TYPELESS;
        case ImageFormat::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
        case ImageFormat::R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
        case ImageFormat::R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;
        case ImageFormat::R32G8X24_TYPELESS: return DXGI_FORMAT_R32G8X24_TYPELESS;
        case ImageFormat::D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case ImageFormat::R32_FLOAT_X8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        case ImageFormat::X32_TYPELESS_G8X24_UINT: return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
        case ImageFormat::R10G10B10A2_TYPELESS: return DXGI_FORMAT_R10G10B10A2_TYPELESS;
        case ImageFormat::R10G10B10A2_UNORM: return DXGI_FORMAT_R10G10B10A2_UNORM;
        case ImageFormat::R10G10B10A2_UINT: return DXGI_FORMAT_R10G10B10A2_UINT;
        case ImageFormat::R11G11B10_FLOAT: return DXGI_FORMAT_R11G11B10_FLOAT;
        case ImageFormat::R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case ImageFormat::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case ImageFormat::R8G8B8A8_UINT: return DXGI_FORMAT_R8G8B8A8_UINT;
        case ImageFormat::R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
        case ImageFormat::R8G8B8A8_SINT: return DXGI_FORMAT_R8G8B8A8_SINT;
        case ImageFormat::R16G16_TYPELESS: return DXGI_FORMAT_R16G16_TYPELESS;
        case ImageFormat::R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
        case ImageFormat::R16G16_UNORM: return DXGI_FORMAT_R16G16_UNORM;
        case ImageFormat::R16G16_UINT: return DXGI_FORMAT_R16G16_UINT;
        case ImageFormat::R16G16_SNORM: return DXGI_FORMAT_R16G16_SNORM;
        case ImageFormat::R16G16_SINT: return DXGI_FORMAT_R16G16_SINT;
        case ImageFormat::R32_TYPELESS: return DXGI_FORMAT_R32_TYPELESS;
        case ImageFormat::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
        case ImageFormat::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case ImageFormat::R32_UINT: return DXGI_FORMAT_R32_UINT;
        case ImageFormat::R32_SINT: return DXGI_FORMAT_R32_SINT;
        case ImageFormat::R24G8_TYPELESS: return DXGI_FORMAT_R24G8_TYPELESS;
        case ImageFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case ImageFormat::R24_UNORM_X8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case ImageFormat::X24_TYPELESS_G8_UINT: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
        case ImageFormat::R8G8_TYPELESS: return DXGI_FORMAT_R8G8_TYPELESS;
        case ImageFormat::R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case ImageFormat::R8G8_UINT: return DXGI_FORMAT_R8G8_UINT;
        case ImageFormat::R8G8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
        case ImageFormat::R8G8_SINT: return DXGI_FORMAT_R8G8_SINT;
        case ImageFormat::R16_TYPELESS: return DXGI_FORMAT_R16_TYPELESS;
        case ImageFormat::R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
        case ImageFormat::D16_UNORM: return DXGI_FORMAT_D16_UNORM;
        case ImageFormat::R16_UNORM: return DXGI_FORMAT_R16_UNORM;
        case ImageFormat::R16_UINT: return DXGI_FORMAT_R16_UINT;
        case ImageFormat::R16_SNORM: return DXGI_FORMAT_R16_SNORM;
        case ImageFormat::R16_SINT: return DXGI_FORMAT_R16_SINT;
        case ImageFormat::R8_TYPELESS: return DXGI_FORMAT_R8_TYPELESS;
        case ImageFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case ImageFormat::R8_UINT: return DXGI_FORMAT_R8_UINT;
        case ImageFormat::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
        case ImageFormat::R8_SINT: return DXGI_FORMAT_R8_SINT;
        case ImageFormat::A8_UNORM: return DXGI_FORMAT_A8_UNORM;
        case ImageFormat::R1_UNORM: return DXGI_FORMAT_R1_UNORM;
        case ImageFormat::R9G9B9E5_SHAREDEXP: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        case ImageFormat::R8G8_B8G8_UNORM: return DXGI_FORMAT_R8G8_B8G8_UNORM;
        case ImageFormat::G8R8_G8B8_UNORM: return DXGI_FORMAT_G8R8_G8B8_UNORM;
        case ImageFormat::BC1_TYPELESS: return DXGI_FORMAT_BC1_TYPELESS;
        case ImageFormat::BC1_UNORM: return DXGI_FORMAT_BC1_UNORM;
        case ImageFormat::BC1_UNORM_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
        case ImageFormat::BC2_TYPELESS: return DXGI_FORMAT_BC2_TYPELESS;
        case ImageFormat::BC2_UNORM: return DXGI_FORMAT_BC2_UNORM;
        case ImageFormat::BC2_UNORM_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
        case ImageFormat::BC3_TYPELESS: return DXGI_FORMAT_BC3_TYPELESS;
        case ImageFormat::BC3_UNORM: return DXGI_FORMAT_BC3_UNORM;
        case ImageFormat::BC3_UNORM_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;
        case ImageFormat::BC4_TYPELESS: return DXGI_FORMAT_BC4_TYPELESS;
        case ImageFormat::BC4_UNORM: return DXGI_FORMAT_BC4_UNORM;
        case ImageFormat::BC4_SNORM: return DXGI_FORMAT_BC4_SNORM;
        case ImageFormat::BC5_TYPELESS: return DXGI_FORMAT_BC5_TYPELESS;
        case ImageFormat::BC5_UNORM: return DXGI_FORMAT_BC5_UNORM;
        case ImageFormat::BC5_SNORM: return DXGI_FORMAT_BC5_SNORM;
        case ImageFormat::B5G6R5_UNORM: return DXGI_FORMAT_B5G6R5_UNORM;
        case ImageFormat::B5G5R5A1_UNORM: return DXGI_FORMAT_B5G5R5A1_UNORM;
        case ImageFormat::B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case ImageFormat::B8G8R8X8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM;
        case ImageFormat::R10G10B10_XR_BIAS_A2_UNORM: return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
        case ImageFormat::B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_TYPELESS;
        case ImageFormat::B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case ImageFormat::B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_TYPELESS;
        case ImageFormat::B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
        case ImageFormat::BC6H_TYPELESS: return DXGI_FORMAT_BC6H_TYPELESS;
        case ImageFormat::BC6H_UF16: return DXGI_FORMAT_BC6H_UF16;
        case ImageFormat::BC6H_SF16: return DXGI_FORMAT_BC6H_SF16;
        case ImageFormat::BC7_TYPELESS: return DXGI_FORMAT_BC7_TYPELESS;
        case ImageFormat::BC7_UNORM: return DXGI_FORMAT_BC7_UNORM;
        case ImageFormat::BC7_UNORM_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;
        case ImageFormat::AYUV: return DXGI_FORMAT_AYUV;
        case ImageFormat::Y410: return DXGI_FORMAT_Y410;
        case ImageFormat::Y416: return DXGI_FORMAT_Y416;
        case ImageFormat::NV12: return DXGI_FORMAT_NV12;
        case ImageFormat::P010: return DXGI_FORMAT_P010;
        case ImageFormat::P016: return DXGI_FORMAT_P016;
        case ImageFormat::OPAQUE_420: return DXGI_FORMAT_420_OPAQUE;
        case ImageFormat::YUY2: return DXGI_FORMAT_YUY2;
        case ImageFormat::Y210: return DXGI_FORMAT_Y210;
        case ImageFormat::Y216: return DXGI_FORMAT_Y216;
        case ImageFormat::NV11: return DXGI_FORMAT_NV11;
        case ImageFormat::AI44: return DXGI_FORMAT_AI44;
        case ImageFormat::IA44: return DXGI_FORMAT_IA44;
        case ImageFormat::P8: return DXGI_FORMAT_P8;
        case ImageFormat::A8P8: return DXGI_FORMAT_A8P8;
        case ImageFormat::B4G4R4A4_UNORM: return DXGI_FORMAT_B4G4R4A4_UNORM;
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

ImageFormat DxgiFormatToImageFormat(DXGI_FORMAT format) {
    switch(format) {
        case DXGI_FORMAT_UNKNOWN: return ImageFormat::UNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_TYPELESS: return ImageFormat::R32G32B32A32_TYPELESS;
        case DXGI_FORMAT_R32G32B32A32_FLOAT: return ImageFormat::R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R32G32B32A32_UINT: return ImageFormat::R32G32B32A32_UINT;
        case DXGI_FORMAT_R32G32B32A32_SINT: return ImageFormat::R32G32B32A32_SINT;
        case DXGI_FORMAT_R32G32B32_TYPELESS: return ImageFormat::R32G32B32_TYPELESS;
        case DXGI_FORMAT_R32G32B32_FLOAT: return ImageFormat::R32G32B32_FLOAT;
        case DXGI_FORMAT_R32G32B32_UINT: return ImageFormat::R32G32B32_UINT;
        case DXGI_FORMAT_R32G32B32_SINT: return ImageFormat::R32G32B32_SINT;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return ImageFormat::R16G16B16A16_TYPELESS;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return ImageFormat::R16G16B16A16_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_UNORM: return ImageFormat::R16G16B16A16_UNORM;
        case DXGI_FORMAT_R16G16B16A16_UINT: return ImageFormat::R16G16B16A16_UINT;
        case DXGI_FORMAT_R16G16B16A16_SNORM: return ImageFormat::R16G16B16A16_SNORM;
        case DXGI_FORMAT_R16G16B16A16_SINT: return ImageFormat::R16G16B16A16_SINT;
        case DXGI_FORMAT_R32G32_TYPELESS: return ImageFormat::R32G32_TYPELESS;
        case DXGI_FORMAT_R32G32_FLOAT: return ImageFormat::R32G32_FLOAT;
        case DXGI_FORMAT_R32G32_UINT: return ImageFormat::R32G32_UINT;
        case DXGI_FORMAT_R32G32_SINT: return ImageFormat::R32G32_SINT;
        case DXGI_FORMAT_R32G8X24_TYPELESS: return ImageFormat::R32G8X24_TYPELESS;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return ImageFormat::D32_FLOAT_S8X24_UINT;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return ImageFormat::R32_FLOAT_X8X24_TYPELESS;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return ImageFormat::X32_TYPELESS_G8X24_UINT;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS: return ImageFormat::R10G10B10A2_TYPELESS;
        case DXGI_FORMAT_R10G10B10A2_UNORM: return ImageFormat::R10G10B10A2_UNORM;
        case DXGI_FORMAT_R10G10B10A2_UINT: return ImageFormat::R10G10B10A2_UINT;
        case DXGI_FORMAT_R11G11B10_FLOAT: return ImageFormat::R11G11B10_FLOAT;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return ImageFormat::R8G8B8A8_TYPELESS;
        case DXGI_FORMAT_R8G8B8A8_UNORM: return ImageFormat::R8G8B8A8_UNORM;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return ImageFormat::R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_R8G8B8A8_UINT: return ImageFormat::R8G8B8A8_UINT;
        case DXGI_FORMAT_R8G8B8A8_SNORM: return ImageFormat::R8G8B8A8_SNORM;
        case DXGI_FORMAT_R8G8B8A8_SINT: return ImageFormat::R8G8B8A8_SINT;
        case DXGI_FORMAT_R16G16_TYPELESS: return ImageFormat::R16G16_TYPELESS;
        case DXGI_FORMAT_R16G16_FLOAT: return ImageFormat::R16G16_FLOAT;
        case DXGI_FORMAT_R16G16_UNORM: return ImageFormat::R16G16_UNORM;
        case DXGI_FORMAT_R16G16_UINT: return ImageFormat::R16G16_UINT;
        case DXGI_FORMAT_R16G16_SNORM: return ImageFormat::R16G16_SNORM;
        case DXGI_FORMAT_R16G16_SINT: return ImageFormat::R16G16_SINT;
        case DXGI_FORMAT_R32_TYPELESS: return ImageFormat::R32_TYPELESS;
        case DXGI_FORMAT_D32_FLOAT: return ImageFormat::D32_FLOAT;
        case DXGI_FORMAT_R32_FLOAT: return ImageFormat::R32_FLOAT;
        case DXGI_FORMAT_R32_UINT: return ImageFormat::R32_UINT;
        case DXGI_FORMAT_R32_SINT: return ImageFormat::R32_SINT;
        case DXGI_FORMAT_R24G8_TYPELESS: return ImageFormat::R24G8_TYPELESS;
        case DXGI_FORMAT_D24_UNORM_S8_UINT: return ImageFormat::D24_UNORM_S8_UINT;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return ImageFormat::R24_UNORM_X8_TYPELESS;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return ImageFormat::X24_TYPELESS_G8_UINT;
        case DXGI_FORMAT_R8G8_TYPELESS: return ImageFormat::R8G8_TYPELESS;
        case DXGI_FORMAT_R8G8_UNORM: return ImageFormat::R8G8_UNORM;
        case DXGI_FORMAT_R8G8_UINT: return ImageFormat::R8G8_UINT;
        case DXGI_FORMAT_R8G8_SNORM: return ImageFormat::R8G8_SNORM;
        case DXGI_FORMAT_R8G8_SINT: return ImageFormat::R8G8_SINT;
        case DXGI_FORMAT_R16_TYPELESS: return ImageFormat::R16_TYPELESS;
        case DXGI_FORMAT_R16_FLOAT: return ImageFormat::R16_FLOAT;
        case DXGI_FORMAT_D16_UNORM: return ImageFormat::D16_UNORM;
        case DXGI_FORMAT_R16_UNORM: return ImageFormat::R16_UNORM;
        case DXGI_FORMAT_R16_UINT: return ImageFormat::R16_UINT;
        case DXGI_FORMAT_R16_SNORM: return ImageFormat::R16_SNORM;
        case DXGI_FORMAT_R16_SINT: return ImageFormat::R16_SINT;
        case DXGI_FORMAT_R8_TYPELESS: return ImageFormat::R8_TYPELESS;
        case DXGI_FORMAT_R8_UNORM: return ImageFormat::R8_UNORM;
        case DXGI_FORMAT_R8_UINT: return ImageFormat::R8_UINT;
        case DXGI_FORMAT_R8_SNORM: return ImageFormat::R8_SNORM;
        case DXGI_FORMAT_R8_SINT: return ImageFormat::R8_SINT;
        case DXGI_FORMAT_A8_UNORM: return ImageFormat::A8_UNORM;
        case DXGI_FORMAT_R1_UNORM: return ImageFormat::R1_UNORM;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return ImageFormat::R9G9B9E5_SHAREDEXP;
        case DXGI_FORMAT_R8G8_B8G8_UNORM: return ImageFormat::R8G8_B8G8_UNORM;
        case DXGI_FORMAT_G8R8_G8B8_UNORM: return ImageFormat::G8R8_G8B8_UNORM;
        case DXGI_FORMAT_BC1_TYPELESS: return ImageFormat::BC1_TYPELESS;
        case DXGI_FORMAT_BC1_UNORM: return ImageFormat::BC1_UNORM;
        case DXGI_FORMAT_BC1_UNORM_SRGB: return ImageFormat::BC1_UNORM_SRGB;
        case DXGI_FORMAT_BC2_TYPELESS: return ImageFormat::BC2_TYPELESS;
        case DXGI_FORMAT_BC2_UNORM: return ImageFormat::BC2_UNORM;
        case DXGI_FORMAT_BC2_UNORM_SRGB: return ImageFormat::BC2_UNORM_SRGB;
        case DXGI_FORMAT_BC3_TYPELESS: return ImageFormat::BC3_TYPELESS;
        case DXGI_FORMAT_BC3_UNORM: return ImageFormat::BC3_UNORM;
        case DXGI_FORMAT_BC3_UNORM_SRGB: return ImageFormat::BC3_UNORM_SRGB;
        case DXGI_FORMAT_BC4_TYPELESS: return ImageFormat::BC4_TYPELESS;
        case DXGI_FORMAT_BC4_UNORM: return ImageFormat::BC4_UNORM;
        case DXGI_FORMAT_BC4_SNORM: return ImageFormat::BC4_SNORM;
        case DXGI_FORMAT_BC5_TYPELESS: return ImageFormat::BC5_TYPELESS;
        case DXGI_FORMAT_BC5_UNORM: return ImageFormat::BC5_UNORM;
        case DXGI_FORMAT_BC5_SNORM: return ImageFormat::BC5_SNORM;
        case DXGI_FORMAT_B5G6R5_UNORM: return ImageFormat::B5G6R5_UNORM;
        case DXGI_FORMAT_B5G5R5A1_UNORM: return ImageFormat::B5G5R5A1_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return ImageFormat::B8G8R8A8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_UNORM: return ImageFormat::B8G8R8X8_UNORM;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return ImageFormat::R10G10B10_XR_BIAS_A2_UNORM;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return ImageFormat::B8G8R8A8_TYPELESS;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return ImageFormat::B8G8R8A8_UNORM_SRGB;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS: return ImageFormat::B8G8R8X8_TYPELESS;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return ImageFormat::B8G8R8X8_UNORM_SRGB;
        case DXGI_FORMAT_BC6H_TYPELESS: return ImageFormat::BC6H_TYPELESS;
        case DXGI_FORMAT_BC6H_UF16: return ImageFormat::BC6H_UF16;
        case DXGI_FORMAT_BC6H_SF16: return ImageFormat::BC6H_SF16;
        case DXGI_FORMAT_BC7_TYPELESS: return ImageFormat::BC7_TYPELESS;
        case DXGI_FORMAT_BC7_UNORM: return ImageFormat::BC7_UNORM;
        case DXGI_FORMAT_BC7_UNORM_SRGB: return ImageFormat::BC7_UNORM_SRGB;
        case DXGI_FORMAT_AYUV: return ImageFormat::AYUV;
        case DXGI_FORMAT_Y410: return ImageFormat::Y410;
        case DXGI_FORMAT_Y416: return ImageFormat::Y416;
        case DXGI_FORMAT_NV12: return ImageFormat::NV12;
        case DXGI_FORMAT_P010: return ImageFormat::P010;
        case DXGI_FORMAT_P016: return ImageFormat::P016;
        case DXGI_FORMAT_420_OPAQUE: return ImageFormat::OPAQUE_420;
        case DXGI_FORMAT_YUY2: return ImageFormat::YUY2;
        case DXGI_FORMAT_Y210: return ImageFormat::Y210;
        case DXGI_FORMAT_Y216: return ImageFormat::Y216;
        case DXGI_FORMAT_NV11: return ImageFormat::NV11;
        case DXGI_FORMAT_AI44: return ImageFormat::AI44;
        case DXGI_FORMAT_IA44: return ImageFormat::IA44;
        case DXGI_FORMAT_P8: return ImageFormat::P8;
        case DXGI_FORMAT_A8P8: return ImageFormat::A8P8;
        case DXGI_FORMAT_B4G4R4A4_UNORM: return ImageFormat::B4G4R4A4_UNORM;
        default: return ImageFormat::UNKNOWN;
    }
}
