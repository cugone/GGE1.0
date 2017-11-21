#pragma once

#include <string>

#include "Engine/RHI/RHITypes.hpp"

#ifdef _DEBUG
#define INITGUID
#endif
#include <d3d11.h>
//#include <d3d10.h>
#include <DXGI.h>

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

// LIBS
#pragma comment( lib, "d3d11.lib" )
//#pragma comment( lib, "d3d10.lib" )
#pragma comment( lib, "DXGI.lib" )

#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" ) // needed for shader compilation/reflection

#ifdef DX_SAFE_RELEASE
#undef DX_SAFE_RELEASE
#endif

#ifndef DX_SAFE_RELEASE
#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }
#endif


D3D11_FILTER FilterModeToD3DFilter(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison);
FilterMode FilterModeFromString(const char* str);
FilterMode FilterModeFromString(std::string str);

FilterComparisonMode FilterComparisonModeFromString(const char* str);
FilterComparisonMode FilterComparisonModeFromString(std::string str);

D3D11_TEXTURE_ADDRESS_MODE AddressModeToD3DAddressMode(const TextureAddressMode& address_mode);
TextureAddressMode TextureAddressModeFromString(const char* str);
TextureAddressMode TextureAddressModeFromString(std::string str);

D3D11_COMPARISON_FUNC ComparisonFunctionToD3DComparisonFunction(const ComparisonFunction& compareFunc);
ComparisonFunction ComparisonFunctionFromString(std::string str);
ComparisonFunction ComparisonFunctionFromString(const char* str);

D3D11_STENCIL_OP StencilOperationToD3DStencilOperation(const StencilOperation& stencil_operation);
StencilOperation StencilOperationFromString(const char* str);
StencilOperation StencilOperationFromString(std::string str);

unsigned int GetFilterMaskFromModes(const FilterMode& minFilterMode, const FilterMode& magFilterMode, const FilterMode& mipFilterMode, const FilterComparisonMode& minMaxComparison);

D3D11_USAGE BufferUsageToD3DUsage(const BufferUsage& usage);
D3D11_BIND_FLAG BufferBindUsageToD3DBindFlags(const BufferBindUsage& bindFlags);
D3D11_CPU_ACCESS_FLAG CPUAccessFlagFromUsage(const BufferUsage& usage);

D3D11_PRIMITIVE_TOPOLOGY PrimitiveTypeToD3dTopology(const PrimitiveType& topology);

DXGI_FORMAT ImageFormatToDxgiFormat(const ImageFormat& format);
ImageFormat DxgiFormatToImageFormat(DXGI_FORMAT format);
