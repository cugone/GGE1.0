#include "Engine/Core/Profiler.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <string>
#include <sstream>

#include "Engine/BuildConfig.cpp"
#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Memory.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Time.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

std::thread::id Profiler::_mainThreadId = std::thread::id();
profiler_node_t* Profiler::_activeNode = nullptr;
std::vector<profiler_node_t*> Profiler::_completedList = std::vector<profiler_node_t*>();
bool Profiler::_isRunning = false;
bool Profiler::_isPaused = false;
bool Profiler::_snapshotRequested = false;
bool Profiler::_resumeRequested = false;
bool Profiler::_isOpen = false;
SimpleRenderer* Profiler::_renderer = nullptr;

Rgba GetHistoryColorFromFrameTime(double frame_time);
void DeleteTree(profiler_node_t*& head);

struct profiler_node_t {
    std::string tagName;
    double startTime;
    double endTime;
    profiler_node_t* parent;
    profiler_node_t* child;
    profiler_node_t* prev;
    profiler_node_t* next;

    profiler_node_t()
        : tagName()
        , startTime(0.0)
        , endTime(0.0)
        , parent(nullptr)
        , child(nullptr)
        , prev(nullptr)
        , next(nullptr)
    {
        /* DO NOTHING */
    }
};

struct profiler_data_t {
    std::string tagName;
    std::size_t callCount;
    double selfTime;
    double totalTime;
    double selfRatio;
    double totalRatio;

    profiler_data_t()
    : tagName()
        , callCount(0)
        , selfTime(0.0)
        , totalTime(0.0)
        , selfRatio(0.0)
        , totalRatio(0.0)
    {
        /* DO NOTHING */
    }

    bool operator<(const profiler_data_t* other) {
        return this->tagName < other->tagName;
    }

};

ProfilerReport::ProfilerReport(profiler_node_t* frame)
    : _currentFrame(frame)
    , _report()
    , _reportFlat()
{
    /* DO NOTHING */
}

ProfilerReport::~ProfilerReport() {
    for(auto& row : _report) {
        delete row;
        row = nullptr;
    }
    _report.clear();
}

void ProfilerReport::CreateTreeView() {
    auto height = Profiler::CalculateHeight(_currentFrame);
    if(height == 0) {
        return;
    }
    _report.reserve(height);
    auto head = _currentFrame;
    while(head != nullptr) {
        profiler_data_t* data = new profiler_data_t;
        data->tagName = head->tagName;
        auto cur_frame = head;
        while(cur_frame) {
            double frame_time = cur_frame->endTime - cur_frame->startTime;
            data->selfTime = frame_time;
            data->totalTime += frame_time;
            cur_frame = cur_frame->child;
        }
        data->totalRatio = (head->endTime - head->startTime) / data->totalTime;
        data->selfRatio = data->selfTime / data->totalTime;
        _report.push_back(data);
        head = head->child;
    }
}

void ProfilerReport::CreateFlatView() {
    auto head = _currentFrame;
    while(head != nullptr) {
        profiler_data_t* data = new profiler_data_t;
        data->callCount = 1;
        data->tagName = head->tagName;
        auto cur_frame = head;
        while(cur_frame) {
            double frame_time = cur_frame->endTime - cur_frame->startTime;
            data->selfTime = frame_time;
            data->totalTime += frame_time;
            cur_frame = cur_frame->child;
        }
        data->totalRatio = (head->endTime - head->startTime) / data->totalTime;
        data->selfRatio = data->selfTime / data->totalTime;
        auto inserted = _reportFlat.insert(data);
        if(inserted.second == false) {
            ++data->callCount;
        }
        head = head->child;
    }
}

void ProfilerReport::SortByTotalTime() {
    std::sort(_report.begin(), _report.end(),
    [](const profiler_data_t* const a, const profiler_data_t* const b)->bool {
        return a->totalTime < b->totalTime;
    }
    );
}

void ProfilerReport::LogHeader() {
    g_theFileLogger->LogTagf("profiler", "%s", "\n-------------------------\nSTART PROFILER REPORT\n-------------------------\n");
}
void ProfilerReport::LogFooter() {
    g_theFileLogger->LogTagf("profiler", "%s", "\n-------------------------\nEND PROFILER REPORT\n-------------------------\n");
}
void ProfilerReport::PrintHeader() {
    std::ostringstream ss;
    ss << std::left;
    ss << std::setw(60) << "TAG NAME";
    ss << std::setw(10) << "CALLS";
    ss << std::setw(10) << "TOTAL%";
    ss << std::setw(20) << "TOTAL TIME";
    ss << std::setw(10) << "SELF%";
    ss << std::setw(10) << "SELF TIME";
    ss << '\n';

    g_theFileLogger->LogTagf("profiler", "%s", ss.str().c_str());
}

void ProfilerReport::Log() {
    if(!_report.empty()) {
        for(const auto * data : _report) {
            PrintRow(data);
        }
    }
    if(!_reportFlat.empty()) {
        for(const auto * data : _reportFlat) {
            PrintRow(data);
        }
    }
}

void ProfilerReport::PrintRow(const profiler_data_t* data) {
    std::ostringstream ss;
    ss << std::left;
    ss << std::setw(60) << data->tagName;
    ss << std::setw(10) << data->callCount;
    ss << std::setw(10) << std::fixed << std::setprecision(2) << data->totalRatio * 100.0;
    ss << std::dec;
    ss << std::setw(1)  << "%";
    ss << std::setw(20) << data->totalTime;
    ss << std::setw(1)  << std::fixed << std::setprecision(2) << data->selfRatio * 100.0;
    ss << std::dec;
    ss << std::setw(1)  << "%";
    ss << std::setw(10) << data->selfTime << '\n';

    g_theFileLogger->LogTagf("profiler", "%s", ss.str().c_str());
}

Profiler::Profiler() {
    /* DO NOTHING */
}

Profiler::Profiler(const char* tag_str) {
    ProfilerPush(tag_str);
}
Profiler::~Profiler() {
    while(_activeNode) {
        ProfilerPop();
    }
}

#ifdef PROFILE_BUILD

//Deletes a tree and sets the head to nullptr at the call site.
void DeleteTree(profiler_node_t*& head) {
    if(head == nullptr) return;
    if(head->child) {
        DeleteTree(head->child);
        head->child = nullptr;
    }
    profiler_node_t* last_node = head->prev;
    if(last_node && last_node->child) {
        DeleteTree(last_node->child);
        last_node->child = nullptr;
    }
    while(last_node && last_node->child == nullptr) {
        auto cur_node = last_node;
        last_node = last_node->prev;
        if(last_node == cur_node) {
            break;
        }
        delete cur_node;
        cur_node = nullptr;
    }
    delete head;
}

void Profiler::Initialize() {
    g_theConsole->RegisterCommand("profiler_pause",
                                  [&](const std::string& /*args*/) { Profiler::ProfilerPause(); }
    , "Pauses the profiler.");

    g_theConsole->RegisterCommand("profiler_resume",
                                  [&](const std::string& /*args*/) { Profiler::ProfilerResume(); }
    , "Resumes the profiler.");

    g_theConsole->RegisterCommand("profiler_snapshot",
                                  [&](const std::string& /*args*/) { Profiler::ProfilerSnapshot(); }
    , "Profiles one frame.");

    g_theConsole->RegisterCommand("profiler_report",
    [&](const std::string& args) {
        Arguments arg_set(args);

        profiler_report_type type = profiler_report_type::FLAT;
        std::string type_str;
        if(arg_set.GetNext(type_str)) {
            if(type_str == "flat") {
                type = profiler_report_type::FLAT;
            } else if(type_str == "tree") {
                type = profiler_report_type::TREE;
            }
        }

        Profiler::PrintReport(type);
    }
    , "Prints a profile report to the log file.");

}

void Profiler::BeginFrame() {
    /* DO NOTHING */
}

void Profiler::Render() const {

    if(!IsOpen()) {
        return;
    }

#ifdef TRACK_MEMORY
    Memory::RenderMemoryGraph(_renderer);
    RenderProfilerGraph(_renderer);
#if TRACK_MEMORY == TRACK_MEMORY_BASIC
    Memory::PrintBasicMemoryProfile(_renderer);
#elif TRACK_MEMORY == TRACK_MEMORY_VERBOSE
    Memory::PrintVerboseMemoryProfile(_renderer);
#endif
#else //Never gets called!
    Memory::PrintDisabledMemoryProfile(_renderer);
#endif

}

void Profiler::RenderProfilerGraph(SimpleRenderer* renderer) const {

    const auto& window_dimensions = renderer->_rhi_output->GetDimensions();
    float window_height = static_cast<float>(window_dimensions.y);
    float window_width = static_cast<float>(window_dimensions.x);

    float graph_bg_ypadding = window_height * 0.01f;
    float graph_bg_xpadding = window_width * 0.01f;
    float graph_bg_width = window_width * 0.50f;
    float graph_bg_left = window_width - graph_bg_width - graph_bg_xpadding;
    float graph_bg_right = window_width - graph_bg_xpadding;
    float graph_bg_height = window_height * 0.25f;
    float graph_bg_top = 3.0f * graph_bg_ypadding + graph_bg_height;
    float graph_bg_bottom = graph_bg_top + graph_bg_height;
    AABB2 graph_bg(Vector2(graph_bg_left, graph_bg_top), Vector2(graph_bg_right, graph_bg_bottom));

    renderer->DrawDebugBox2D(graph_bg, 2.0f, Rgba::TURQUOISE, Rgba::WHITE);

    //Draw profile on graph
    if(_completedList.empty()) {
        return;
    }

    std::size_t history_count = (std::min)(_completedList.size(), MAX_PROFILE_HISTORY);

    float history_width = graph_bg_width / history_count;
    float history_bottom = graph_bg_bottom;

    //Find max elem by frame time
    auto max_iter = std::max_element(_completedList.begin(), _completedList.end(),
                     [&](const profiler_node_t* a, const profiler_node_t* b) {
                        return (a->endTime - a->startTime) < (b->endTime - b->startTime);
                     }
                    );

    double max_time = (*max_iter)->endTime - (*max_iter)->startTime;

    for(auto i = 0u; i < history_count; ++i) {
        auto curFrame = _completedList[i];
        if(!curFrame) {
            continue;
        }
        double frame_time = _completedList[i]->endTime - _completedList[i]->startTime;
        float history_height = MathUtils::RangeMap((float)frame_time, 0.0f, (float)max_time, 0.0f, (float)graph_bg_height);
        float history_right = graph_bg_right - i * history_width;
        float history_left = history_right - history_width;
        float history_top = history_bottom - history_height;
        AABB2 history_box(Vector2(history_left, history_top), Vector2(history_right, history_bottom));
        Rgba history_color = GetHistoryColorFromFrameTime(frame_time);
        renderer->DrawDebugBox2D(history_box, 0.0f, history_color, history_color);
    }

}

Rgba GetHistoryColorFromFrameTime(double frame_time) {
    double fps = 1.0 / frame_time;
    if(fps >= 60.0) {
        return Rgba::FORESTGREEN;
    } else if(fps > 30.0) {
        return Rgba::YELLOW;
    } else if(fps > 15.0) {
        return Rgba::ORANGE;
    } else {
        return Rgba::RED;
    }
}
void Profiler::EndFrame() {
    /* DO NOTHING */
}

bool Profiler::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}

void Profiler::PrintReport(const profiler_report_type& type /*= profiler_report_type::FLAT*/) {
    ProfilerReport::LogHeader();
    ProfilerReport::PrintHeader();

    //for(auto frame : _completedList) {
        profiler_node_t* frame = Profiler::ProfilerGetPreviousFrame();
        if(!frame) {
            g_theFileLogger->LogTagf("profiler", "%s", "Bad Frame Request");
        } else {
            ProfilerReport report(frame);

            switch(type) {
                case profiler_report_type::TREE:
                    report.CreateTreeView();
                    break;
                case profiler_report_type::FLAT:
                    report.CreateFlatView();
                    break;
                default:
                    /* DO NOTHING */;
            }
            report.SortByTotalTime();
            report.Log();
        }
    //}
    ProfilerReport::LogFooter();
}

void Profiler::ToggleOpenClosed() {
    _isOpen = !_isOpen;
}
bool Profiler::IsOpen() {
    return _isOpen;
}
void Profiler::ProfilerSystemStartup() {

    g_theProfiler = new Profiler();

    _mainThreadId = std::this_thread::get_id();

    _renderer = nullptr;
    _isOpen = false;
    _activeNode = nullptr;
    _completedList.reserve(MAX_PROFILE_HISTORY);
    _isRunning = true;
    _isPaused = false;
    _snapshotRequested = false;
    _resumeRequested = false;

}
void Profiler::ProfilerSystemShutdown() {

    for(auto& tree_node : _completedList) {
        if(!tree_node) {
            continue;
        }
        DeleteTree(tree_node);
        tree_node = nullptr;
    }
    _completedList.clear();

    _mainThreadId = std::thread::id();
    _renderer = nullptr;
    _activeNode = nullptr;
    _isRunning = false;
    _isPaused = false;
    _snapshotRequested = false;
    _resumeRequested = false;

    delete g_theProfiler;
    g_theProfiler = nullptr;

}

void Profiler::VerifyThreadSymmetry(const char* functionName) {
    ASSERT_OR_DIE(functionName != nullptr, "Must provide function name!");
    ASSERT_OR_DIE(std::this_thread::get_id() == _mainThreadId, functionName);
}

void Profiler::FreeOldTrees() {
    VerifyThreadSymmetry(__FUNCTION__);
    if(_completedList.size() >= MAX_PROFILE_TREES) {
        auto last_iter = _completedList.end() - MAX_PROFILE_TREES;
        for(auto iter = _completedList.begin(); iter != last_iter; ++iter) {
            DeleteTree(*iter);
            (*iter) = nullptr;
        }
        auto one_past_last_iter = last_iter != _completedList.end() ? last_iter + 1 : _completedList.end();
        _completedList.erase(std::remove(_completedList.begin(), one_past_last_iter, nullptr), one_past_last_iter);
    }
}

void Profiler::ProfilerPush(const char* tag) {
    VerifyThreadSymmetry(__FUNCTION__);
    if(_isPaused) {
        if(_resumeRequested && _activeNode == nullptr) {
            _isPaused = false;
            _snapshotRequested = false;
            _resumeRequested = false;
        }
        if(!_snapshotRequested) {
            return;
        }
    }

    if(_snapshotRequested) {
        _snapshotRequested = false;
    }

    profiler_node_t* node = new profiler_node_t;

    node->tagName = tag;
    node->startTime = GetCurrentTimeSeconds();
    node->parent = _activeNode;

    if(_activeNode) {
        if(_activeNode->child) {
            _activeNode->child->prev->next = node;
            node->prev = _activeNode->child->prev;
            _activeNode->child->prev = node;
        } else {
            _activeNode->child = node;
            _activeNode->child->prev = _activeNode->child;
            _activeNode->child->next = _activeNode->child;
        }
    }
    _activeNode = node;

}

void Profiler::ProfilerPop() {
    VerifyThreadSymmetry(__FUNCTION__);
    ASSERT_OR_DIE(_activeNode != nullptr, "Popping an empty profile tree.");

    _activeNode->endTime = GetCurrentTimeSeconds();

    if(_activeNode->parent) {
        _activeNode = _activeNode->parent;
    } else {
        AddNodeToCompletedList(_activeNode);
        _activeNode = nullptr;
        FreeOldTrees();
    }
    
}

void Profiler::SetRenderer(SimpleRenderer* renderer) {
    _renderer = renderer;
}
profiler_node_t* Profiler::GetChildLeaf(profiler_node_t* node) {
    VerifyThreadSymmetry(__FUNCTION__);
    if(node == nullptr) {
        return nullptr;
    }
    if(IsLeaf(node)) {
        return node;
    }
    return GetChildLeaf(node->child);
}

bool Profiler::IsLeaf(profiler_node_t* node) {
    if(node == nullptr) {
        return false;
    }
    return !node->child;
}
std::size_t Profiler::CalculateHeight(profiler_node_t* node) {
    VerifyThreadSymmetry(__FUNCTION__);
    if(node == nullptr) {
        return 0;
    }
    return 1 + CalculateHeight(node->child);
}

std::size_t Profiler::CalculateSiblingCount(profiler_node_t* node) {
    VerifyThreadSymmetry(__FUNCTION__);
    if(!(node && node->next)) {
        return 0;
    }
    return 1 + CalculateSiblingCount(node->next);
}
std::size_t Profiler::CalculateDescendantsCount(profiler_node_t* node) {
    VerifyThreadSymmetry(__FUNCTION__);
    unsigned long node_count = 0;
    auto cur = node;
    while(cur) {
        node_count = 1 + CalculateSiblingCount(cur);
        cur = cur->child;
    }
    return node_count;
}
std::size_t Profiler::CalculateChildrenCount(profiler_node_t* node) {
    if(!node) {
        return 0;
    }
    return 1 + CalculateSiblingCount(node->child);
}
profiler_node_t* Profiler::ProfilerGetPreviousFrame() {
    VerifyThreadSymmetry(__FUNCTION__);
    if(_completedList.empty()) {
        return nullptr;
    }
    return _completedList.back();
}
profiler_node_t* Profiler::ProfilerGetPreviousFrame(const std::string& root_tag) {
    VerifyThreadSymmetry(__FUNCTION__);
    auto last_with_tag = std::find_if(_completedList.rbegin(), _completedList.rend(),
                                      [&](profiler_node_t* node) { return node->tagName == root_tag; });
    if(last_with_tag == _completedList.rend()) {
        return nullptr;
    }
    return *last_with_tag;
}

void Profiler::ProfilerPause() {
    VerifyThreadSymmetry(__FUNCTION__);
    _isPaused = true;
}
void Profiler::ProfilerResume() {
    VerifyThreadSymmetry(__FUNCTION__);
    if(_activeNode == nullptr) {
        _isPaused = false;
    }
}
void Profiler::ProfilerSnapshot() {
    if(!_isPaused) {
        g_theConsole->NotifyMsg("Can not take snapshot when Profiler is running.");
    }
    VerifyThreadSymmetry(__FUNCTION__);
}
void Profiler::ProfilerTrackAllocation(size_t /*byte_size*/) {
    VerifyThreadSymmetry(__FUNCTION__);
}
void Profiler::ProfilerTrackFree(void* /*ptr*/, size_t /*byte_size*/) {
    VerifyThreadSymmetry(__FUNCTION__);
}

void Profiler::AddNodeToCompletedList(profiler_node_t* node) {
    VerifyThreadSymmetry(__FUNCTION__);
    _completedList.push_back(node);
}

#else
// No-op if no opt-in of PROFILE_BUILD

void Profiler::ProfilerSystemStartup() {

    g_theProfiler = new Profiler();
    _isOpen = false;

}
void Profiler::ProfilerSystemShutdown() {
    delete g_theProfiler;
    g_theProfiler = nullptr;
}
void Profiler::SetRenderer(SimpleRenderer* renderer) { _renderer = renderer; }
void Profiler::ProfilerPush(const char* /*tag*/) { /* DO NOTHING */ }
void Profiler::ProfilerPop() { /* DO NOTHING */ }
profiler_node_t* Profiler::GetChildLeaf(profiler_node_t* /*node*/) { return nullptr; }
std::size_t Profiler::CalculateHeight(profiler_node_t* /*node*/) { return 0; }
bool Profiler::IsLeaf(profiler_node_t* /*node*/) { return false; }
bool Profiler::IsOpen() { return _isOpen; }
profiler_node_t* Profiler::ProfilerGetPreviousFrame() { return nullptr; }
profiler_node_t* Profiler::ProfilerGetPreviousFrame(const std::string& /*root_tag*/ /*= nullptr*/) { return nullptr; }
void Profiler::ProfilerPause() { /* DO NOTHING */ }
void Profiler::ProfilerResume() { /* DO NOTHING */ }
void Profiler::ProfilerSnapshot() { /* DO NOTHING */ }
void Profiler::ProfilerTrackAllocation(size_t /*byte_size*/) { /* DO NOTHING */ }
void Profiler::ProfilerTrackFree(void* /*ptr*/, size_t /*byte_size*/) { /* DO NOTHING */ }
void Profiler::AddNodeToCompletedList(profiler_node_t* /*node*/) { /* DO NOTHING */ }
void Profiler::VerifyThreadSymmetry(const char* /*functionName*/) { /* DO NOTHING */ }
void Profiler::Initialize() { /* DO NOTHING */ }
void Profiler::BeginFrame() { /* DO NOTHING */ }
void Profiler::Render() const {
    if(IsOpen()) {
        Memory::PrintDisabledMemoryProfile(_renderer);
    }
}
void Profiler::EndFrame() { /* DO NOTHING */ }
bool Profiler::ProcessSystemMessage(const SystemMessage& /*msg*/) { return false; }
void Profiler::FreeOldTrees() { /* DO NOTHING */ }
void Profiler::RenderProfilerGraph(SimpleRenderer* /*renderer*/) const { /* DO NOTHING */ }
void DeleteTree(profiler_node_t*& /*head*/) { /* DO NOTHING */ }
void Profiler::PrintReport(const profiler_report_type& /*type*/) { /* DO NOTHING */ }
void Profiler::ToggleOpenClosed() {
    _isOpen = !_isOpen;
}
#endif