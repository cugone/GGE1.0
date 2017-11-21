#include "Engine/Display.hpp"

std::vector<HMONITOR> Display::_monitors{};
std::vector<MONITORINFO> Display::_monitorInfos{};

BOOL CALLBACK Display::_monitorEnumerator(HMONITOR, HDC, LPRECT, LPARAM) {
    return false;
}

Display::Display() {
    /* DO NOTHING */
}

Display::~Display() {
    /* DO NOTHING */
}

IntVector2 Display::GetDesktopBounds() {

    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect(desktopWindowHandle, &desktopRect);

    return IntVector2(desktopRect.right, desktopRect.bottom);
}

int Display::GetDisplayCount() {
    int count = 0;
    DISPLAY_DEVICE d;
    d.cb = sizeof(DISPLAY_DEVICE);
    while(::EnumDisplayDevices(nullptr, count, &d, EDD_GET_DEVICE_INTERFACE_NAME) != 0) {
        ++count;
    }
    return count;
}