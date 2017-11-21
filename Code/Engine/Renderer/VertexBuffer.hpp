#pragma once

#include "Engine/Renderer/IBuffer.hpp"
#include "Engine/Renderer/Vertex3D.hpp"

#include <vector>

enum class BufferUsage : unsigned int;
enum class BufferBindUsage : unsigned int;

class RHIDevice;
class RHIDeviceContext;

class VertexBuffer : public IBuffer {
public:
	VertexBuffer(RHIDevice* owner, const std::vector<Vertex3D>& buffer, const BufferUsage& usage, const BufferBindUsage& bindUsage);
	virtual ~VertexBuffer();

    void Update(RHIDeviceContext* context, const std::vector<Vertex3D>& buffer);

protected:
private:

};