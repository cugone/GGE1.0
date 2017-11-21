#include "Engine/Core/Signal.hpp"

#include <chrono>
#include <thread>

#include "Engine/EngineConfig.hpp"

Signal::Signal()
    : os_event(nullptr)
{
    os_event = ::CreateEvent(nullptr,
                             TRUE,
                             FALSE,
                             nullptr);
}

Signal::~Signal() {

    ::CloseHandle(os_event);
    os_event = nullptr;

}

void Signal::notify_all() {
    ::SetEvent(os_event);
}

void Signal::wait() {
    DWORD result = ::WaitForSingleObject(os_event, INFINITE);
    if(result == WAIT_OBJECT_0) {
        ::ResetEvent(os_event);
    }
}

bool Signal::wait_for(unsigned int ms) {
    DWORD result = ::WaitForSingleObject(os_event, ms);
    if(result == WAIT_OBJECT_0) {
        ::ResetEvent(os_event);
        return true;
    }
    return false;
}

static bool gSignalTestRunning = false;

struct signal_test_t {
    unsigned int index;
    Signal *signal;
};

//------------------------------------------------------------------------
static void SignalTestThread(void *data)
{
    signal_test_t *test = (signal_test_t*)data;

    unsigned int count = 0;
    while(gSignalTestRunning) {
        test->signal->wait();
        ++count;
        g_theFileLogger->LogPrintf("Thread[%i] was signaled [%i].\n", test->index, count);
    }

    delete test;
}

void SignalTest() {
    unsigned int num_threads = 8;
    Signal signal;

    gSignalTestRunning = true;

    for(unsigned int i = 0; i < num_threads; ++i) {
        signal_test_t *test = new signal_test_t();
        test->index = i;
        test->signal = &signal;

        // thread will handle deleting the variable.
        std::thread t(SignalTestThread, test);
        t.detach();
    }

    // give all the threads a chance to startup.
    // not guaranteed, but good enough for a test.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // now, signal them 50 times - do all go at once, or 
    // do they go one at a time?
    for(unsigned int i = 0; i < 50; ++i) {
        signal.notify_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    gSignalTestRunning = false;
    signal.notify_all();

    // not the safest - but just wait till everyone has exited.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

}