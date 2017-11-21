#include "Engine/RHI/RHIInstance.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/EngineConfig.hpp"

#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Window.hpp"

RHIInstance* RHIInstance::_instance = nullptr;
IDXGIDebug* RHIInstance::_debuggerInstance = nullptr;

RHIInstance* RHIInstance::CreateInstance() {
    if(_instance) {
        return _instance;
    }
    _instance = new RHIInstance();
    _instance->_debuggerInstance = nullptr;

#if defined(RENDER_DEBUG)

    HMODULE debug_module = nullptr;

    // Debug Setup

    debug_module = ::LoadLibraryA("Dxgidebug.dll");

    using GetDebugModuleCB = HRESULT(WINAPI *)(REFIID, void**);

    GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress(debug_module, "DXGIGetDebugInterface");

    HRESULT hr = cb(__uuidof(IDXGIDebug), (void**)&_instance->_debuggerInstance);

    bool succeeded = SUCCEEDED(hr);
    ASSERT_OR_DIE(succeeded, "DXGIDugger failed to initialize.");

    _instance->_debuggerInstance->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);

#endif
    return _instance;
}

void RHIInstance::DestroyInstance() {
    if(_instance) {
        delete _instance;
        _instance = nullptr;
    }
}

RHIOutput* RHIInstance::CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition /*= IntVector2::ZERO*/, const RHIOutputMode& outputMode /*= RHIOutputMode::WINDOWED*/) {
    unsigned int device_flags = 0U;
#ifdef RENDER_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;

    // This flag fails unless we' do 11.1 (which we're not), and we query that
    // the adapter support its (which we're not).  Just here to let you know it exists.
    // device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

    Window* window = new Window;
    window->SetClientSize(clientPosition, clientSize);
    window->Open();

    ID3D11DeviceContext* dx_context;
    ID3D11Device* dx_device;
    IDXGISwapChain* dx_swapchain;


    DXGI_SWAP_CHAIN_DESC swap_desc;
    memset(&swap_desc, 0, sizeof(swap_desc));

    // fill the swap chain description struct
    swap_desc.BufferCount = 1;                                    // two buffers (one front, one back?)

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

RHIInstance::RHIInstance() {
    /* DO NOTHING */
}

RHIInstance::~RHIInstance() {
#ifdef RENDER_DEBUG
    _instance->_debuggerInstance->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
    _instance->_debuggerInstance->Release();
    _instance->_debuggerInstance = nullptr;
#endif
}