#include "log_appender.h"
#include "log_formatter.h"
#include "log_level.h"
#include "log.h"

#include <iostream>
#include <time.h>

void test_logformatter() {
    sylar::LogFormatter::ptr fmt = 
            std::make_shared<sylar::LogFormatter>("%d%T%m%n");

    sylar::LogEvent::ptr event = 
        std::make_shared<sylar::LogEvent>(__FILE__, __LINE__, 0, 0, 0, time(NULL), "main", sylar::LogLevel::DEBUG);

    event->getSS() << "Test Formatter: " << (bool)fmt->isError();

    fmt->format(std::cout, event);
}

void test_logAppender() {
    sylar::LogEvent::ptr event = 
        std::make_shared<sylar::LogEvent>(__FILE__, __LINE__, 0, 0, 0, time(NULL), "main", sylar::LogLevel::DEBUG);

    event->getSS() << "Test Formatter";

    sylar::LogAppender::ptr std_append = std::make_shared<sylar::StdoutLogAppender>();
    sylar::LogAppender::ptr file_append = std::make_shared<sylar::FileLogAppender>("../bin/log/system.txt");

    sylar::LogFormatter::ptr fmt_1 = 
            std::make_shared<sylar::LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T%f:%l%T%m%n");
    sylar::LogFormatter::ptr fmt_2 = 
            std::make_shared<sylar::LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T[%p]%T%f:%l%T%m%n");
    std_append->setFormatter(fmt_1);
    file_append->setFormatter(fmt_2);
    std_append->log(event);
    file_append->log(event);

    std::cout << std_append->toYamlString() << std::endl;
}

void test_log() {
    sylar::Logger::ptr log = std::make_shared<sylar::Logger>();
    sylar::LogAppender::ptr std_append = std::make_shared<sylar::StdoutLogAppender>();
    sylar::LogAppender::ptr file_append = std::make_shared<sylar::FileLogAppender>("../bin/log/system.txt");
    log->addAppender(std_append);
    log->addAppender(file_append);

    sylar::LogEvent::ptr event = 
        std::make_shared<sylar::LogEvent>(__FILE__, __LINE__, 0, 0, 0, time(NULL), "main", sylar::LogLevel::DEBUG);

    event->getSS() << "Test Formatter";

    log->setLevel(sylar::LogLevel::INFO);
    
    log->log(event);
    std::cout << log->toYamlString() << std::endl;
}

void test_logwraper() {
    sylar::Logger::ptr g_logger = sylar::LoggerManager::GetInstance()->getLogger("root");
    SYLAR_LOG_DEBUG(g_logger) << "Test DEBUG";
    SYLAR_LOG_INFO(g_logger) << "Test INFO";
    SYLAR_LOG_WARN(g_logger) << "Test WARN";
    SYLAR_LOG_ERROR(g_logger) << "Test ERROR";
    SYLAR_LOG_FATAL(g_logger) << "Test FATAL";

    SYLAR_LOG_FMT_DEBUG(g_logger, "Test DEBUG, %s, %d", "format", 1);
    SYLAR_LOG_FMT_INFO(g_logger, "Test INFO, %s, %d", "format", 1);
    SYLAR_LOG_FMT_WARN(g_logger, "Test WARN, %s, %d", "format", 1);
    SYLAR_LOG_FMT_ERROR(g_logger, "Test ERROR, %s, %d", "format", 1);
    SYLAR_LOG_FMT_FATAL(g_logger, "Test FATAL, %s, %d", "format", 1);
}

int main() {
    test_log();
    return 0;
}