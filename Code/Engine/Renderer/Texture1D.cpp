#include "Engine/Renderer/Texture1D.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"

Texture1D::Texture1D() noexcept
    : TextureBase()
    , _dx_resource(nullptr)
{
    /* DO NOTHING */
}

Texture1D::Texture1D(RHIDevice* device, ID3D11Texture1D* texture) noexcept
    : TextureBase()
    , _dx_resource(nullptr)
{
    SetDeviceAndTexture(device, texture);
}

Texture1D::Texture1D(const Texture1D& other) noexcept
    : TextureBase(other)
    , _dx_resource(other._dx_resource)
{
    /* DO NOTHING */
}

Texture1D::Texture1D(Texture1D&& r_other) noexcept
    : TextureBase(r_other)
    , _dx_resource(r_other._dx_resource)
{
    r_other._dx_resource = nullptr;
}

Texture1D& Texture1D::operator=(const Texture1D& rhs) noexcept {
    TextureBase::operator=(rhs);
    _dx_resource = rhs._dx_resource;
    return *this;
}

Texture1D& Texture1D::operator=(Texture1D&& r_rhs) noexcept {
    TextureBase::operator=(std::move(r_rhs));
    _dx_resource = r_rhs._dx_resource;
    r_rhs._dx_resource = nullptr;
    return *this;
}

Texture1D::~Texture1D() {
    if(_dx_resource) {
        _dx_resource->Release();
        _dx_resource = nullptr;
    }
}


void Texture1D::SetDeviceAndTexture(RHIDevice* device, ID3D11Texture1D* texture) noexcept {

    _device = device;
    _dx_resource = texture;

    D3D11_TEXTURE1D_DESC t_desc;
    _dx_resource->GetDesc(&t_desc);
    _dimensions = IntVector3(t_desc.Width, 0, 0);

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
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
        desc.Texture1D.MipSlice = 0;
        desc.Format = t_desc.Format;

        success &= SUCCEEDED(_device->GetDxDevice()->CreateUnorderedAccessView(_dx_resource, &desc, &_dx_uav));
    }
    ASSERT_OR_DIE(success, "Set device and texture failed.");
}

bool Texture1D::IsValid() const noexcept {
    return _dx_resource != nullptr;
}

void Texture1D::SetDebugName(char const *name) noexcept {
    if((_dx_resource != nullptr) && (name != nullptr)) {
        _dx_resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name) + 1, name);
    }
}

ID3D11Texture1D* Texture1D::GetDxResource() const noexcept {
    return _dx_resource;
}
