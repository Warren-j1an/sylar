#pragma once

#include <string>

namespace sylar {
class LogLevel {
public:
    enum Level {
        UNKONW = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    static const char *ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string &str);
};
}