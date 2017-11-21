#include "Engine/Renderer/Texture3D.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"

Texture3D::Texture3D() noexcept
    : TextureBase()
    , _dx_resource(nullptr)
{
    /* DO NOTHING */
}

Texture3D::Texture3D(RHIDevice* device, ID3D11Texture3D* texture) noexcept
    : TextureBase()
    , _dx_resource(nullptr)
{
    SetDeviceAndTexture(device, texture);
}

Texture3D::Texture3D(const Texture3D& other) noexcept
    : TextureBase(other)
    , _dx_resource(other._dx_resource)
{
    /* DO NOTHING */
}

Texture3D::Texture3D(Texture3D&& r_other) noexcept
    : TextureBase(r_other)
    , _dx_resource(r_other._dx_resource)
{
    r_other._dx_resource = nullptr;
}

Texture3D& Texture3D::operator=(const Texture3D& rhs) noexcept {
    TextureBase::operator=(rhs);
    _dx_resource = rhs._dx_resource;
    return *this;
}

Texture3D& Texture3D::operator=(Texture3D&& r_rhs) noexcept {
    TextureBase::operator=(std::move(r_rhs));
    _dx_resource = r_rhs._dx_resource;
    r_rhs._dx_resource = nullptr;
    return *this;
}

Texture3D::~Texture3D() {
    if(_dx_resource) {
        _dx_resource->Release();
        _dx_resource = nullptr;
    }
}


void Texture3D::SetDeviceAndTexture(RHIDevice* device, ID3D11Texture3D* texture) noexcept {
    
    _device = device;
    _dx_resource = texture;

    D3D11_TEXTURE3D_DESC t_desc;
    _dx_resource->GetDesc(&t_desc);
    _dimensions = IntVector3(t_desc.Width, t_desc.Height, t_desc.Depth);

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
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
        desc.Texture3D.MipSlice = 0;
        desc.Format = t_desc.Format;

        success &= SUCCEEDED(_device->GetDxDevice()->CreateUnorderedAccessView(_dx_resource, &desc, &_dx_uav));
    }
    ASSERT_OR_DIE(success, "Set device and texture failed.");
}

bool Texture3D::IsValid() const noexcept {
    return _dx_resource != nullptr;
}

void Texture3D::SetDebugName(char const *name) noexcept {
    if((_dx_resource != nullptr) && (name != nullptr)) {
        _dx_resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name) + 1, name);
    }
}

ID3D11Texture3D* Texture3D::GetDxResource() const noexcept {
    return _dx_resource;
}
