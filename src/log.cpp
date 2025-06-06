#include "log.hpp"

#include <chrono>

Log& Log::getInstance() {
    static Log instance;
    return instance;
}

Log::~Log() {
#ifndef DEBUG
    logFile_.close();
#endif
}

void Log::log(LogType type, const std::string& msg, const std::string& file,
    const std::string& function, u32 line, bool callOnce) {
    if(callOnce) {
        std::string cacheEntry = msg + file + function + std::to_string(line);

        // allready in cache
        if(logOnceCache_.find(cacheEntry) != logOnceCache_.end()) {
            return;
        } else {
            logOnceCache_.insert(cacheEntry);
        }
    }

    std::string logEntry =
        "[" + currentDateTime() + "] " + "<" + logTypeToString(type) + "> " + msg;

#ifdef DEBUG

    if(type != LogType::INFO) {
        logEntry += " | " + function + " " + file + "(" + std::to_string(line) + ")";
    }
    std::cerr << logEntry << std::endl;
#else
    logFile_ << logEntry << std::endl;

    if(type == LogType::FATAL_ERROR) {
        logFile_.close();
    }

#endif
}

std::string Log::currentDateTime() {
    auto tp = std::chrono::system_clock::now();
    return std::format("{:%d/%m/%Y %H:%M:%S}", tp);
}

std::string Log::logTypeToString(LogType type) {
    switch(type) {
        case LogType::INFO:
            return "INFO";
        case LogType::ERROR:
            return "ERROR";
        case LogType::FATAL_ERROR:
            return "FATAL ERROR";
        default:
            break;
    }
    return "";
}

Log::Log() {
#ifndef DEBUG
    logFile_.open("log.txt", std::ios::out);
    if(!logFile_.is_open()) {
        std::cerr << "failed to open log file\n";
    }
#endif
}
