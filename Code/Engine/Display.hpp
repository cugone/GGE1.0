#pragma once

#include "Engine/Math/IntVector2.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <functional>

class Display {
public:
	Display();
	~Display();

    static IntVector2 GetDesktopBounds();
    static int GetDisplayCount();

protected:
    BOOL CALLBACK _monitorEnumerator(HMONITOR, HDC, LPRECT, LPARAM);
private:
    static std::vector<HMONITOR> _monitors;
    static std::vector<MONITORINFO> _monitorInfos;
};