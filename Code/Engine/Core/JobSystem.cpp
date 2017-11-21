#include "Engine/Core/JobSystem.hpp"

#include "Engine/Core/Atomic.hpp"
#include "Engine/Core/Signal.hpp"
#include "Engine/Core/Time.hpp"

#include "Engine/EngineConfig.hpp"

static void GenericJobThread(Signal *signal) {
    JobConsumer jc;
    if(g_theJobSystem) {
        jc.add_category(JobType::JOBTYPE_GENERIC);
        JobSystem::SetCategorySignal(JobType::JOBTYPE_GENERIC, signal);
        while(g_theJobSystem && g_theJobSystem->is_running) {
            signal->wait();
            jc.consume_all();
        }
        jc.consume_all();
    }
}

void JobSystem::BeginFrame() {
    MainStep();
}

bool JobSystem::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}

std::vector<ThreadSafeQueue<Job*>*> JobSystem::queues = std::vector<ThreadSafeQueue<Job*>*>();
std::vector<Signal*> JobSystem::signals = std::vector<Signal*>();

JobSystem::JobSystem()
    : generic_consumer(nullptr)
    , main_consumer(nullptr)
    , io_consumer(nullptr)
    , queue_count(0)
    , is_running(false)
    , mainJobSignal(nullptr)
{
    /* DO NOTHING */
}

JobSystem::~JobSystem() {
    mainJobSignal = nullptr;

    delete generic_consumer;
    generic_consumer = nullptr;

    for(std::size_t i = 0; i < JOBTYPE_MAX; ++i) {
        if(signals[i] == nullptr) {
            continue;
        }
        signals[i]->notify_all();
        while(!queues[i]->empty()) {
            Job* job;
            if(queues[i]->pop(job)) {
                delete job;
                job = nullptr;
            }
        }
        delete signals[i];
        signals[i] = nullptr;
    }
    queues.clear();
    signals.clear();

    queue_count = 0;
    is_running = false;

}


void JobSystem::Startup(int generic_thread_count, unsigned int category_count, Signal* mainJobSignal) {
    int core_count = static_cast<int>(std::thread::hardware_concurrency());
    if(generic_thread_count <= 0) {
        core_count += generic_thread_count;
    }
    --core_count; // one is always being created - so subtract from total wanted;

                  // We need queues! 
    g_theJobSystem = new JobSystem();
    g_theJobSystem->mainJobSignal = mainJobSignal;
    g_theJobSystem->queues.resize(category_count);
    g_theJobSystem->signals.resize(category_count);
    g_theJobSystem->queue_count = category_count;
    g_theJobSystem->is_running = true;

    for(unsigned int i = 0; i < category_count; ++i) {
        g_theJobSystem->queues[i] = new ThreadSafeQueue<Job*>();
    }

    //Unwrap only if it becomes a problem!
    //g_theJobSystem->queues[JOBTYPE_GENERIC] = new ThreadSafeQueue<Job*>();
    //g_theJobSystem->queues[JOBTYPE_MAIN] = new ThreadSafeQueue<Job*>();
    //g_theJobSystem->queues[JOBTYPE_IO] = new ThreadSafeQueue<Job*>();
    //g_theJobSystem->queues[JOBTYPE_RENDER] = new ThreadSafeQueue<Job*>();
    //g_theJobSystem->queues[JOBTYPE_LOGGING] = new ThreadSafeQueue<Job*>();

    for(unsigned int i = 0; i < category_count; ++i) {
        g_theJobSystem->signals[i] = nullptr;
    }

    g_theJobSystem->signals[JOBTYPE_GENERIC] = new Signal();

    JobConsumer* generic_consumer = new JobConsumer();
    generic_consumer->add_category(JOBTYPE_GENERIC);
    g_theJobSystem->generic_consumer = generic_consumer;

    std::thread t_generic(GenericJobThread, g_theJobSystem->signals[JOBTYPE_GENERIC]);

    t_generic.detach();
    for(int i = 0; i < core_count; ++i) {
        std::thread t(GenericJobThread, g_theJobSystem->signals[JOBTYPE_GENERIC]);
        t.detach();
    }
}

void JobSystem::Shutdown() {
    g_theJobSystem->is_running = false;
    delete g_theJobSystem;
    g_theJobSystem = nullptr;
}

void JobSystem::MainStep() {
    JobConsumer jc;
    jc.add_category(JOBTYPE_MAIN);
    SetCategorySignal(JOBTYPE_MAIN, g_theJobSystem->mainJobSignal);
    jc.consume_all();
}

void JobSystem::SetCategorySignal(const JobType& category_id, Signal* signal) {
    signals[category_id] = signal;
}

Job* JobSystem::Create(const JobType& category, job_work_cb cb, void* user_data) {
    Job* j = new Job;
    j->type = category;
    j->state = JOBSTATE_CREATED;
    j->work_cb = cb;
    j->user_data = user_data;
    j->num_dependencies = 1;
    return std::move(j);
}

void JobSystem::DispatchAndRelease(Job* job) {
    JobSystem::Dispatch(job);
    JobSystem::Release(job);
}

void JobSystem::Run(const JobType& category, job_work_cb cb, void* user_data) {
    Job* job = Create(category, cb, user_data);
    job->state = JOBSTATE_RUNNING;
    DispatchAndRelease(job);
}

void JobSystem::Dispatch(Job* job) {
    job->state = JOBSTATE_DISPATCHED;
    AtomicIncrement(&job->num_dependencies);
    g_theJobSystem->queues[job->type]->push(job);
    Signal* signal = g_theJobSystem->signals[job->type];
    if(signal != nullptr) {
        signal->notify_all();
    }
}

bool JobSystem::Release(Job* job) {
    unsigned int dcount = AtomicDecrement(&job->num_dependencies);
    if(dcount != 0) {
        return false;
    }
    delete job;
    return true;
}

void JobSystem::Wait(Job* job) {
    while(job->state != JOBSTATE_FINISHED) {
        std::this_thread::yield();
    }
}

void JobSystem::WaitAndRelease(Job* job) {
    JobSystem::Wait(job);
    JobSystem::Release(job);
}

unsigned int JobSystem::GetLiveJobCount() {
    unsigned int count = 0;
    for(auto& i : this->queues) {
        ThreadSafeQueue<Job*>& queue = *i;
        count += queue.size();
    }
    return count;
}

unsigned int JobSystem::GetActiveJobCount() {
    return 0;
}

JobConsumer::JobConsumer()
: _consumables()
{
    /* DO NOTHING */
}

void JobConsumer::add_category(const JobType& category) {
    auto q = JobSystem::queues[category];
    if(q) {
        _consumables.push_back(q);
    }
}

bool JobConsumer::consume_job() {
    if(_consumables.empty()) {
        return false;
    }

    for(auto & _consumable : _consumables) {
        ThreadSafeQueue<Job*>& queue = *_consumable;
        Job* job = nullptr;
        if(!queue.pop(job)) {
            return false;
        }
        job->work_cb(job->user_data);
        job->on_finish();
        job->state = JOBSTATE_FINISHED;
        delete job;
    }
    return true;
}

unsigned int JobConsumer::consume_all() {
    unsigned int processed_jobs = 0;
    while(consume_job()) {
        ++processed_jobs;
    }
    return processed_jobs;
}

void JobConsumer::consume_for_ms(unsigned int ms) {
    double ms_t = static_cast<double>(ms) * 0.001;
    double start_time = GetCurrentTimeSeconds();
    double end_time = GetCurrentTimeSeconds();
    while(end_time - start_time < ms_t && consume_job()) {
        end_time = GetCurrentTimeSeconds();
    }
}

void Job::depends_on(Job* dependency) {
    this->dependent_on(dependency);
}

void Job::add_dependent(Job* dependent) {
    _dependents.push_back(dependent);
}

void Job::on_finish() {
    for(auto & _dependent : _dependents) {
        _dependent->on_dependancy_finished();
    }
}

void Job::on_dependancy_finished() {
    JobSystem::DispatchAndRelease(this);
}

void Job::dependent_on(Job* parent) {
    AtomicIncrement(&num_dependencies);
    parent->add_dependent(this);
}