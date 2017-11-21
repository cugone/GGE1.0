#include "Engine/Renderer/ComputeShader.hpp"

ComputeShader::ComputeShader(const std::string& filename,
                             RHIDevice* device,
                             ID3D11ComputeShader* cs,
                             ID3DBlob* cs_bytecode,
                             ID3D11InputLayout* input_layout)
    : _name(filename)
    , _device(device)
    , _cs(cs)
    , _cs_bytecode(cs_bytecode)
    , _layout(input_layout)
{
    /* DO NOTHING */
}

ComputeShader::~ComputeShader() {
    _device = nullptr;

    _cs->Release();
    _cs_bytecode->Release();

    _layout->Release();

}
RHIDevice* ComputeShader::GetDevice() {
    return _device;
}
ID3D11ComputeShader* ComputeShader::GetCS() {
    return _cs;
}

ID3D11InputLayout* ComputeShader::GetInputLayout() {
    return _layout;
}

ID3DBlob* ComputeShader::GetCSBC() {
    return _cs_bytecode;
}

const std::string& ComputeShader::GetName() const {
    return _name;
}