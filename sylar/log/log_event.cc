#include "log_event.h"

namespace sylar{
LogEvent::LogEvent(const std::string& file, uint32_t line, uint32_t elaps, uint32_t threadID, 
    uint32_t fiberID, time_t time, const std::string& threadName, LogLevel::Level level) : 
    m_file(file), m_line(line), m_elaps(elaps), m_threadID(threadID), 
    m_fiberID(fiberID), m_time(time), m_threadName(threadName), m_level(level) {}

void LogEvent::format(const char *fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char *fmt, va_list al) {
    char *buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != -1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}
}