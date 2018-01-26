#pragma once

using byte_t = unsigned char;

#define BIT(x) ((1) << (x))

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Window.hpp"

Window* GetWindowFromHwnd(HWND hwnd);

LRESULT CALLBACK EngineMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);