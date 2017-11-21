#pragma once

#include <string>

#include "Engine/RHI/RHIDevice.hpp"

class ComputeShader {
public:
    ComputeShader(const std::string& filename,
	              RHIDevice* device,
                  ID3D11ComputeShader* cs,
                  ID3DBlob* cs_bytecode,
                  ID3D11InputLayout* input_layout);
    ~ComputeShader();

    RHIDevice* GetDevice();
    ID3D11ComputeShader* GetCS();
    ID3D11InputLayout* GetInputLayout();
    ID3DBlob* GetCSBC();

    const std::string& GetName() const;

protected:
private:
    std::string _name;
    RHIDevice* _device;
    ID3D11ComputeShader* _cs;
    ID3DBlob* _cs_bytecode;
    ID3D11InputLayout* _layout;
};




//#pragma once
//
//#include <map>
//#include <utility>
//
//#include "Engine/RHI/RHIDevice.hpp"
//#include "Engine/RHI/DX11.hpp"
//
//class ShaderProgram {
//public:
//    ShaderProgram(const std::string& name,
//                  RHIDevice* device,
//                  ID3D11VertexShader* vs,
//                  ID3D11PixelShader* fs,
//                  ID3DBlob* vs_bytecode,
//                  ID3DBlob* fs_bytecode,
//                  ID3D11InputLayout* input_layout);
//    ~ShaderProgram();
//
//    ID3D11VertexShader* GetVS();
//    ID3D11PixelShader* GetFS();
//    ID3D11InputLayout* GetInputLayout();
//    ID3DBlob* GetVSBC();
//    ID3DBlob* GetFSBC();
//
//    const std::string& GetName() const;
//
//protected:
//private:
//    std::string _name;
//    RHIDevice* _device;
//    ID3DBlob* _vs_bytecode;
//    ID3DBlob* _fs_bytecode;
//    ID3D11VertexShader* _vs;
//    ID3D11PixelShader* _fs;
//    ID3D11InputLayout* _layout;
//
//};