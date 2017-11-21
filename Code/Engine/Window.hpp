#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <functional>
#include <string>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/RHI/RHITypes.hpp"

class Window {
public:

    Window();
    ~Window();

    void Open();
    bool IsOpen();

    void Close();
    bool IsClosed();

    void Show();
    void Hide();
    void Unhide();

    void SetForegroundWindow();
    void SetFocus();

    IntVector2 GetClientSize() const;

    IntVector2 GetClientPosition();
    IntVector2 GetClientDimensions();

    void SetClientSize(const IntVector2& clientPosition, const IntVector2& clientSize);

    void SetWindowSize(const IntVector2& windowSize);
    void SetWindowPosition(const IntVector2& windowPosition);

    void SetDisplayMode(const RHIOutputMode& mode);
    RHIOutputMode GetDisplayMode() const;

    void SetWindowStyle(unsigned long styleFlags = 0, unsigned long styleFlagsEx = 0);
    void SetTitle(const std::string& title);
    void ShowMenu(bool showMenu = true);

    HMENU GetMenu() const;
    void MakeMenu();
    void AppendStringToMenu(HMENU menu, unsigned int menuID, const char* str);
    void SetFullscreen(bool fullscreen);
    bool IsFullscreen() const;
    bool IsWindowed() const;

    HWND GetWindowHandle();
    void SetWindowHandle(HWND hwnd);

    void ConstrainMouseToWindow();
    void UnconstrainMouseToWindow();
    bool IsMouseConstrained() const;

    void RegisterCustomMessageHandler(std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> cb);

    IntVector2 ScreenToWindowCoords(const IntVector2& screenPos) const;

    std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> custom_message_handler;

protected:
    void RegisterWindowClass();
    void UnregisterWindowClass();
private:

    HWND _hwnd;
    UINT _msg;
    WPARAM _wparam;
    LPARAM _lparam;
    WNDCLASSEX _windowClassDescription;
    std::string _windowTitle;
    IntVector2 _clientSize;
    IntVector2 _windowSize;
    IntVector2 _windowPosition;
    unsigned long _styleFlags;
    unsigned long _styleFlagsEx;
    bool _hasMenu;
    HMENU _hmenu;
    RHIOutputMode _currentMode;
    RECT _initialClippingArea;
    bool _isClippingCursor;

    static std::size_t _refCount;
};