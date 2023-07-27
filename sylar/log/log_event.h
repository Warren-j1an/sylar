#pragma once

#include <time.h>
#include <memory>
#include <sstream>
#include <stdarg.h>

#include "log_level.h"

namespace sylar {
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent(const std::string& file, uint32_t line, uint32_t elaps, uint32_t threadID, 
        uint32_t fiberID, time_t time, const std::string& threadName, LogLevel::Level level);
    
    const std::string& getFile() const { return m_file; }
    uint32_t getLine() const { return m_line; }
    uint32_t getElaps() const { return m_elaps; }
    uint32_t getThreadID() const { return m_threadID; }
    uint32_t getFiberID() const { return m_fiberID; }
    time_t getTime() const { return m_time; }
    const std::string& getThreadName() const { return m_threadName; }
    std::string getContent() const { return m_ss.str(); }
    LogLevel::Level getLevel() const { return m_level; }

    std::stringstream& getSS() { return m_ss; }

    void format(const char *fmt, ...);
    void format(const char *fmt, va_list al);

private:
    std::string m_file;
    uint32_t m_line;
    uint32_t m_elaps;
    uint32_t m_threadID;
    uint32_t m_fiberID;
    time_t m_time;
    std::string m_threadName;
    std::stringstream m_ss;
    LogLevel::Level m_level;
};
}