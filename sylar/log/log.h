#pragma once

#include <map>
#include <memory>
#include <list>

#include "log_appender.h"
#include "log_event.h"
#include "log_level.h"
#include "mutex.h"
#include "singleton.h"
#include "thread.h"
#include "util.h"

#define SYLAR_LOG_LEVEL(logger, level) \
    sylar::LogWraper(logger, \
        std::make_shared<sylar::LogEvent>(__FILE__, __LINE__, 0, sylar::GetThreadID(), \
        sylar::GetFiberID(), time(NULL), sylar::Thread::GetName(), level)).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    sylar::LogWraper(logger, std::make_shared<sylar::LogEvent>(__FILE__, __LINE__, 0, sylar::GetThreadID(), \
        sylar::GetFiberID(), time(NULL), sylar::Thread::GetName(), level)).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_NAME(name) sylar::LoggerManager::GetInstance()->getLogger(name)


namespace sylar {
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Spinlock MutexType;

    Logger(const std::string& name = "root", LogLevel::Level level = LogLevel::DEBUG);
    void log(LogEvent::ptr event);
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();
    void setFormatter(LogFormatter::ptr fmt);
    void setFormatter(const std::string& pattern);
    LogFormatter::ptr getFormatter();
    const std::string& getName() const { return m_name; }
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }
    std::string toYamlString();

private:
    std::string m_name;
    LogLevel::Level m_level;
    std::list<LogAppender::ptr> m_appenders;
    LogFormatter::ptr m_formatter;
    MutexType m_mutex;
};

class LogWraper {
public:
    LogWraper(Logger::ptr log, LogEvent::ptr event);
    ~LogWraper();
    LogEvent::ptr getEvent();
    std::stringstream& getSS();
    
private:
    Logger::ptr m_logger;
    LogEvent::ptr m_event;
};

class LoggerManager {
friend class Singleton<LoggerManager>;
public:
    typedef Spinlock MutexType;

    static LoggerManager* GetInstance();
    Logger::ptr getLogger(const std::string &name);
    std::string toYamlString();

private:
    LoggerManager();

private:
    std::map<std::string, Logger::ptr> m_loggers;
    MutexType m_mutex;
};
}