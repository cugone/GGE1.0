#include "Engine/Renderer/StructuredBuffer.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//BIND AS DYNAMIC SHADER RESOURCE!
StructuredBuffer::StructuredBuffer(RHIDevice* owner, const void* data, unsigned int element_size, unsigned int element_count, const BufferUsage& usage, const BufferBindUsage& bindFlag)
    : IBuffer()
    , elem_count(element_count)
    , elem_size(element_size)
    , dx_srv(nullptr)
    , buffer_size(elem_count * elem_size)
{
    D3D11_BUFFER_DESC vb_desc;
    memset(&vb_desc, 0, sizeof(vb_desc));

    vb_desc.ByteWidth = static_cast<unsigned int>(buffer_size);
    vb_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindFlag);
    vb_desc.Usage = BufferUsageToD3DUsage(usage);
    vb_desc.CPUAccessFlags = CPUAccessFlagFromUsage(usage);
    vb_desc.StructureByteStride = elem_size;
    vb_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA initial_data;
    memset(&initial_data, 0, sizeof(initial_data));
    initial_data.pSysMem = data;

    owner->GetDxDevice()->CreateBuffer(&vb_desc, &initial_data, &_dx_buffer);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desc.Buffer.ElementOffset = 0u;
    desc.Buffer.NumElements = element_count;

    HRESULT result = owner->GetDxDevice()->CreateShaderResourceView(_dx_buffer, &desc, &dx_srv);
    bool succeeded = SUCCEEDED(result);
    if(!succeeded) {
        ERROR_AND_DIE("Failed to create Structured Buffer's SRV.");
    }

}
StructuredBuffer::~StructuredBuffer() {
    _dx_buffer->Release();
    _dx_buffer = nullptr;

    dx_srv->Release();
    dx_srv = nullptr;

}
void StructuredBuffer::Update(RHIDeviceContext* context, const void* buffer) {
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* dx_context = context->GetDxContext();

    bool succeeded = SUCCEEDED(dx_context->Map(_dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0u, &resource));
    if(succeeded) {
        std::memcpy(resource.pData, buffer, buffer_size);
        dx_context->Unmap(_dx_buffer, 0);
    }
}