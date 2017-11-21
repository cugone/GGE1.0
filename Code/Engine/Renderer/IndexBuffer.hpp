#pragma once

#include "Engine/Renderer/IBuffer.hpp"
#include <vector>

enum class BufferUsage : unsigned int;
enum class BufferBindUsage : unsigned int;

class RHIDevice;
class RHIDeviceContext;
struct ID3D11Buffer;

class IndexBuffer : public IBuffer {
public:
	IndexBuffer(RHIDevice* owner, const std::vector<unsigned int>& buffer, const BufferUsage& usage, const BufferBindUsage& bindUsage);
	virtual ~IndexBuffer();

    void Update(RHIDeviceContext* context, const std::vector<unsigned int>& buffer);

protected:
private:
};