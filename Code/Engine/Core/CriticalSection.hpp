#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CriticalSection {
public:
    inline CriticalSection() { ::InitializeCriticalSection(&cs); }
    inline ~CriticalSection() { ::DeleteCriticalSection(&cs); }
    inline void enter() { ::EnterCriticalSection(&cs); }
    inline void leave() { ::LeaveCriticalSection(&cs); }
    inline bool tryenter() { return ::TryEnterCriticalSection(&cs) != 0; }
protected:
private:
    CRITICAL_SECTION cs;
};


