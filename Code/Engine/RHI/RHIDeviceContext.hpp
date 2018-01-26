#pragma once

#include "Engine/Renderer/ComputeJob.hpp"

class Rgba;
class Texture2D;
class RHIDevice;
class RasterState;
class BlendState;
class ComputeShader;
class ConstantBuffer;
class StructuredBuffer;
class ReadWriteBuffer;
class DepthStencilBuffer;
class DepthStencilState;
class ShaderProgram;
class Shader;
class Material;

struct ID3D11DeviceContext;

class RHIDeviceContext {
public:
    RHIDeviceContext(RHIDevice* parentDevice = nullptr, ID3D11DeviceContext* dx_context = nullptr);
    ~RHIDeviceContext();

    // For an immediate/display context, we need the window we're rendering to
    // and the swapchain that manages the render target
    void ClearState();  // clears all rendering state
    void Flush();        // flushes all commands

    void ClearColorTarget(Texture2D* output, const Rgba& color);
    void ClearDepthStencilTarget(Texture2D* output
                                 , bool depth = true
                                 , bool stencil = true
                                 , float depthValue = 1.0f
                                 , unsigned char stencilValue = 0);

    RHIDevice* GetParentDevice() const;
    ID3D11DeviceContext* GetDxContext() const;

    void SetConstantBuffer(unsigned int index, ConstantBuffer* cb);
    void SetStructuredBuffer(unsigned int index, StructuredBuffer* sb);
    void SetReadWriteBuffer(unsigned int index, ReadWriteBuffer* rwb);

    void SetTexture(unsigned int index, Texture2D* texture);
    void SetUnorderedAccessViews(unsigned int index, Texture2D* texture);
    void SetRasterState(RasterState* rs);
    void SetDepthStencilState(DepthStencilState* ds);
    void SetBlendState(BlendState* bs);

    void SetShaderProgram(ShaderProgram* shader);
    void SetComputeShader(ComputeShader* shader);

    void UnbindAllShaderResources();

    template<typename T>
    void Dispatch(ComputeJob* job, T& job_data);

    void SetShader(Shader* shader);
    void SetMaterial(Material* material);

private:
    RHIDevice* _device;
    ID3D11DeviceContext* _dx_context;

    static unsigned int STRUCTURED_BUFFER_OFFSET;
};

template<typename T>
void RHIDeviceContext::Dispatch(ComputeJob* job, T& job_data) {
    job->Update(this, &job_data);
    const auto& dims = job->GetGridDimensions();
    auto x = static_cast<unsigned int>(dims.x);
    auto y = static_cast<unsigned int>(dims.y);
    auto z = static_cast<unsigned int>(dims.z);
    _dx_context->Dispatch(x, y, z);
    SetUnorderedAccessViews(0, nullptr);
}
