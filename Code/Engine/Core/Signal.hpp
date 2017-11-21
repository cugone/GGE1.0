#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Signal {
public:
	Signal();
	~Signal();

    void notify_all();
    void wait();
    bool wait_for(unsigned int ms);

protected:
private:
    HANDLE os_event;
};

void SignalTest();
