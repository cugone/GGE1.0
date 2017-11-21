#include "Engine/rhi/RHIDeviceContext.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/ComputeJob.hpp"
#include "Engine/Renderer/ComputeShader.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/ReadWriteBuffer.hpp"
#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Renderer/DepthStencilState.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"

unsigned int RHIDeviceContext::STRUCTURED_BUFFER_OFFSET = 64;

RHIDeviceContext::RHIDeviceContext(RHIDevice* parentDevice /*= nullptr*/, ID3D11DeviceContext* dx_context /*= nullptr*/)
: _device(parentDevice)
, _dx_context(dx_context)
{
    /* DO NOTHING */
}

RHIDeviceContext::~RHIDeviceContext() {
    UnbindAllShaderResources();
    _device = nullptr;
    _dx_context->Release();
    _dx_context = nullptr;
}

void RHIDeviceContext::ClearState() {
    _dx_context->ClearState();
}
void RHIDeviceContext::Flush() {
    _dx_context->Flush();
}

void RHIDeviceContext::ClearColorTarget(Texture2D* output, const Rgba& color) {
    auto rtv = output->GetRenderTargetView();
    float colorAsFloats[4];
    color.GetAsFloats(colorAsFloats[0], colorAsFloats[1], colorAsFloats[2], colorAsFloats[3]);
    _dx_context->ClearRenderTargetView(rtv, colorAsFloats);
}

void RHIDeviceContext::ClearDepthStencilTarget(Texture2D* output
                                               , bool depth /*= true*/
                                               , bool stencil /*= true*/
                                               , float depthValue /*= 1.0f*/
                                               , unsigned char stencilValue /*= 0*/) {
    auto dsv = output->GetDepthStencilView();
    if (!dsv) {
        return;
    }
    D3D11_CLEAR_FLAG clearflag;
    if (depth) {
        clearflag = (D3D11_CLEAR_FLAG)(clearflag | D3D11_CLEAR_DEPTH);
    }
    if (stencil) {
        clearflag = (D3D11_CLEAR_FLAG)(clearflag | D3D11_CLEAR_STENCIL);
    }
    _dx_context->ClearDepthStencilView(dsv, clearflag, depthValue, stencilValue);
}

RHIDevice* RHIDeviceContext::GetParentDevice() const {
    return _device;
}

ID3D11DeviceContext* RHIDeviceContext::GetDxContext() const {
    return _dx_context;
}

void RHIDeviceContext::SetRasterState(RasterState* rs) {
    if(rs) {
        _dx_context->RSSetState(rs->GetDxRasterizer());
    } else {
        _dx_context->RSSetState(nullptr);
    }
}

void RHIDeviceContext::SetConstantBuffer(unsigned int index, ConstantBuffer* cb) {
    if(cb) {
        ID3D11Buffer* const dx_buffer = cb->GetDxBuffer();
        _dx_context->VSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->PSSetConstantBuffers(index, 1, &dx_buffer);
        _dx_context->CSSetConstantBuffers(index, 1, &dx_buffer);
    } else {
        ID3D11Buffer* nobuffer[1] = { nullptr };
        _dx_context->VSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->PSSetConstantBuffers(index, 1, nobuffer);
        _dx_context->CSSetConstantBuffers(index, 1, nobuffer);
    }
}

void RHIDeviceContext::SetStructuredBuffer(unsigned int index, StructuredBuffer* sb) {
    if(sb) {
        _dx_context->VSSetShaderResources(STRUCTURED_BUFFER_OFFSET + index, 1, &sb->dx_srv);
        _dx_context->PSSetShaderResources(STRUCTURED_BUFFER_OFFSET + index, 1, &sb->dx_srv);
        _dx_context->CSSetShaderResources(STRUCTURED_BUFFER_OFFSET + index, 1, &sb->dx_srv);
    } else {
        ID3D11ShaderResourceView* nosrvs[1] = { nullptr };
        _dx_context->VSSetShaderResources(STRUCTURED_BUFFER_OFFSET + index, 1, nosrvs);
        _dx_context->PSSetShaderResources(STRUCTURED_BUFFER_OFFSET + index, 1, nosrvs);
        _dx_context->CSSetShaderResources(STRUCTURED_BUFFER_OFFSET + index, 1, nosrvs);
    }
}

void RHIDeviceContext::SetReadWriteBuffer(unsigned int index, ReadWriteBuffer* rwb) {
    if(rwb) {
        _dx_context->CSSetShaderResources(index, 1, &rwb->dx_srv);
    } else {
        ID3D11ShaderResourceView* nosrvs[1] = { nullptr };
        _dx_context->CSSetShaderResources(index, 1, nosrvs);
    }
}

void RHIDeviceContext::SetTexture(unsigned int index, Texture2D* texture) {
    ID3D11ShaderResourceView* dx_resource = texture->GetShaderResourceView();
    _dx_context->PSSetShaderResources(index, 1, &dx_resource);
    _dx_context->VSSetShaderResources(index, 1, &dx_resource);
    _dx_context->CSSetShaderResources(index, 1, &dx_resource);
}

void RHIDeviceContext::SetUnorderedAccessViews(unsigned int index, Texture2D* texture) {
    if(texture) {
        ID3D11UnorderedAccessView* one_uav[1] = { texture->GetUnorderedAccessView() };
        _dx_context->CSSetUnorderedAccessViews(index, 1,
                                               one_uav,
                                               nullptr);
    } else {
        ID3D11UnorderedAccessView* nouavs[1] = { nullptr };
        _dx_context->CSSetUnorderedAccessViews(index, 1, nouavs, nullptr);
    }
}

void RHIDeviceContext::UnbindAllShaderResources() {
    ID3D11ShaderResourceView* nosrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
    _dx_context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nosrvs);
    _dx_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nosrvs);
    _dx_context->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nosrvs);
}

void RHIDeviceContext::SetShader(Shader* shader) {
    if(shader == nullptr) {
        SetShaderProgram(nullptr);
        SetRasterState(nullptr);
        SetDepthStencilState(nullptr);
        SetBlendState(nullptr);
    } else {
        SetShaderProgram(shader->GetShaderProgram());
        SetRasterState(shader->GetRasterState());
        SetDepthStencilState(shader->GetDepthStencilState());
        SetBlendState(shader->GetBlendState());
    }
}
void RHIDeviceContext::SetMaterial(Material* material) {
    if(material == nullptr) {
        return;
    }
    SetShader(material->GetShader());
    auto tex_count = material->GetTextureCount();
    for(std::size_t i = 0; i < tex_count; ++i) {
        SetTexture(i, material->GetTexture(i));
    }
}

void RHIDeviceContext::SetDepthStencilState(DepthStencilState* ds) {
    _dx_context->OMSetDepthStencilState(ds ? ds->GetDxDepthStencilState() : nullptr, 1);
}

void RHIDeviceContext::SetBlendState(BlendState* bs) {
    float constant[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    _dx_context->OMSetBlendState(bs ? bs->GetDxBlendState() : nullptr, constant, 0xFFFFFFFF);
}

void RHIDeviceContext::SetShaderProgram(ShaderProgram* shader) {
    if(shader) {
        auto vs = shader->GetVS();
        auto fs = shader->GetFS();
        auto il = shader->GetInputLayout();
        _dx_context->VSSetShader(vs, nullptr, 0U);
        _dx_context->PSSetShader(fs, nullptr, 0U);
        _dx_context->IASetInputLayout(il);
    } else {
        _dx_context->VSSetShader(nullptr, nullptr, 0U);
        _dx_context->PSSetShader(nullptr, nullptr, 0U);
        _dx_context->IASetInputLayout(nullptr);
    }
}

void RHIDeviceContext::SetComputeShader(ComputeShader* shader) {
    if(shader) {
        auto cs = shader->GetCS();
        auto il = shader->GetInputLayout();
        _dx_context->CSSetShader(cs, nullptr, 0U);
        _dx_context->IASetInputLayout(il);
    } else {
        _dx_context->CSSetShader(nullptr, nullptr, 0U);
        _dx_context->IASetInputLayout(nullptr);
    }
}