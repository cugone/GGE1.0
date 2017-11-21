#pragma once

#include "Engine/RHI/RHITypes.hpp"

class Window;
class RHIDevice;
class Texture2D;
class IntVector2;
class Rgba;

struct IDXGISwapChain;

class RHIOutput {
public:
	RHIOutput(RHIDevice* parent, Window* wnd, IDXGISwapChain* dxSwapChain);

    ~RHIOutput();

    const RHIDevice* GetParentDevice() const;
    RHIDevice* GetParentDevice();

    const Window* GetWindow() const;
    Window* GetWindow();

    const IDXGISwapChain* GetSwapChain() const;
    IDXGISwapChain* GetSwapChain();

    Texture2D* GetBackBuffer();
    IntVector2 GetDimensions() const;
    float GetAspectRatio() const;

    bool SetDisplayMode(const RHIOutputMode& newMode);

    void Present(bool vsync);

protected:
    void CreateBackbuffer();
    void ResetBackbuffer();
private:
	Window* _window;
    RHIDevice* _parentDevice;
    IDXGISwapChain* _dx_swapchain;
    Texture2D* _back_buffer;
};