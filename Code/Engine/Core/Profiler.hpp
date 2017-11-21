#pragma once

#include <set>
#include <string>
#include <thread>
#include <vector>

#include "Engine/Core/EngineSubsystem.hpp"

struct profiler_node_t;
struct profiler_data_t;

class SimpleRenderer;

enum class profiler_report_type {
    FLAT,
    TREE,
};

class Profiler : public EngineSubsystem {
public:
	Profiler();
    Profiler(const char* tag_str);
	virtual ~Profiler() override;
   
    static void ProfilerSystemStartup();
    static void ProfilerSystemShutdown();

    static void ProfilerPush(const char* tag);
    static void ProfilerPop();

    static void SetRenderer(SimpleRenderer* renderer);

    static profiler_node_t* GetChildLeaf(profiler_node_t* node);
    static bool IsLeaf(profiler_node_t* node);
    static std::size_t CalculateHeight(profiler_node_t* node);
    static std::size_t CalculateSiblingCount(profiler_node_t* node);
    static std::size_t CalculateDescendantsCount(profiler_node_t* node);
    static std::size_t CalculateChildrenCount(profiler_node_t* node);

    static profiler_node_t* ProfilerGetPreviousFrame();
    static profiler_node_t* ProfilerGetPreviousFrame(const std::string& root_tag);

    static void ProfilerPause();
    static void ProfilerResume();
    static void ProfilerSnapshot();
    static void ProfilerTrackAllocation(size_t byte_size);
    static void ProfilerTrackFree(void* ptr, size_t byte_size);

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

    static void PrintReport(const profiler_report_type& type /*= profiler_report_type::FLAT*/);
    static void ToggleOpenClosed();
    static bool IsOpen();

protected:
private:
    static profiler_node_t* _activeNode;
    static std::vector<profiler_node_t*>_completedList;
    //TODO: Support multiple threads!
    static std::thread::id _mainThreadId;
    static bool _isRunning;
    static bool _isPaused;
    static bool _snapshotRequested;
    static bool _resumeRequested;
    static bool _isOpen;
    static void FreeOldTrees();
    static void AddNodeToCompletedList(profiler_node_t* node);
    static void VerifyThreadSymmetry(const char* functionName);
    void RenderProfilerGraph(SimpleRenderer* renderer) const;
    static SimpleRenderer* _renderer;
};

#if !defined PROFILE_SCOPE && !defined PROFILE_SCOPE_FUNCTION
#define PROFILE_SCOPE(tag_str) Profiler __pscope_##__LINE__##(tag_str)
#define PROFILE_SCOPE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#endif

class ProfilerReport {
public:
    ProfilerReport(profiler_node_t* frame);
    ~ProfilerReport();

    void CreateTreeView();
    void CreateFlatView();
    void SortByTotalTime();
    void Log();

protected:
private:
    static void LogHeader();
    static void LogFooter();
    static void PrintHeader();
    void PrintRow(const profiler_data_t* data);

    profiler_node_t* _currentFrame;
    std::vector<profiler_data_t*> _report;
    std::set<profiler_data_t*> _reportFlat;

    friend class Profiler;
};