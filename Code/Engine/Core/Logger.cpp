#include "Engine/Core/Logger.hpp"

#include <cstdarg>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <ctime>

#include "Engine/BuildConfig.cpp"
#include "Engine/EngineConfig.hpp"

#include "Engine/Core/CriticalSection.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

CriticalSection Logger::_cs = CriticalSection();

struct copy_log_job_t {
    Logger* logger;
    std::string filename;
};

void Logger::Lock() {
    Logger::_cs.enter();
}
void Logger::Unlock() {
    Logger::_cs.leave();
}

std::ofstream& Logger::GetStream() {
    return _stream;
}
void Logger::Initialize() {

    g_theConsole->RegisterCommand("log_copy",
                                  [&](const std::string& args) {
        Arguments arg_set(args);
        std::string filename;
        if(arg_set.GetNext(filename)) {
            copy_log_job_t* job_data = new copy_log_job_t;
            job_data->logger = this;
            job_data->filename = filename;
            JobSystem::Run(JobType::JOBTYPE_LOGGING, JobCopyLog, job_data);
        } else {
            g_theConsole->WarnMsg("No destination specified.");
        }
    }
    , "Copies the current log file to [filename]");

}
bool Logger::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}
Logger::Logger()
    : _stream()
    , _thread()
    , _workerQueue()
    , _tagList{}
    , _log_signal()
    , _logMode(LogMode::ENABLE)
    , _isRunning(false)
    , _requestFlush(false)
{
    /* DO NOTHING */
}

Logger::~Logger() {
    _cs.enter();
    _isRunning = false;
    JobSystem::SetCategorySignal(JOBTYPE_LOGGING, nullptr);
    _cs.leave();
}

void Logger::logWorker() {
    JobConsumer log_consumer;
    log_consumer.add_category(JOBTYPE_LOGGING);
    JobSystem::SetCategorySignal(JOBTYPE_LOGGING, &_log_signal);

    while(IsRunning()) {
        _log_signal.wait();
        while(!_workerQueue.empty()) {
            WriteToFile();
        }
        RequestFlush();
        log_consumer.consume_all();
    }
}

void JobCopyLog(void* user_data) {
    namespace FS = std::experimental::filesystem;
    copy_log_job_t* data = (copy_log_job_t*)user_data;
    Logger* logger = data->logger;
    logger->GetStream().flush();
    logger->GetStream().close();
    FS::copy_file("Data/Log/game.log", data->filename, FS::copy_options::overwrite_existing);
    logger->GetStream().open("Data/Log/game.log", std::ios::app);
    delete data;
}

void JobPrint(void* user_data) {
    copy_log_job_t* data = (copy_log_job_t*)user_data;
    Logger* logger = data->logger;
    logger->GetStream() << data->filename;
    logger->GetStream().flush();
    delete data;
}

void Logger::RequestFlush() {
    if(_requestFlush) {
        _stream.flush();
        _requestFlush = false;
    }
}
void Logger::WriteToFile() {
    std::string f;
    if(_workerQueue.pop(f)) {
        _cs.enter();
        if(_stream.is_open()) {
            _stream.write(f.data(), f.size());
        }
        _cs.leave();
    }
}
bool Logger::IsRunning() const {
    _cs.enter();
    bool result = _isRunning;
    _cs.leave();
    return result;
}
void Logger::SetIsRunning(bool isRunning /*= true*/) {
    _cs.enter();
    _isRunning = isRunning;
    _cs.leave();
}
void Logger::LogStartup(const char* filepath) {

    namespace FS = std::experimental::filesystem;
    std::string f = (filepath == nullptr ? "" : filepath);
    FS::path p(f);
    FS::path parent_path = p.parent_path();
    if(!FS::exists(parent_path)) {
        FS::create_directories(parent_path);
    }

    unsigned int file_count = 0;
    for(FS::directory_iterator count_iter(parent_path); count_iter != FS::directory_iterator(); ++count_iter) {
        ++file_count;
    }
    if(file_count > MAX_LOGS + 1u) {
        LogPrintf("Removing old logs.\n");
        unsigned int i = 0;
        for(FS::directory_iterator remove_iter(parent_path); remove_iter != FS::directory_iterator(); ++remove_iter) {
            if(i < MAX_LOGS + 1u) {
                ++i;
                continue;
            }
            auto cur_file = remove_iter->path();
            std::error_code ec;
            if(FS::remove(cur_file, ec)) {
                LogPrintf("Removed %s\n", cur_file.string().c_str());
            }
        }
    }
    
    if(!IsRunning()) {
        SetIsRunning(true);
        _stream.open(p);
        if(_stream.fail()) {
            DebuggerPrintf("Logger failed to initialize!\n");
            _stream.clear();
            _isRunning = false;
        }
        _thread = std::thread(&Logger::logWorker, this); //Create actual thread
    }
}
void Logger::LogShutdown() {
    if(IsRunning()) {
        _log_signal.notify_all();
        SetIsRunning(false);
        _thread.join();
        _stream.flush();
        _stream.close();
    }
}

void Logger::LogPrintf(const char* messageFormat, ...) {
    va_list variableArgumentList;
    va_start(variableArgumentList, messageFormat);
    char* tag = "log";
    LogTagf_list(tag, messageFormat, variableArgumentList);
    va_end(variableArgumentList);
}

void Logger::LogWarnf(const char* messageFormat, ...) {
    va_list variableArgumentList;
    va_start(variableArgumentList, messageFormat);
    char* tag = "warning";
    LogTagf_list(tag, messageFormat, variableArgumentList);
    va_end(variableArgumentList);
}

void Logger::LogErrorf(const char* messageFormat, ...) {
    va_list variableArgumentList;
    va_start(variableArgumentList, messageFormat);
    char* tag = "error";
    LogTagf_list(tag, messageFormat, variableArgumentList);
    va_end(variableArgumentList);
}

void Logger::LogTagf(const char* tag, const char* messageFormat, ...) {
    va_list variableArgumentList;
    va_start(variableArgumentList, messageFormat);
    LogTagf_list(tag, messageFormat, variableArgumentList);
    va_end(variableArgumentList);
}

void Logger::LogTagf_list(const char* tag, const char* messageFormat, va_list variableArgumentList) {
    const int MESSAGE_MAX_LENGTH = 2048;
    char messageLiteral[MESSAGE_MAX_LENGTH];
    vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
    messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

    std::stringstream msg;
    InsertTimeStamp(msg);
    InsertTag(msg, tag);
    InsertMessage(msg, messageLiteral);

    //disablemode = blacklist
    bool foundTag = _tagList.find(tag) != _tagList.end();
    bool enableMode = _logMode == Logger::LogMode::ENABLE;
    bool shouldPush = (!foundTag && enableMode) || (foundTag && !enableMode);
    if(shouldPush) {
        _workerQueue.push(msg.str());
        _log_signal.notify_all();
    }
}

void Logger::InsertTimeStamp(std::stringstream& msg) {
    std::time_t t;
    t = std::time(nullptr);
    std::tm tm;
    ::localtime_s(&tm, &t);
    msg << "[" << std::put_time(&tm, "%H:%M:%S") << "]";
}

void Logger::InsertTag(std::stringstream& msg, const char* tag) {
    msg << "[" << tag << "]";
}

void Logger::InsertMessage(std::stringstream& msg, const char* messageLiteral) {
    msg << messageLiteral;
}
void Logger::LogFlush() {
    _requestFlush = true;
    _log_signal.notify_all();
    while(_requestFlush) {
        std::this_thread::yield();
    }
}

void Logger::ThreadTest(const std::string& path, unsigned int byte_size) {
    std::thread t([=]() {
        g_theFileLogger->LogTagf("test", "Writing %u bytes to file %s...\n", byte_size, path.c_str());
        
        std::vector<unsigned char> buf(byte_size, '\0');
        FileUtils::WriteBufferToFile(buf.data(), buf.size(), path);

        g_theFileLogger->LogTagf("test", "Done.\n");
    });
    t.detach();
}

void Logger::LogThreadTest(const std::string& path, unsigned int byte_size) {
    decltype(auto) thread_work = [=]()->void {
        g_theFileLogger->LogTagf("test", "Writing %u bytes to file %s.\n", byte_size, path.c_str());

        std::vector<unsigned char> buf(byte_size, '\0');
        FileUtils::WriteBufferToFile(buf.data(), buf.size(), path);

        g_theFileLogger->LogTagf("test", "Done.\n");
    };

    std::thread t(thread_work);
    t.detach();
}

void Logger::LogEnableTag(const char* tag) {
    _tagList.erase(tag);
}
void Logger::LogDisableTag(const char* tag) {
    _tagList.insert(tag);
}
void Logger::LogFlushTest() {
    g_theFileLogger->Lock();
    LogTagf("test", "LOG FLUSH TEST\n");
    g_theFileLogger->Unlock();
    LogFlush();
    //__debugbreak();
}
void Logger::LogEnableAll() {
    _logMode = Logger::LogMode::ENABLE;
    _tagList.clear();
}
void Logger::LogDisableAll() {
    _logMode = Logger::LogMode::DISABLE;
}