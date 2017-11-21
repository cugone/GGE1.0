#include "Engine/Renderer/ConstantBuffer.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

ConstantBuffer::ConstantBuffer(RHIDevice* owner, const void* buffer, unsigned int buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage)
    : IBuffer()
    , _buffer_size(buffer_size)
{

    D3D11_BUFFER_DESC vb_desc;
    memset(&vb_desc, 0, sizeof(vb_desc));

    vb_desc.ByteWidth = _buffer_size;
    vb_desc.BindFlags = BufferBindUsageToD3DBindFlags(bindUsage);
    vb_desc.Usage = BufferUsageToD3DUsage(usage);
    vb_desc.StructureByteStride = 0;
    vb_desc.CPUAccessFlags = CPUAccessFlagFromUsage(usage);


    D3D11_SUBRESOURCE_DATA initial_data;
    memset(&initial_data, 0, sizeof(initial_data));
    initial_data.pSysMem = buffer;

    _dx_buffer = nullptr;
    HRESULT hr = owner->GetDxDevice()->CreateBuffer(&vb_desc, &initial_data, &_dx_buffer);
    bool succeeded = SUCCEEDED(hr);
    if (!succeeded) {
        ERROR_AND_DIE("Constant buffer failed to create.");
    }
}
ConstantBuffer::~ConstantBuffer() {
    _dx_buffer->Release();
    _dx_buffer = nullptr;
}
void ConstantBuffer::Update(RHIDeviceContext* context, void* buffer) {
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* dx_context = context->GetDxContext();
    bool succeeded = SUCCEEDED(dx_context->Map(_dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource));
    if(succeeded) {
        std::memcpy(resource.pData, buffer, _buffer_size);
        dx_context->Unmap(_dx_buffer, 0);
    }
}