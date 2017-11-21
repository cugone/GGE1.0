#pragma once

#include "Engine/BuildConfig.cpp"

class SimpleRenderer;

#include <string>

class Callstack;
class CriticalSection;

namespace Memory {

void PrintLiveAllocationsJob(void* /*user_data*/);
void PrintLiveAllocations();

struct allocation_t {
    size_t byte_size;
    void* ptr;
    allocation_t* prev;
    allocation_t* next;
    Callstack* cs;
    size_t timeOfAllocation;
};

size_t GetAllocCount();
size_t GetAllocBytes();
size_t GetFrameAllocs();
size_t GetFrameFrees();
size_t GetPrevFrameAllocs();
size_t GetPrevFrameFrees();
size_t GetAllocHighWater();
allocation_t* GetAllocationList();
void TickMemoryProfiler();

std::string GetFriendlyByteString(const std::size_t& bytes);

void PrintBasicMemoryProfile(SimpleRenderer* renderer);
void PrintVerboseMemoryProfile(SimpleRenderer* renderer);
void PrintDisabledMemoryProfile(SimpleRenderer* renderer);
unsigned int PrintMemoryProfileData(SimpleRenderer* renderer, unsigned int line_idx);
void RenderMemoryGraph(SimpleRenderer* renderer);
CriticalSection* CreateOrGetMemoryLock();

extern size_t g_AllocCount;
extern size_t g_AllocBytes;
extern size_t g_FrameAllocs;
extern size_t g_FrameFrees;
extern size_t g_PrevFrameFrees;
extern size_t g_PrevFrameAllocs;
extern size_t g_AllocHighWater;
extern allocation_t* g_AllocationList;
extern CriticalSection* g_memorytracker_lock;

}

#ifdef TRACK_MEMORY
void* operator new(const size_t size);
void operator delete(void* ptr);
#else

#endif