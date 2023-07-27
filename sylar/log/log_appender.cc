#include "log_appender.h"
#include "yaml-cpp/yaml.h"

#include <iostream>

namespace sylar {
LogAppender::LogAppender() : m_formatter(nullptr) {}

void LogAppender::setFormatter(LogFormatter::ptr fmt) {
    MutexType::Lock lock(m_mutex);
    m_formatter = fmt;
}

LogFormatter::ptr LogAppender::getFormatter() {
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

void StdoutLogAppender::log(LogEvent::ptr event) {
    MutexType::Lock lock(m_mutex);
    if (m_formatter) {
        m_formatter->format(std::cout, event);
    }
}

std::string StdoutLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if (m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

FileLogAppender::FileLogAppender(const std::string& file) : LogAppender(), m_filename(file) {
    reopen();
}

void FileLogAppender::log(LogEvent::ptr event) {
    MutexType::Lock lock(m_mutex);
    m_formatter->format(m_filestream, event);
}

std::string FileLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if (m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::app);
    return !!m_filestream;
}
}