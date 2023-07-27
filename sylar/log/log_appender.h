#pragma once

#include <fstream>

#include "log_formatter.h"
#include "mutex.h"

namespace sylar {
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Spinlock MutexType;

    LogAppender();
    virtual ~LogAppender() {}
    virtual void log(LogEvent::ptr event) = 0;
    virtual std::string toYamlString() = 0;
    void setFormatter(LogFormatter::ptr fmt);
    LogFormatter::ptr getFormatter();

protected:
    LogFormatter::ptr m_formatter;
    MutexType m_mutex;
};

class StdoutLogAppender : public LogAppender {
public:
    void log(LogEvent::ptr event) override;
    std::string toYamlString() override;
};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender(const std::string& file);
    void log(LogEvent::ptr event) override;
    std::string toYamlString() override;
    bool reopen();

private:
    std::string m_filename;
    std::ofstream m_filestream;
    uint64_t m_lastTime;
};
}