#pragma once

#include "Engine/Math/IntVector3.hpp"

class RHIDevice;
struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11UnorderedAccessView;

class TextureBase {
public:
    TextureBase() noexcept;
    TextureBase(const TextureBase& other) noexcept;
    TextureBase(TextureBase&& r_other) noexcept;
    TextureBase& operator=(const TextureBase& rhs) noexcept;
    TextureBase& operator=(TextureBase&& r_rhs) noexcept;
	virtual ~TextureBase()=0;

    const IntVector3& GetDimensions() const noexcept;
    IntVector3& GetDimensions() noexcept;

    virtual bool IsValid() const noexcept =0;
    bool IsRenderTarget() const noexcept;
    bool IsLoaded() const noexcept;
    void IsLoaded(bool is_loaded) noexcept;

    ID3D11RenderTargetView* GetRenderTargetView() noexcept;
    ID3D11ShaderResourceView* GetShaderResourceView() noexcept;
    ID3D11DepthStencilView* GetDepthStencilView() noexcept;
    ID3D11UnorderedAccessView* GetUnorderedAccessView() noexcept;

protected:

    RHIDevice* _device = nullptr;
    ID3D11RenderTargetView* _dx_rtv = nullptr;
    ID3D11ShaderResourceView* _dx_srv = nullptr;
    ID3D11DepthStencilView* _dx_dsv = nullptr;
    ID3D11UnorderedAccessView* _dx_uav = nullptr;
    IntVector3 _dimensions = IntVector3{ 1,0,0 };
    bool _is_loaded = false;

private:

};