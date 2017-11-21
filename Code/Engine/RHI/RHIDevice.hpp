#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/Renderer/Vertex3D.hpp"

#include <string>
#include <vector>

class RHIDeviceContext;
class RHIOutput;
class Window;
class ShaderProgram;
class ComputeShader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class ReadWriteBuffer;
class StructuredBuffer;
class DepthStencilBuffer;

struct ID3D11Device;
struct ID3D11InputLayout;

class RHIDevice {
public:
    RHIDevice(ID3D11Device* dx_device = nullptr);
	~RHIDevice();

    void SetImmediateContext(RHIDeviceContext* immediateContext);
    RHIDeviceContext* GetImmediateContext() const;
    ID3D11Device* GetDxDevice() const;

    RHIOutput* CreateOutput(Window* window, const RHIOutputMode& mode = RHIOutputMode::WINDOWED);
    RHIOutput* CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::WINDOWED);

    ComputeShader* CreateComputeShaderFromHlslFile(const std::string& filename);
    ComputeShader* CreateComputeShaderFromHlslString(const std::string& name, const std::string& shader);
    ShaderProgram* CreateShaderFromHlslFile(const std::string& filename);
    ShaderProgram* CreateShaderFromHlslString(const std::string& name, const std::string& shader);
    VertexBuffer* CreateVertexBuffer(const std::vector<Vertex3D>& buffer, const BufferUsage& usage, const BufferBindUsage& bindFlag);
    IndexBuffer* CreateIndexBuffer(const std::vector<unsigned int>& buffer, const BufferUsage& usage, const BufferBindUsage& bindFlag);
    ConstantBuffer* CreateConstantBuffer(void* buffer, unsigned int buffer_size, const BufferUsage& usage, const BufferBindUsage& bindFlag);
    ReadWriteBuffer* CreateReadWriteBuffer(const void* data, unsigned int element_size, unsigned int element_count, const BufferUsage& usage, const BufferBindUsage& bindFlag);
    StructuredBuffer* CreateStructuredBuffer(const void* data, unsigned int element_size, unsigned int element_count, const BufferUsage& usage, const BufferBindUsage& bindFlag);

    static ID3DBlob* CompileHlslToShaderBlob(char const* opt_filename,
                                             void const* source_code,
                                             size_t const source_code_size,
                                             char const* entrypoint,
                                             char const* target);

    ID3D11InputLayout* CreateInputLayout(ID3DBlob* vs_bytecode);
private:
    RHIOutput* CreateOutputFromWindow(Window* &window, const RHIOutputMode& outputMode);

    RHIDeviceContext* _immediate_context;
    ID3D11Device* _dx_device;

};