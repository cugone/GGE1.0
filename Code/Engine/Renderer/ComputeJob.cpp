#include "Engine/Renderer/ComputeJob.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/ComputeShader.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/IBuffer.hpp"

ComputeJob::ComputeJob(ComputeShader* computeShader)
    : _shader(computeShader)
    , _buffer(nullptr)
    , _texture(nullptr)
    , _gridDimensions(Vector3::ONE)
{
    /* DO NOTHING */
}

ComputeJob::~ComputeJob() {
    delete _buffer;
    _buffer = nullptr;
}

void ComputeJob::SetBuffer(IBuffer* buffer) {
    if(_buffer) {
        delete _buffer;
        _buffer = nullptr;
    }
    _buffer = buffer;
}

IBuffer* ComputeJob::GetBuffer() const {
    return _buffer;
}
void ComputeJob::SetShader(ComputeShader* computeShader) {
    _shader = computeShader;
}

void ComputeJob::SetReadWriteTexture(Texture2D* texture) {
    _texture = texture;
}

void ComputeJob::SetGridDimensions(const Vector3& dimensions) {
    _gridDimensions = dimensions;
}
const Vector3& ComputeJob::GetGridDimensions() const {
    return _gridDimensions;
}
const Texture2D* ComputeJob::GetTexture() const {
    return _texture;
}

void ComputeJob::Update(RHIDeviceContext* context, void* data) {

    context->SetUnorderedAccessViews(0, _texture);
    ConstantBuffer* bufferAsCB = dynamic_cast<ConstantBuffer*>(_buffer);
    StructuredBuffer* bufferAsSB = dynamic_cast<StructuredBuffer*>(_buffer);
    if(dynamic_cast<ConstantBuffer*>(_buffer)) {
        context->SetConstantBuffer(0, bufferAsCB);
        bufferAsCB->Update(context, data);
    }
    if(dynamic_cast<StructuredBuffer*>(_buffer)) {
        context->SetStructuredBuffer(0, bufferAsSB);
        bufferAsCB->Update(context, data);
    }
    context->SetComputeShader(_shader);

}