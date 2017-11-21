#include "Engine/Renderer/IndexBuffer.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

IndexBuffer::IndexBuffer(RHIDevice* owner, const std::vector<unsigned int>& buffer, const BufferUsage& usage, const BufferBindUsage& bindUsage)
{

    D3D11_BUFFER_DESC vb_desc;
    memset(&vb_desc, 0, sizeof(vb_desc));

    vb_desc.ByteWidth = sizeof(unsigned int) * buffer.size();
    vb_desc.Usage = BufferUsageToD3DUsage(usage);
    vb_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    vb_desc.StructureByteStride = sizeof(unsigned int);
    vb_desc.CPUAccessFlags = CPUAccessFlagFromUsage(usage);

    D3D11_SUBRESOURCE_DATA initial_data;
    memset(&initial_data, 0, sizeof(initial_data));
    initial_data.pSysMem = buffer.data();

    _dx_buffer = nullptr;
    HRESULT hr = owner->GetDxDevice()->CreateBuffer(&vb_desc, &initial_data, &_dx_buffer);
    bool succeeded = SUCCEEDED(hr);
    if (!succeeded) {
        ERROR_AND_DIE("Index Buffer failed to create.");
    }
}

IndexBuffer::~IndexBuffer() {
    _dx_buffer->Release();
    _dx_buffer = nullptr;
}

void IndexBuffer::Update(RHIDeviceContext* context, const std::vector<unsigned int>& buffer) {
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* dx_context = context->GetDxContext();
    bool succeeded = SUCCEEDED(dx_context->Map(_dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource));
    if(succeeded) {
        std::memcpy(resource.pData, buffer.data(), sizeof(unsigned int) * buffer.size());
        dx_context->Unmap(_dx_buffer, 0);
    }
}