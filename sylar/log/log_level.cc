#include "log_level.h"

#include <algorithm>

namespace sylar {
const char *LogLevel::ToString(LogLevel::Level level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKONW";
    }
}

LogLevel::Level LogLevel::FromString(const std::string &str) {
    if (str == "DEBUG" || str == "debug") {
        return LogLevel::DEBUG;
    }
    if (str == "INFO" || str == "info") {
        return LogLevel::INFO;
    }
    if (str == "WARN" || str == "warn") {
        return LogLevel::WARN;
    }
    if (str == "ERROR" || str == "error") {
        return LogLevel::ERROR;
    }
    if (str == "FATAL" || str == "fatal") {
        return LogLevel::FATAL;
    }
    return LogLevel::UNKONW;
}
}