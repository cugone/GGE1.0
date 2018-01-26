#pragma once

#include <thread>
#include <vector>

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/ThreadSafeQueue.hpp"

class Signal;

enum JobType : unsigned int {
    JOBTYPE_GENERIC,
    JOBTYPE_MAIN,
    JOBTYPE_IO,
    JOBTYPE_RENDER,
    JOBTYPE_LOGGING,
    JOBTYPE_MAX,
};

enum JobState : unsigned int {
    JOBSTATE_NONE,
    JOBSTATE_CREATED,
    JOBSTATE_DISPATCHED,
    JOBSTATE_ENQUEUED,
    JOBSTATE_RUNNING,
    JOBSTATE_FINISHED,
    JOBSTATE_MAX,
};

//typedef void(*job_work_cb)(void*);
typedef std::function<void(void*)> job_work_cb;

class Job {
public:
	~Job();
    JobType type;
    JobState state = JOBSTATE_NONE;
    job_work_cb work_cb;
    void* user_data;

    void depends_on(Job* dependency);
    void dependent_on(Job* parent);

    std::vector<Job*> _dependents;
    unsigned int num_dependencies;

    void on_finish();
    void on_dependancy_finished();

private:
    void add_dependent(Job* dependent);

};

class JobConsumer {
public:
    JobConsumer();
    void add_category(const JobType& category);
    bool consume_job();
    unsigned int consume_all();
    void consume_for_ms(unsigned int ms);

    std::vector<ThreadSafeQueue<Job*>*> _consumables;
};

class JobSystem : public EngineSubsystem {
public:
    static void Startup(int generic_thread_count, unsigned int category_count, Signal* mainJobSignal);
    static void Shutdown();

    static void MainStep();
    static void SetCategorySignal(const JobType& category_id, Signal* signal);

    static Job* Create(const JobType& category, job_work_cb cb, void* user_data);
    static void DispatchAndRelease(Job* job);
    static void Run(const JobType& category, job_work_cb cb, void* user_data);

    static void Dispatch(Job* job);
    static bool Release(Job* job);

    static void Wait(Job* job);

    static void WaitAndRelease(Job* job);

    std::size_t GetLiveJobCount();
    std::size_t GetActiveJobCount();

    virtual void BeginFrame() override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;
public:

    JobSystem();
    ~JobSystem();

    static std::vector<ThreadSafeQueue<Job*>*> queues;
    static std::vector<Signal*> signals;
    JobConsumer* generic_consumer;
    JobConsumer* main_consumer;
    JobConsumer* io_consumer;
    Signal* mainJobSignal;
    unsigned int queue_count;
    bool is_running;
};