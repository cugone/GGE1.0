#pragma once

#include <cstdint>

#define MAX_FRAMES_PER_CALLSTACK (128u)
#define MAX_CALLSTACK_STR_LENGTH (2048u)
#define MAX_CALLSTACK_LINES (128u)

struct callstack_line_t
{
    char filename[MAX_CALLSTACK_STR_LENGTH];
    char function_name[MAX_CALLSTACK_STR_LENGTH];
    uint32_t line;
    uint32_t offset;
};

class Callstack
{
public:
    Callstack();

    uint32_t hash;
    uint32_t frame_count;
    void* frames[MAX_FRAMES_PER_CALLSTACK];
};

bool CallstackSystemInit();
void CallstackSystemDeinit();

// Feel free to reorganize this in a way you like - this is very C style.  
// Really, we just want to make sure these call stacks are not allocated on the heap.
// - You could creat them in-place in the meta-data if you prefer (provide memory to fill)
// - You could overload new on the Callstack class, cause "new Callstack(skip_frames)" to call that, a
//   and keeping this functionality.

// As this is - this will create a call stack using malloc (untracked allocation), skipping the first few frames.
Callstack* CreateCallstack(uint32_t skip_frames = 0);
void DestroyCallstack(Callstack*& c);

uint32_t CallstackGetLines(callstack_line_t* line_buffer, const uint32_t max_lines, const Callstack* cs);

void MakeCallstackReport(const Callstack* cs);

