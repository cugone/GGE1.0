#pragma once

#include "Engine/Renderer/IBuffer.hpp"

class RHIDevice;
class RHIDeviceContext;

enum class BufferUsage : unsigned int;
enum class BufferBindUsage : unsigned int;

class ConstantBuffer : public IBuffer {
public:
	ConstantBuffer(RHIDevice* owner, const void* buffer, unsigned int buffer_size, const BufferUsage& usage, const BufferBindUsage& bindUsage);
	virtual ~ConstantBuffer();

    void Update(RHIDeviceContext* context, void* buffer);
protected:
private:
	unsigned int _buffer_size;
};