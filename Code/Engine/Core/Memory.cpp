#include "Engine/Core/Memory.hpp"

#include "Engine/BuildConfig.cpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <new>
#include <numeric>
#include <sstream>
#include <type_traits>
#include <vector>

#include "Engine/BuildConfig.cpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Core/CallStack.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/CriticalSection.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"

#include "Engine/EngineConfig.hpp"

#include "Engine/Math/AABB2.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

size_t Memory::g_AllocCount = 0;
size_t Memory::g_AllocBytes = 0;
size_t Memory::g_FrameAllocs = 0;
size_t Memory::g_FrameFrees = 0;
size_t Memory::g_PrevFrameAllocs = 0;
size_t Memory::g_PrevFrameFrees = 0;
size_t Memory::g_AllocHighWater = 0;
Memory::allocation_t* Memory::g_AllocationList = nullptr;
CriticalSection* Memory::g_memorytracker_lock = nullptr;

size_t Memory::GetAllocCount() {
    return g_AllocCount;
}
size_t Memory::GetAllocBytes() {
    return g_AllocBytes;
}

size_t Memory::GetFrameAllocs() {
    return g_FrameAllocs;
}
size_t Memory::GetFrameFrees() {
    return g_FrameFrees;
}
size_t Memory::GetPrevFrameAllocs() {
    return g_PrevFrameAllocs;
}
size_t Memory::GetPrevFrameFrees() {
    return g_PrevFrameFrees;
}
size_t Memory::GetAllocHighWater() {
    return g_AllocHighWater;
}
Memory::allocation_t* Memory::GetAllocationList() {
    return g_AllocationList;
}

void Memory::TickMemoryProfiler() {
#ifdef TRACK_MEMORY
    g_PrevFrameFrees = g_FrameFrees;
    g_PrevFrameAllocs = g_FrameAllocs;
    g_FrameFrees = 0;
    g_FrameAllocs = 0;
#endif
}

std::string Memory::GetFriendlyByteString(const std::size_t& bytes) {

    const long double maxBytesAsKiB = MathUtils::ConvertKiBToBytes(2.0f);
    const long double maxBytesAsMiB = MathUtils::ConvertMiBToBytes(2.0f);
    const long double maxBytesAsGiB = MathUtils::ConvertGiBToBytes(2.0f);

    const char* measurement = "GB";
    bool useGB = bytes >= maxBytesAsGiB;
    bool useMB = bytes < maxBytesAsGiB;
    bool useKB = bytes < maxBytesAsMiB;

    useGB = true;
    useMB = false;
    useKB = false;
    if(bytes < maxBytesAsGiB) {
        measurement = "MB";
        useGB = false;
        useMB = true;
        useKB = false;
    }
    if(bytes < maxBytesAsMiB) {
        measurement = "KB";
        useGB = false;
        useMB = false;
        useKB = true;
    }
    if(bytes < maxBytesAsKiB) {
        measurement = " B";
        useGB = false;
        useMB = false;
        useKB = false;
    }
    return Stringf("%.2Lf %s", (useGB ? MathUtils::ConvertBytesToGiB(bytes)
                             : (useMB ? MathUtils::ConvertBytesToMiB(bytes)
                             : (useKB ? MathUtils::ConvertBytesToKiB(bytes)
                             : (bytes))))
                   , measurement);
}

unsigned int Memory::PrintMemoryProfileData(SimpleRenderer* renderer, unsigned int line_idx) {

    unsigned int tab_count = 0;
    std::string tab_str(++tab_count * 2, ' ');

    std::string allocCount_str = std::to_string(GetAllocCount());
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("Total Allocations: ") + allocCount_str, Rgba::WHITE, static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth(tab_str)), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));

    std::string friendlyBytes_str = GetFriendlyByteString(GetAllocBytes());
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("Bytes Allocated: ") + friendlyBytes_str, Rgba::WHITE, static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth(tab_str)), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));

    std::string allocHW_str = GetFriendlyByteString(GetAllocHighWater());
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("Peak Bytes Allocated: ") + allocHW_str, Rgba::WHITE, static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth(tab_str)), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));

    std::string allocFrameAlloc_str = std::to_string(GetPrevFrameAllocs());
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("Allocations last frame: ") + allocFrameAlloc_str, Rgba::WHITE, static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth(tab_str)), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));

    std::string allocFrameFree_str = std::to_string(GetPrevFrameFrees());
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("Frees last frame: ") + allocFrameFree_str, Rgba::WHITE, static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth(tab_str)), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));
    return line_idx;
}

void Memory::PrintLiveAllocationsJob(void* /*user_data*/) {
    PrintLiveAllocations();
}

void Memory::PrintLiveAllocations() {
#ifdef TRACK_MEMORY
    g_theFileLogger->Lock();
    CreateOrGetMemoryLock()->enter();
    auto allocation = GetAllocationList();
    if(!allocation) {
        g_theFileLogger->LogTagf("memory", "\n-------------------------\nSTART LIVE ALLOCATION LOG\n-------------------------\n");
        g_theFileLogger->LogTagf("memory", "\nNO LIVE ALLOCATIONS\n");
        g_theFileLogger->LogTagf("memory", "\n-----------------------\nEND LIVE ALLOCATION LOG\n-----------------------\n");
        CreateOrGetMemoryLock()->leave();
        g_theFileLogger->Unlock();
        return;
    }

    g_theFileLogger->LogTagf("memory", "\n-------------------------\nSTART LIVE ALLOCATION LOG\n-------------------------\n");

    auto count = GetAllocCount() + 1;
    std::vector<allocation_t*> allocationReport(count, nullptr);
    {
        allocation_t* cur_alloc = GetAllocationList();
        for(auto & i : allocationReport) {
            i = cur_alloc;
            cur_alloc = cur_alloc->next;
        }
        cur_alloc = nullptr;
    }

    std::sort(allocationReport.begin(), allocationReport.end(),
    [&](const allocation_t* a, const allocation_t* b) {
            return a->byte_size > b->byte_size;
    });

    std::vector<std::string> callstackStrings;
    {
        for(auto & i : allocationReport) {

            std::string group_byte_str = GetFriendlyByteString(i->byte_size);

            char line_header_buffer[MAX_CALLSTACK_STR_LENGTH];
            sprintf_s(line_header_buffer, MAX_CALLSTACK_STR_LENGTH, "%.2fs: Bytes leaked in callstack: %s\n", static_cast<float>(i->timeOfAllocation), group_byte_str.c_str());
            callstackStrings.emplace_back(line_header_buffer);
            // Printing a call stack, happens when making report
            char line_buffer[MAX_CALLSTACK_STR_LENGTH];
            callstack_line_t lines[MAX_CALLSTACK_LINES];
            uint32_t line_count = CallstackGetLines(lines, MAX_CALLSTACK_LINES, i->cs);
            for(uint32_t j = 0; j < line_count; ++j) {
                // this specific format will make it double click-able in an output window 
                // taking you to the offending line.
                sprintf_s(line_buffer, MAX_CALLSTACK_STR_LENGTH, "\t%s(%u): %s\n",
                          lines[j].filename, lines[j].line, lines[j].function_name);
                callstackStrings.emplace_back(line_buffer);
            }
        }
    }

    std::size_t total_bytes = 0;
    for(auto & iter : allocationReport) {
        total_bytes += iter->byte_size;
    }

    CreateOrGetMemoryLock()->leave();
    g_theFileLogger->Unlock();

    std::string total_byte_str = GetFriendlyByteString(total_bytes);
    g_theFileLogger->LogTagf("memory", "%u leaked allocations.\tTotal: %s\n", count, total_byte_str.c_str());


    for(auto& callstackString : callstackStrings) {
        g_theFileLogger->LogTagf("memory", callstackString.c_str());
    }

    g_theFileLogger->LogTagf("memory", "\n-----------------------\nEND LIVE ALLOCATION LOG\n-----------------------\n");
#endif
}
void Memory::PrintBasicMemoryProfile(SimpleRenderer* renderer) {
    unsigned int line_idx = 1;
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("MEMORY"), Rgba::WHITE, 0.0f, line_idx * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));
    //Print [BASIC] on same line
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("[BASIC]"), Rgba::YELLOW, line_idx * static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth("MEMORY  ")), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));
    line_idx = PrintMemoryProfileData(renderer, line_idx);
}

void Memory::RenderMemoryGraph(SimpleRenderer* renderer) {

    const auto& window_dimensions = renderer->_rhi_output->GetDimensions();
    float window_height = static_cast<float>(window_dimensions.y);
    float window_width = static_cast<float>(window_dimensions.x);

    float graph_bg_ypadding = window_height * 0.01f;
    float graph_bg_xpadding = window_width * 0.01f;
    float graph_bg_width = window_width * 0.50f;
    float graph_bg_left = window_width - graph_bg_width - graph_bg_xpadding;
    float graph_bg_right = window_width - graph_bg_xpadding;
    float graph_bg_height = window_height * 0.25f;
    float graph_bg_top = graph_bg_ypadding;
    float graph_bg_bottom = graph_bg_ypadding + graph_bg_top + graph_bg_height;
    AABB2 graph_bg(Vector2(graph_bg_left, graph_bg_top), Vector2(graph_bg_right, graph_bg_bottom));
    renderer->DrawDebugBox2D(graph_bg, 1.0f, Rgba::CYAN, Rgba::WHITE);

    //Draw memory usage on graph
    auto allocationCount = g_AllocCount;

    float history_width = graph_bg_width / static_cast<float>(MAX_PROFILE_HISTORY);
    float history_bottom = graph_bg_bottom;

    std::size_t history_count = (std::min)(allocationCount, MAX_PROFILE_HISTORY);

    for(auto i = 0u; i < history_count; ++i) {
        float history_height = MathUtils::RangeMap((float)allocationCount, 0.0f, (float)g_AllocHighWater, 0.0f, (float)graph_bg_height);
        float history_top = history_bottom - history_height;
        float history_right = graph_bg_right - i * history_width;
        float history_left = history_right - history_width;
        AABB2 history_box(Vector2(history_left, history_top), Vector2(history_right, history_bottom));
        renderer->DrawDebugBox2D(history_box, 1.0f, Rgba::RED, Rgba::RED);
    }

    //Draw high water mark
    float highWaterLineStartOffset = MathUtils::RangeMap((float)allocationCount, 0.0f, (float)g_AllocHighWater, 0.0f, (float)graph_bg_height);
    float highWaterLineStartY = graph_bg_top + highWaterLineStartOffset;
    Vector2 highWaterLineStart = Vector2(graph_bg_left, highWaterLineStartY);
    Vector2 highWaterLineEnd = Vector2(graph_bg_right, highWaterLineStartY);
    AABB2 highWater_box(highWaterLineStart, highWaterLineEnd);
    renderer->DrawDebugBox2D(highWater_box, 2.0f, Rgba::FORESTGREEN, Rgba::FORESTGREEN);

}

void Memory::PrintVerboseMemoryProfile(SimpleRenderer* renderer) {
    unsigned int line_idx = 1;
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("MEMORY"), Rgba::WHITE, 0.0f, line_idx * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));
    //Print [VERBOSE] on same line
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("[VERBOSE]"), Rgba::YELLOW, line_idx * static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth("MEMORY  ")), line_idx++ * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));

    line_idx = PrintMemoryProfileData(renderer, line_idx);
}

void Memory::PrintDisabledMemoryProfile(SimpleRenderer* renderer) {
    unsigned int line_idx = 1;
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("MEMORY"), Rgba::WHITE, 0.0f, line_idx * static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));
    renderer->DrawTextLine(g_theConsole->GetFont(), std::string("[DISABLED]"), Rgba::YELLOW, line_idx++ * static_cast<float>(g_theConsole->GetFont()->CalculateTextWidth("MEMORY  ")), static_cast<float>(g_theConsole->GetFont()->GetLineHeight()));

}

#ifdef TRACK_MEMORY
CriticalSection* Memory::CreateOrGetMemoryLock() {
    if(Memory::g_memorytracker_lock == nullptr) {
        Memory::g_memorytracker_lock = reinterpret_cast<CriticalSection*>(std::malloc(sizeof(CriticalSection)));
        Memory::g_memorytracker_lock = new (Memory::g_memorytracker_lock)CriticalSection;
    }
    return Memory::g_memorytracker_lock;
}

void* operator new(const size_t size) {
    size_t alloc_size = size + sizeof(Memory::allocation_t);
    Memory::allocation_t* ptr = (Memory::allocation_t*)std::malloc(alloc_size);
    ptr->byte_size = size;
    ptr->ptr = ptr + 1;
    ptr->next = nullptr;
    ptr->prev = nullptr;
    ptr->cs = CreateCallstack();
    ptr->timeOfAllocation = static_cast<size_t>(GetCurrentTimeSeconds());
    Memory::CreateOrGetMemoryLock()->enter();

    ++Memory::g_AllocCount;
    ++Memory::g_FrameAllocs;
    Memory::g_AllocBytes += size;
    Memory::g_AllocHighWater = (std::max)(Memory::g_AllocHighWater, Memory::g_AllocBytes);

    if(Memory::g_AllocationList) {
        Memory::g_AllocationList->prev = ptr;
        ptr->next = Memory::g_AllocationList;
        ptr->prev = nullptr;
        Memory::g_AllocationList = ptr;
    } else {
        Memory::g_AllocationList = ptr;
    }
    Memory::CreateOrGetMemoryLock()->leave();
    return ptr + 1;
}

void operator delete(void* ptr) {
    if(ptr == nullptr) {
        return;
    }

    Memory::allocation_t* size_ptr = (Memory::allocation_t*)ptr;
    --size_ptr;

    Memory::CreateOrGetMemoryLock()->enter();

    --Memory::g_AllocCount;
    ++Memory::g_FrameFrees;

    Memory::g_AllocBytes -= size_ptr->byte_size;

    if(!(size_ptr->next || size_ptr->prev)) {
        Memory::g_AllocationList = nullptr;
    } else {
        if(!size_ptr->prev) {
            Memory::g_AllocationList = size_ptr->next;
        } else {
            size_ptr->prev->next = size_ptr->next;
        }
        if(!size_ptr->next) {
            size_ptr->prev->next = nullptr;
        } else {
            size_ptr->next->prev = size_ptr->prev;
        }
    }
    DestroyCallstack(size_ptr->cs);
    size_ptr->cs = nullptr;
    std::free(size_ptr);

    Memory::CreateOrGetMemoryLock()->leave();
}

#endif