#include "Engine/RHI/RHIDevice.hpp"

#include <filesystem>
#include <type_traits>
#include <sstream>

#include "Engine/RHI/DX11.hpp"

#include "Engine/EngineConfig.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Window.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/FileUtils.hpp"

#include "Engine/Renderer/ComputeShader.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/ReadWriteBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"

RHIDevice::RHIDevice(ID3D11Device* dx_device /*= nullptr*/)
    : _immediate_context(nullptr)
    , _dx_device(dx_device)
{
    /* DO NOTHING */
}
RHIDevice::~RHIDevice() {
    delete _immediate_context;
    _dx_device->Release();
    _dx_device = nullptr;
}

void RHIDevice::SetImmediateContext(RHIDeviceContext* immediateContext) {
    _immediate_context = immediateContext;
}
RHIDeviceContext* RHIDevice::GetImmediateContext() const {
    return _immediate_context;
}

ID3D11Device* RHIDevice::GetDxDevice() const {
    return _dx_device;
}

RHIOutput* RHIDevice::CreateOutput(Window* window, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    return CreateOutputFromWindow(window, outputMode);

}

RHIOutput* RHIDevice::CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition /*= IntVector2::ZERO*/, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    Window* window = new Window;
    window->SetClientSize(clientPosition, clientSize);
    return CreateOutputFromWindow(window, outputMode);
}

ComputeShader* RHIDevice::CreateComputeShaderFromHlslFile(const std::string& filename) {
    bool requested_retry = false;
    std::vector<unsigned char> buffer;
    ID3DBlob* cs_byte_code = nullptr;

    do {
        if(!FileUtils::ReadBufferFromFile(buffer, filename)) {
            return nullptr;
        }

        cs_byte_code = CompileHlslToShaderBlob(filename.c_str(), buffer.data(), buffer.size(), "Main", "cs_5_0");

        requested_retry = false;
#ifdef RENDER_DEBUG
        if(cs_byte_code == nullptr) {
            std::ostringstream error_msg;
            error_msg << "Compute Shader File \"" << filename << "\" failed to compile.";
            error_msg << "\nSee Output window for details.";
            error_msg << "\nPress Retry to re-compile.";
            auto buttonID = MessageBoxA(nullptr,
                                        error_msg.str().c_str(),
                                        "Compilation Error",
                                        MB_RETRYCANCEL | MB_ICONERROR);
            requested_retry = buttonID == IDRETRY;
        }
#endif
    } while(requested_retry);

    buffer.clear();
    buffer.shrink_to_fit();

    if(cs_byte_code != nullptr) {
        ID3D11ComputeShader* cs;

        _dx_device->CreateComputeShader(cs_byte_code->GetBufferPointer(), cs_byte_code->GetBufferSize(), nullptr, &cs);

        namespace FS = std::experimental::filesystem::v1;
        FS::path p(filename);
        ID3D11InputLayout* il = CreateInputLayout(cs_byte_code);
        ComputeShader* compute_shader = new ComputeShader(p.filename().string(), this, cs, cs_byte_code, il);
        il = nullptr;
        return compute_shader;
    }
    return nullptr;
}

ComputeShader* RHIDevice::CreateComputeShaderFromHlslString(const std::string& name, const std::string& shader) {
    bool requested_retry = false;
    ID3DBlob* cs_byte_code = nullptr;

    do {

        cs_byte_code = CompileHlslToShaderBlob(nullptr, shader.data(), shader.size(), "main", "cs_5_0");
        requested_retry = false;
#ifdef RENDER_DEBUG
        if(cs_byte_code == nullptr) {
            std::ostringstream error_msg;
            error_msg << "Compute Shader File \"" << name << "\" failed to compile.";
            error_msg << "\nSee Output window for details.";
            error_msg << "\nPress Retry to re-compile.";
            auto buttonID = MessageBoxA(nullptr,
                                        error_msg.str().c_str(),
                                        "Compilation Error",
                                        MB_RETRYCANCEL | MB_ICONERROR);
            requested_retry = buttonID == IDRETRY;
        }
#endif
    } while(requested_retry);

    if(cs_byte_code != nullptr) {
        ID3D11ComputeShader* cs;

        _dx_device->CreateComputeShader(cs_byte_code->GetBufferPointer(), cs_byte_code->GetBufferSize(), nullptr, &cs);
        auto il = CreateInputLayout(cs_byte_code);
        ComputeShader* program = new ComputeShader(name, this, cs, cs_byte_code, il);
        return program;
    }
    return nullptr;
}

ShaderProgram* RHIDevice::CreateShaderFromHlslFile(const std::string& filename) {
    bool requested_retry = false;
    std::vector<unsigned char> buffer;
    ID3DBlob* vs_byte_code = nullptr;
    ID3DBlob* fs_byte_code = nullptr;

    do {
        if(!FileUtils::ReadBufferFromFile(buffer, filename)) {
            return nullptr;
        }


        vs_byte_code = CompileHlslToShaderBlob(filename.c_str(), buffer.data(), buffer.size(), "VertexFunction", "vs_5_0");
        fs_byte_code = CompileHlslToShaderBlob(filename.c_str(), buffer.data(), buffer.size(), "FragmentFunction", "ps_5_0");
        requested_retry = false;
#ifdef RENDER_DEBUG
        if(vs_byte_code == nullptr || fs_byte_code == nullptr) {
            std::ostringstream error_msg;
            error_msg << "Shader File \"" << filename << "\" failed to compile.";
            error_msg << "\nSee Output window for details.";
            error_msg << "\nPress Retry to re-compile.";
            auto buttonID = MessageBoxA(nullptr,
                                        error_msg.str().c_str(),
                                        "Compilation Error",
                                        MB_RETRYCANCEL | MB_ICONERROR);
            requested_retry = buttonID == IDRETRY;
        }
#endif
    } while(requested_retry);

    buffer.clear();
    buffer.shrink_to_fit();

    if((vs_byte_code != nullptr) &&
        (fs_byte_code != nullptr)
       ) {
        ID3D11VertexShader* vs;
        ID3D11PixelShader* fs;

        _dx_device->CreateVertexShader(vs_byte_code->GetBufferPointer(), vs_byte_code->GetBufferSize(), nullptr, &vs);
        _dx_device->CreatePixelShader(fs_byte_code->GetBufferPointer(), fs_byte_code->GetBufferSize(), nullptr, &fs);

        namespace FS = std::experimental::filesystem::v1;
        FS::path p(filename);
        ID3D11InputLayout* il = CreateInputLayout(vs_byte_code);
        ShaderProgram* program = new ShaderProgram(p.string(), this, vs, fs, vs_byte_code, fs_byte_code, il);
        il = nullptr;
        return program;
    }
    return nullptr;
}

ShaderProgram* RHIDevice::CreateShaderFromHlslString(const std::string& name, const std::string& shader) {
    bool requested_retry = false;
    ID3DBlob* vs_byte_code = nullptr;
    ID3DBlob* fs_byte_code = nullptr;

    do {

        vs_byte_code = CompileHlslToShaderBlob(nullptr, shader.data(), shader.size(), "VertexFunction", "vs_5_0");
        fs_byte_code = CompileHlslToShaderBlob(nullptr, shader.data(), shader.size(), "FragmentFunction", "ps_5_0");
        requested_retry = false;
#ifdef RENDER_DEBUG
        if(vs_byte_code == nullptr || fs_byte_code == nullptr) {
            std::ostringstream error_msg;
            error_msg << "Shader File \"" << name << "\" failed to compile.";
            error_msg << "\nSee Output window for details.";
            error_msg << "\nPress Retry to re-compile.";
            auto buttonID = MessageBoxA(nullptr,
                                        error_msg.str().c_str(),
                                        "Compilation Error",
                                        MB_RETRYCANCEL | MB_ICONERROR);
            requested_retry = buttonID == IDRETRY;
        }
#endif
    } while(requested_retry);

    if((vs_byte_code != nullptr) &&
        (fs_byte_code != nullptr)
       ) {
        ID3D11VertexShader* vs;
        ID3D11PixelShader* fs;

        _dx_device->CreateVertexShader(vs_byte_code->GetBufferPointer(), vs_byte_code->GetBufferSize(), nullptr, &vs);
        _dx_device->CreatePixelShader(fs_byte_code->GetBufferPointer(), fs_byte_code->GetBufferSize(), nullptr, &fs);
        auto il = CreateInputLayout(vs_byte_code);
        ShaderProgram* program = new ShaderProgram(name, this, vs, fs, vs_byte_code, fs_byte_code, il);
        return program;
    }
    return nullptr;
}

VertexBuffer* RHIDevice::CreateVertexBuffer(const std::vector<Vertex3D>& buffer, const BufferUsage& usage, const BufferBindUsage& bindFlag) {
    auto vbo = new VertexBuffer(this, buffer, usage, bindFlag);
    return vbo;
}

IndexBuffer* RHIDevice::CreateIndexBuffer(const std::vector<unsigned int>& buffer, const BufferUsage& usage, const BufferBindUsage& bindFlag) {
    auto ibo = new IndexBuffer(this, buffer, usage, bindFlag);
    return ibo;
}
ConstantBuffer* RHIDevice::CreateConstantBuffer(void* buffer, unsigned int buffer_size, const BufferUsage& usage, const BufferBindUsage& bindFlag) {
    auto cbuf = new ConstantBuffer(this, buffer, buffer_size, usage, bindFlag);
    return cbuf;
}

ReadWriteBuffer* RHIDevice::CreateReadWriteBuffer(const void* data, unsigned int element_size, unsigned int element_count, const BufferUsage& usage, const BufferBindUsage& bindFlag) {
    auto rwbuf = new ReadWriteBuffer(this, data, element_size, element_count, usage, bindFlag);
    return rwbuf;
}
StructuredBuffer* RHIDevice::CreateStructuredBuffer(const void* data, unsigned int element_size, unsigned int element_count, const BufferUsage& usage, const BufferBindUsage& bindFlag) {
    auto sbuf = new StructuredBuffer(this, data, element_size, element_count, usage, bindFlag);
    return sbuf;
}
RHIOutput* RHIDevice::CreateOutputFromWindow(Window*& window, const RHIOutputMode& outputMode) {
    if(window == nullptr) {
        return nullptr;
    }

    unsigned int device_flags = 0U;
#ifdef RENDER_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
    // This flag fails unless we' do 11.1 (which we're not), and we query that
    // the adapter support its (which we're not).  Just here to let you know it exists.
    // device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

    IntVector2 clientSize = window->GetClientSize();
    if(!window->IsOpen()) {
        window->Open();
    }

    ID3D11DeviceContext* dx_context;
    ID3D11Device* dx_device;
    IDXGISwapChain* dx_swapchain;


    DXGI_SWAP_CHAIN_DESC swap_desc;
    memset(&swap_desc, 0, sizeof(swap_desc));

    // fill the swap chain description struct
    swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

    swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
        | DXGI_USAGE_BACK_BUFFER;
    swap_desc.OutputWindow = window->GetWindowHandle();                                // the window to be copied to on present
    swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

                                                                  // Default options.
    bool isWindowed = outputMode == RHIOutputMode::WINDOWED || outputMode == RHIOutputMode::FULLSCREEN_WINDOW;
    swap_desc.Windowed = isWindowed ? TRUE : FALSE;                                    // windowed/full-screen mode
    swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    swap_desc.BufferDesc.Width = clientSize.x;
    swap_desc.BufferDesc.Height = clientSize.y;

    HRESULT hr = ::D3D11CreateDeviceAndSwapChain(nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
                                                 D3D_DRIVER_TYPE_HARDWARE,  // Driver Type - We want to use the GPU (HARDWARE)
                                                 nullptr,                   // Software Module - DLL that implements software mode (we do not use)
                                                 device_flags,              // device creation options
                                                 nullptr,                   // feature level (use default)
                                                 0U,                        // number of feature levels to attempt
                                                 D3D11_SDK_VERSION,         // SDK Version to use
                                                 &swap_desc,                // Description of our swap chain
                                                 &dx_swapchain,            // Swap Chain we're creating
                                                 &dx_device,               // [out] The device created
                                                 nullptr,                   // [out] Feature Level Acquired
                                                 &dx_context);            // Context that can issue commands on this pipe.

                                                                          // SUCCEEDED & FAILED are macros provided by Windows to checking
                                                                          // the results.  Almost every D3D call will return one - be sure to check it.
    bool succeeded = SUCCEEDED(hr);
    if(succeeded) {

        RHIDevice* rhi_device = new RHIDevice(dx_device);
        RHIDeviceContext* rhi_context = new RHIDeviceContext(rhi_device, dx_context);
        rhi_device->SetImmediateContext(rhi_context);

        RHIOutput* rhi_output = new RHIOutput(rhi_device, window, dx_swapchain);

        return rhi_output;
    } else {
        delete window;
        window = nullptr;
    }
    return nullptr;
}



#if defined(_DEBUG)
#define DEBUG_SHADERS
#endif

ID3DBlob* RHIDevice::CompileHlslToShaderBlob(char const *opt_filename,
                                         void const *source_code,
                                         size_t const source_code_size,
                                         char const *entrypoint,
                                         char const* target)
{
    /* DEFINE MACROS - CONTROLS SHADER
    // You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
    D3D_SHADER_MACRO defines[1];
    defines[0].Name = "TEST_MACRO";
    defines[0].Definition = nullptr;
    */

    DWORD compile_flags = 0U;
#if defined(DEBUG_SHADERS)
    compile_flags |= D3DCOMPILE_DEBUG;
    compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
    compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
    compile_flags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR; // my matricies are column major;
#else 
    // compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
    compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

    ID3DBlob *code = nullptr;
    ID3DBlob *errors = nullptr;

    HRESULT hr = ::D3DCompile(source_code,                        // plain text source code
                              source_code_size,                   // size of source code in bytes
                              opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
                              nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
                              D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
                              entrypoint,                         // Entry Point for this shader
                              target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
                              compile_flags,                      // Flags that control compilation
                              0,                                  // Effect Flags (we will not be doing Effect Files)
                              &code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
                              &errors);                          // [OUT] ID3DBlob (buffer) that will store error information

    if(FAILED(hr) || (errors != nullptr)) {
        if(errors != nullptr) {
            char *error_string = (char*)errors->GetBufferPointer();
            DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
                 opt_filename,
                 error_string);
            errors->Release();
            errors = nullptr;
        }
    }

    // will be nullptr if it failed to compile
    return code;
}

ID3D11InputLayout* RHIDevice::CreateInputLayout(ID3DBlob* vs_bytecode) {
    D3D11_INPUT_ELEMENT_DESC desc[8];
    memset(desc, 0, sizeof(desc));

    desc[0].SemanticName = "POSITION";
    desc[0].SemanticIndex = 0;
    desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    desc[0].InputSlot = 0U;
    desc[0].AlignedByteOffset = offsetof(Vertex3D, position);
    desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    desc[0].InstanceDataStepRate = 0U;

    desc[1].SemanticName = "COLOR";                   // name we gave this input
    desc[1].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // Type this data is (float2/vec2 - so 2 floats)
    desc[1].InputSlot = 0U;                        // Input Pipe this comes from
    desc[1].AlignedByteOffset = offsetof(Vertex3D, color);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[1].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    desc[2].SemanticName = "TEXCOORD";                   // name we gave this input
    desc[2].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[2].Format = DXGI_FORMAT_R32G32_FLOAT;     // Type this data is (float2/vec2 - so 2 floats)
    desc[2].InputSlot = 0U;                        // Input Pipe this comes from
    desc[2].AlignedByteOffset = offsetof(Vertex3D, texCoords);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[2].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    desc[3].SemanticName = "NORMAL";                   // name we gave this input
    desc[3].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;     // Type this data is (float2/vec2 - so 2 floats)
    desc[3].InputSlot = 0U;                        // Input Pipe this comes from
    desc[3].AlignedByteOffset = offsetof(Vertex3D, normal);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[3].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    desc[4].SemanticName = "TANGENT";                   // name we gave this input
    desc[4].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;     // Type this data is (float2/vec2 - so 2 floats)
    desc[4].InputSlot = 0U;                        // Input Pipe this comes from
    desc[4].AlignedByteOffset = offsetof(Vertex3D, tangent);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[4].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    desc[5].SemanticName = "BITANGENT";                   // name we gave this input
    desc[5].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;     // Type this data is (float2/vec2 - so 2 floats)
    desc[5].InputSlot = 0U;                        // Input Pipe this comes from
    desc[5].AlignedByteOffset = offsetof(Vertex3D, bitangent);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[5].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    desc[6].SemanticName = "BONE_INDICES";                   // name we gave this input
    desc[6].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[6].Format = DXGI_FORMAT_R32G32B32A32_UINT;     // Type this data is (float2/vec2 - so 2 floats)
    desc[6].InputSlot = 0U;                        // Input Pipe this comes from
    desc[6].AlignedByteOffset = offsetof(Vertex3D, bone_indices);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[6].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    desc[7].SemanticName = "BONE_WEIGHTS";                   // name we gave this input
    desc[7].SemanticIndex = 0;                     // Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
    desc[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;     // Type this data is (float2/vec2 - so 2 floats)
    desc[7].InputSlot = 0U;                        // Input Pipe this comes from
    desc[7].AlignedByteOffset = offsetof(Vertex3D, bone_weights);   // Offset into this pipe to get to the data we want (this resolves to 0)
    desc[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;   // What is this data for (per vertex data is the only one we'll be dealing with in this class)
    desc[7].InstanceDataStepRate = 0U;             // If this were instance data - how often do we step it (0 for vertex data)

    ID3D11InputLayout* dx_inputlayout = nullptr;
    _dx_device->CreateInputLayout(desc, ARRAYSIZE(desc), vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), &dx_inputlayout);

    return dx_inputlayout;
}