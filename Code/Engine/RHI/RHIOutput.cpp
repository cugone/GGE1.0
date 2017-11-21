#include "Engine/RHI/RHIOutput.hpp"

#include "Engine/RHI/DX11.hpp"

#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Window.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Texture2D.hpp"

#include "Engine/Core/Rgba.hpp"

RHIOutput::RHIOutput(RHIDevice* parent, Window* wnd, IDXGISwapChain* dxSwapChain)
    : _parentDevice(parent)
    , _window(wnd)
    , _dx_swapchain(dxSwapChain)
    , _back_buffer(nullptr)
{
    CreateBackbuffer();
}

void RHIOutput::CreateBackbuffer() {
    if(_back_buffer != nullptr) {
        delete _back_buffer;
    }
    ID3D11Texture2D* back_buffer = nullptr;
    _dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
    _back_buffer = new Texture2D(_parentDevice, back_buffer);
}

void RHIOutput::ResetBackbuffer() {
    delete _back_buffer;
    _back_buffer = nullptr;
    CreateBackbuffer();
}
RHIOutput::~RHIOutput() {

    if(_parentDevice) {
        delete _parentDevice;
        _parentDevice = nullptr;
    }

    if(_window) {
        delete _window;
        _window = nullptr;
    }
    if(_back_buffer) {
        delete _back_buffer;
        _back_buffer = nullptr;
    }

    _dx_swapchain->Release();
    _dx_swapchain = nullptr;

}

const RHIDevice* RHIOutput::GetParentDevice() const {
    return _parentDevice;
}

RHIDevice* RHIOutput::GetParentDevice() {
    return const_cast<RHIDevice*>(static_cast<const RHIOutput&>(*this).GetParentDevice());
}

const Window* RHIOutput::GetWindow() const {
    return _window;
}

Window* RHIOutput::GetWindow() {
    return const_cast<Window*>(static_cast<const RHIOutput&>(*this).GetWindow());
}

const IDXGISwapChain* RHIOutput::GetSwapChain() const {
    return _dx_swapchain;
}

IDXGISwapChain* RHIOutput::GetSwapChain() {
    return const_cast<IDXGISwapChain*>(static_cast<const RHIOutput&>(*this).GetSwapChain());
}

Texture2D* RHIOutput::GetBackBuffer() {
    return _back_buffer;
}

IntVector2 RHIOutput::GetDimensions() const {
    if(_window) {
        return _window->GetClientSize();
    } else {
        return IntVector2::ZERO;
    }
}

float RHIOutput::GetAspectRatio() const {
    if(_window) {
        auto dims = GetDimensions();
        return dims.y / static_cast<float>(dims.x);
    }
    return 0.0f;
}

bool RHIOutput::SetDisplayMode(const RHIOutputMode& newMode) {
    _window->SetDisplayMode(newMode);
    auto width = _window->GetClientSize().x;
    auto height = _window->GetClientSize().y;
    DXGI_SWAP_CHAIN_DESC desc;
    GetSwapChain()->GetDesc(&desc);
    _parentDevice->GetImmediateContext()->Flush();
    _parentDevice->GetImmediateContext()->ClearState();
    delete _back_buffer;
    _back_buffer = nullptr;
    GetSwapChain()->ResizeBuffers(desc.BufferCount, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateBackbuffer();
    return true;
}

void RHIOutput::Present(bool vsync) {
    _dx_swapchain->Present(vsync ? 1 : 0, 0);
}