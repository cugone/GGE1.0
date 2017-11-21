#include "Engine/Renderer/TextureBase.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"

TextureBase::TextureBase() noexcept {
    /* DO NOTHING */
}

TextureBase::TextureBase(const TextureBase& other) noexcept
: _device(other._device)
, _dimensions(other._dimensions)
, _dx_dsv(other._dx_dsv)
, _dx_rtv(other._dx_rtv)
, _dx_srv(other._dx_srv)
, _dx_uav(other._dx_uav)
, _is_loaded(false)
{
    /* DO NOTHING */
}

TextureBase::TextureBase(TextureBase&& r_other) noexcept {
    
    _is_loaded = r_other._is_loaded;

    _device = r_other._device;
    _dx_rtv = r_other._dx_rtv;
    _dx_srv = r_other._dx_srv;
    _dx_dsv = r_other._dx_dsv;
    _dx_uav = r_other._dx_uav;
    _dimensions = r_other._dimensions;

    r_other._is_loaded = false;
    r_other._device = nullptr;
    r_other._dx_rtv = nullptr;
    r_other._dx_srv = nullptr;
    r_other._dx_dsv = nullptr;
    r_other._dx_uav = nullptr;
    r_other._dimensions = IntVector3::ZERO;

}

TextureBase& TextureBase::operator=(const TextureBase& rhs) noexcept {

    _is_loaded = rhs._is_loaded;

    _device = rhs._device;
    _dx_rtv = rhs._dx_rtv;
    _dx_srv = rhs._dx_srv;
    _dx_dsv = rhs._dx_dsv;
    _dx_uav = rhs._dx_uav;

    _dimensions = rhs._dimensions;

    return *this;
}

TextureBase& TextureBase::operator=(TextureBase&& r_rhs) noexcept {

    _is_loaded = r_rhs._is_loaded;

    _device = r_rhs._device;
    _dx_rtv = r_rhs._dx_rtv;
    _dx_srv = r_rhs._dx_srv;
    _dx_dsv = r_rhs._dx_dsv;
    _dx_uav = r_rhs._dx_uav;

    _dimensions = r_rhs._dimensions;

    r_rhs._is_loaded = false;
    r_rhs._device = nullptr;
    r_rhs._dx_rtv = nullptr;
    r_rhs._dx_srv = nullptr;
    r_rhs._dx_dsv = nullptr;
    r_rhs._dx_uav = nullptr;
    r_rhs._dimensions = IntVector3::ZERO;

    return *this;
}

TextureBase::~TextureBase() noexcept {
    _is_loaded = false;
    _device = nullptr;

    if(_dx_rtv) {
        _dx_rtv->Release();
        _dx_rtv = nullptr;
    }
    if(_dx_dsv) {
        _dx_dsv->Release();
        _dx_dsv = nullptr;
    }

    if(_dx_srv) {
        _dx_srv->Release();
        _dx_srv = nullptr;
    }

    if(_dx_uav) {
        _dx_uav->Release();
        _dx_uav = nullptr;
    }
}

const IntVector3& TextureBase::GetDimensions() const noexcept {
    return _dimensions;
}

IntVector3& TextureBase::GetDimensions() noexcept {
    return const_cast<IntVector3&>(static_cast<const TextureBase&>(*this).GetDimensions());
}

bool TextureBase::IsRenderTarget() const noexcept {
    return _dx_rtv != nullptr;
}

bool TextureBase::IsLoaded() const noexcept {
    return _is_loaded;
}

void TextureBase::IsLoaded(bool is_loaded) noexcept {
    _is_loaded = is_loaded;
}

ID3D11RenderTargetView* TextureBase::GetRenderTargetView() noexcept {
    return _dx_rtv;
}

ID3D11ShaderResourceView* TextureBase::GetShaderResourceView() noexcept {
    return _dx_srv;
}

ID3D11DepthStencilView* TextureBase::GetDepthStencilView() noexcept {
    return _dx_dsv;
}

ID3D11UnorderedAccessView* TextureBase::GetUnorderedAccessView() noexcept {
    return _dx_uav;
}