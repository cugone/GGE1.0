#include "Engine/Renderer/ShaderProgram.hpp"

#include <filesystem>
#include <sstream>

#include "Engine/RHI/DX11.hpp"

#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

ShaderProgram::ShaderProgram(const std::string& name,
                             RHIDevice* device,
                             ID3D11VertexShader* vs,
                             ID3D11PixelShader* fs,
                             ID3DBlob* vs_bytecode,
                             ID3DBlob* fs_bytecode,
                             ID3D11InputLayout* input_layout,
                             ID3D11GeometryShader* gs /*= nullptr*/,
                             ID3DBlob* gs_bytecode /*= nullptr*/)
    : _name(name)
    , _device(device)
    , _vs_bytecode(vs_bytecode)
    , _fs_bytecode(fs_bytecode)
    , _gs_bytecode(gs_bytecode)
    , _vs(vs)
    , _fs(fs)
    , _gs(gs)
    , _layout(input_layout)
{
    /* DO NOTHING */
}
ShaderProgram::~ShaderProgram() {
    _device = nullptr;

    _vs->Release();
    _vs_bytecode->Release();
    _vs = nullptr;
    _vs_bytecode = nullptr;

    _fs->Release();
    _fs_bytecode->Release();
    _fs = nullptr;
    _fs_bytecode = nullptr;

    _layout->Release();
    _layout = nullptr;

    if(_gs) {
        _gs->Release();
        _gs = nullptr;
    }
    if(_gs_bytecode) {
        _gs_bytecode->Release();
        _gs_bytecode = nullptr;
    }

}

ID3D11VertexShader* ShaderProgram::GetVS() {
    return _vs;
}
ID3D11PixelShader* ShaderProgram::GetFS() {
    return _fs;
}
ID3D11GeometryShader* ShaderProgram::GetGS() {
    return _gs;
}
ID3DBlob* ShaderProgram::GetVSBC() {
    return _vs_bytecode;
}
ID3DBlob* ShaderProgram::GetFSBC() {
    return _fs_bytecode;
}
ID3DBlob* ShaderProgram::GetGSBC() {
    return _gs_bytecode;
}

ID3D11InputLayout* ShaderProgram::GetInputLayout() {
    return _layout;
}
const std::string& ShaderProgram::GetName() const {
    return _name;
}
