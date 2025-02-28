#pragma once

#include "utils.hpp"

#include <fstream>

enum class LogType {
    INFO = 0,
    ERROR = 1,
    FATAL_ERROR = 2
};

#define INFO(msg) Log::getInstance().log(LogType::INFO, msg, __FILE__, __func__, __LINE__);
#define ERROR(msg) Log::getInstance().log(LogType::ERROR, msg, __FILE__, __func__, __LINE__);
#define ERROR_ONCE(msg) Log::getInstance().log(LogType::ERROR, msg, __FILE__, __func__, __LINE__, true);
#define FATAL_ERROR(msg) Log::getInstance().log(LogType::FATAL_ERROR, msg, __FILE__, __func__, __LINE__); __builtin_trap();

class Log {

public:
    static Log& getInstance();
    ~Log();

public:
    void log(LogType type, const std::string& msg, const std::string& file, const std::string& function, u32 line, bool callOnce = false);

private:
    std::fstream logFile_;
    std::unordered_set<std::string> logOnceCache_;

private:
    std::string currentDateTime();
    std::string logTypeToString(LogType type);

private:
    Log();
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;
};