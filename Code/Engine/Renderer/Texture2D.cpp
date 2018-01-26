#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"

Texture2D::Texture2D() noexcept
    : TextureBase()
    , _dx_resource(nullptr)
{
    /* DO NOTHING */
}

Texture2D::Texture2D(RHIDevice* device, ID3D11Texture2D* texture) noexcept
    : TextureBase()
    , _dx_resource(nullptr)
{
    SetDeviceAndTexture(device, texture);
}

Texture2D::Texture2D(const Texture2D& other) noexcept
    : TextureBase(other)
    , _dx_resource(other._dx_resource)
{
    /* DO NOTHING */
}

Texture2D::Texture2D(Texture2D&& r_other) noexcept
    : TextureBase(r_other)
    , _dx_resource(r_other._dx_resource)
{
    r_other._dx_resource = nullptr;
}

Texture2D& Texture2D::operator=(const Texture2D& rhs) noexcept {
    TextureBase::operator=(rhs);
    _dx_resource = rhs._dx_resource;
    return *this;
}

Texture2D& Texture2D::operator=(Texture2D&& r_rhs) noexcept {
    TextureBase::operator=(std::move(r_rhs));
    _dx_resource = r_rhs._dx_resource;
    r_rhs._dx_resource = nullptr;
    return *this;
}

Texture2D::~Texture2D() {
    if(_dx_resource) {
        _dx_resource->Release();
        _dx_resource = nullptr;
    }
}


void Texture2D::SetDeviceAndTexture(RHIDevice* device, ID3D11Texture2D* texture) noexcept {
    
    _device = device;
    _dx_resource = texture;

    D3D11_TEXTURE2D_DESC t_desc;
    _dx_resource->GetDesc(&t_desc);
    _dimensions = IntVector3(t_desc.Width, t_desc.Height, 0);

    bool success = true;
    if(t_desc.BindFlags & D3D11_BIND_RENDER_TARGET) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateRenderTargetView(_dx_resource, nullptr, &_dx_rtv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateShaderResourceView(_dx_resource, nullptr, &_dx_srv)) == true;
    }

    if(t_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
        success &= SUCCEEDED(_device->GetDxDevice()->CreateDepthStencilView(_dx_resource, nullptr, &_dx_dsv));
    }

    if(t_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;
        desc.Format = t_desc.Format;

        success &= SUCCEEDED(_device->GetDxDevice()->CreateUnorderedAccessView(_dx_resource, &desc, &_dx_uav));
    }
    ASSERT_OR_DIE(success, "Set device and texture failed.");
}

bool Texture2D::IsValid() const noexcept {
    return _dx_resource != nullptr;
}

void Texture2D::SetDebugName( [[maybe_unused]] char const *name) noexcept {
#if _DEBUG
    if((_dx_resource != nullptr) && (name != nullptr)) {
        _dx_resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name) + 1, name);
    }
#endif
}

ID3D11Texture2D* Texture2D::GetDxResource() const noexcept {
    return _dx_resource;
}
