#pragma once

#include <string>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"

class ComputeShader;
class IBuffer;
class RHIDevice;
class RHIDeviceContext;
class Texture2D;

class ComputeJob {
public:
	ComputeJob(ComputeShader* computeShader);
    ~ComputeJob();

    
    void SetBuffer(IBuffer* buffer);

    void SetShader(ComputeShader* computeShader);
    void SetReadWriteTexture(Texture2D* texture);
    void SetGridDimensions(const Vector3& dimensions);

    const Vector3& GetGridDimensions() const;
    const Texture2D* GetTexture() const;
    IBuffer* GetBuffer() const;

    void Update(RHIDeviceContext* context, void* data);
protected:
private:
    ComputeShader* _shader;
    IBuffer* _buffer;
    Texture2D* _texture;
    Vector3 _gridDimensions;
};