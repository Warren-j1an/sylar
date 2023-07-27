#include "log.h"
#include "yaml-cpp/yaml.h"

#include <iostream>

namespace sylar {
Logger::Logger(const std::string &name, LogLevel::Level level) : 
    m_name(name), m_level(level) {
    m_formatter = std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T%f:%l%T%m%n");
}

void Logger::log(LogEvent::ptr event) {
    if (m_level > event->getLevel()) {
        return;
    }
    MutexType::Lock lock(m_mutex);
    for (auto appender : m_appenders) {
        appender->log(event);
    }
}

void Logger::addAppender(LogAppender::ptr appender) {
    if (!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    MutexType::Lock lock(m_mutex);
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    MutexType::Lock lock(m_mutex);
    for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders() {
    MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::ptr fmt) {
    MutexType::Lock lock(m_mutex);
    m_formatter = fmt;
    for (auto appender : m_appenders) {
        appender->setFormatter(fmt);
    }
}

void Logger::setFormatter(const std::string& pattern) {
    LogFormatter::ptr fmt = std::make_shared<LogFormatter>(pattern);
    if (fmt->isError()) {
        std::cout << "ERROR Logger::setFormatter name=" << m_name 
            << ", pattern=" << pattern << std::endl; 
        return;
    }
    setFormatter(fmt);
}

LogFormatter::ptr Logger::getFormatter() {
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

std::string Logger::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    node["level"] = LogLevel::ToString(m_level);
    if (m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    for (auto appender : m_appenders) {
        node["appenders"].push_back(YAML::Load(appender->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

LogWraper::LogWraper(Logger::ptr log, LogEvent::ptr event) : m_logger(log), m_event(event) {}

LogWraper::~LogWraper() {
    m_logger->log(m_event);
}

LogEvent::ptr LogWraper::getEvent() {
    return m_event;
}

std::stringstream& LogWraper::getSS() {
    return m_event->getSS();
}

LoggerManager* LoggerManager::GetInstance() {
    return Singleton<LoggerManager>::GetInstance();
}

LoggerManager::LoggerManager() {
    Logger::ptr root = std::make_shared<Logger>();
    LogAppender::ptr appender = std::make_shared<StdoutLogAppender>();
    root->addAppender(appender);
    m_loggers[root->getName()] = root;
}

Logger::ptr LoggerManager::getLogger(const std::string &name) {
    MutexType::Lock lock(m_mutex);
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    } else {
        Logger::ptr log = std::make_shared<Logger>(name);
        m_loggers[name] = log;
        return log;
    }
}

std::string LoggerManager::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    for (auto log : m_loggers) {
        node.push_back(YAML::Load(log.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}
}