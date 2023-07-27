#pragma once

#include <time.h>
#include <memory>
#include <vector>
#include <sstream>

#include "log_level.h"
#include "log_event.h"

namespace sylar {
/**
 * m, MessageFormatItem             //m:消息
 * p, LevelFormatItem               //p:日志级别
 * r, ElapseFormatItem              //r:累计毫秒数
 * t, ThreadIdFormatItem            //t:线程id
 * n, NewLineFormatItem             //n:换行
 * d, DateTimeFormatItem            //d:时间
 * f, FilenameFormatItem            //f:文件名
 * l, LineFormatItem                //l:行号
 * T, TabFormatItem                 //T:Tab
 * F, FiberIdFormatItem             //F:协程id
 * N, ThreadNameFormatItem          //N:线程名称
 * s, StringFormatItem              //s:直接输出给定字符串
*/
class FormatItem {
public:
    typedef std::shared_ptr<FormatItem> ptr;

    virtual ~FormatItem() {}
    virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
};

class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    LogFormatter(const std::string& pattern);
    std::ostream& format(std::ostream& ofs, LogEvent::ptr event);

    void init();
    bool isError() const { return m_error; }
    const std::string getPattern() const { return m_pattern; }

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error;
};

class MessageFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << LogLevel::ToString(event->getLevel());
    }
};

class ElapseFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getElaps();
    }
};

class ThreadIdFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getThreadID();
    }
};

class NewLineFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class DateTimeFormatItem : public FormatItem {
public:
    DateTimeFormatItem(const std::string& pattern = "%Y-%m-%d %H:%M:%S") : m_pattern(pattern) {
        if (m_pattern.empty()) {
            m_pattern = "%Y-%m-%d %H:%M:%S";
        }
    }

    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_pattern.c_str(), &tm);
        os << buf;
    }

private:
    std::string m_pattern;
};

class FilenameFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class TabFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << "\t";
    }
};

class FiberIdFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getFiberID();
    }
};

class ThreadNameFormatItem : public FormatItem {
public:
    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};

class StringFormatItem : public FormatItem {
public:
    StringFormatItem(const std::string& string) : m_string(string) {}

    virtual void format(std::ostream& os, LogEvent::ptr event) override {
        os << m_string;
    }

private:
    std::string m_string;
};
}