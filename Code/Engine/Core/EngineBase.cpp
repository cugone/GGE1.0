#include "Engine/Core/EngineBase.hpp"

Window* GetWindowFromHwnd(HWND hwnd) {
    Window *wnd = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    return wnd;
}

//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK EngineMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam) {

    Window* window = GetWindowFromHwnd(windowHandle);
    if(window && window->custom_message_handler) {
        bool wasProcessed = window->custom_message_handler(windowHandle, wmMessageCode, wParam, lParam);
        if(wasProcessed) {
            return 0;
        }
    }

    switch(wmMessageCode) {
        case WM_CREATE:
        {
            CREATESTRUCT *cp = (CREATESTRUCT*)lParam;
            Window *wnd = (Window*)cp->lpCreateParams;

            wnd->SetWindowHandle(windowHandle);
            SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)wnd);

            if(wnd && wnd->custom_message_handler) {
                bool wasProcessed = wnd->custom_message_handler(windowHandle, wmMessageCode, wParam, lParam);
                if(wasProcessed) {
                    return 0;
                }
            }
        }

        case WM_PAINT:
        {
            // Ignore WM_PAINT messages
            // [c4] Must handle them otherwise they just keep coming
            PAINTSTRUCT ps;
            ::BeginPaint(windowHandle, &ps);
            ::EndPaint(windowHandle, &ps);
            return 1;
        }
        default:
        /* DO NOTHING */;
    }

    return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

