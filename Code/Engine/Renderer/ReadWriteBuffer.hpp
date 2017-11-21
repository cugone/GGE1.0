#pragma once

#include "Engine/Renderer/IBuffer.hpp"

enum class BufferUsage : unsigned int;
enum class BufferBindUsage : unsigned int;

class RHIDevice;
class RHIDeviceContext;

class ReadWriteBuffer : public IBuffer {
public:
    ReadWriteBuffer(RHIDevice* owner, const void* data, unsigned int element_size, unsigned int element_count, const BufferUsage& usage, const BufferBindUsage& bindFlag);
    virtual ~ReadWriteBuffer() override;

    void Update(RHIDeviceContext* context, const void* buffer);

    ID3D11ShaderResourceView* dx_srv;
protected:

    unsigned int elem_count;
    unsigned int elem_size;
    std::size_t buffer_size;
private:

};