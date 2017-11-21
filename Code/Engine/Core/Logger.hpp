#pragma once

#include <condition_variable>
#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <thread>

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/CriticalSection.hpp"
#include "Engine/Core/Signal.hpp"
#include "Engine/Core/ThreadSafeQueue.hpp"

class Logger : public EngineSubsystem {
public:

    enum class LogMode : uint8_t {
        ENABLE,
        DISABLE,
    };

    Logger();
    virtual ~Logger() override;
    
    void LogStartup(const char* filepath);
    void LogShutdown();
    void LogPrintf(const char* messageFormat, ...);
    void LogWarnf(const char* messageFormat, ...);
    void LogErrorf(const char* messageFormat, ...);
    void LogTagf(const char* tag, const char* messageFormat, ...);
    void LogFlush();
    void LogFlushTest();
    void LogThreadTest(const std::string& path, unsigned int byte_size);
    void ThreadTest(const std::string& path, unsigned int byte_size);

    void LogEnableTag(const char* tag);
    void LogDisableTag(const char* tag);

    void LogEnableAll();
    void LogDisableAll();

    static void Lock();
    static void Unlock();

    std::ofstream& GetStream();


    virtual void Initialize() override;

    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;
protected:
    void LogTagf_list(const char* tag, const char* messageFormat, va_list variableArgumentList);

    void InsertTimeStamp(std::stringstream& msg);
    void InsertTag(std::stringstream& msg, const char* tag);
    void InsertMessage(std::stringstream& msg, const char* messageLiteral);

    void logWorker();

    void RequestFlush();
    void WriteToFile();
    bool IsRunning() const;
    void SetIsRunning(bool isRunning = true);

    std::ofstream _stream;
    std::thread _thread;
    ThreadSafeQueue<std::string> _workerQueue;
    std::set<std::string> _tagList;
    Signal _log_signal;
    Logger::LogMode _logMode;
    bool _isRunning;
    bool _requestFlush;
private:
    static CriticalSection _cs;

};

void JobCopyLog(void* user_data);
void JobPrint(void* user_data);