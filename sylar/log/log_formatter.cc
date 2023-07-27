#include "log_formatter.h"

#include <ctype.h>
#include <functional>
#include <map>
#include <sstream>
#include <string>

namespace sylar {
LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern), m_error(false) {
    init();
}

std::ostream& LogFormatter::format(std::ostream& ofs, LogEvent::ptr event) {
    for (auto item : m_items) {
        item->format(ofs, event);
    }
    return ofs;
}

// "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
void LogFormatter::init() {
    std::vector<std::tuple<std::string, std::string>> vec;
    std::string nstr;
    for (size_t i = 0; i < m_pattern.size(); i++) {
        if (m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%') {
            nstr.append(1, m_pattern[i + 1]);
            continue;
        }

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple("s", nstr));
        }
        nstr.clear();

        size_t n = i + 2;
        std::string str, fmt;
        if (i + 1 == m_pattern.size() || !isalpha(m_pattern[i + 1])) {
            m_error = true;
            str = "<<error pattern>>";
        }
        else {
            str = m_pattern.substr(i + 1, 1);
        }

        if (n < m_pattern.size() && m_pattern[n] == '{') {
            n++;
            while (n < m_pattern.size() && m_pattern[n] != '}') {
                n++;
            }
            if (n == m_pattern.size()) {
                m_error = true;
                vec.push_back(std::make_tuple("s", str + "{<<error format>>}"));
                i = n;
                continue;
            }
            fmt = m_pattern.substr(i + 3, n - i - 3);
            vec.push_back(std::make_tuple(str, fmt));
            i = n;
        }
        else {
            vec.push_back(std::make_tuple(str, ""));
            i = i + 1;
        }
    }
    
    for (auto i : vec) {
        if (std::get<0>(i) == "m") {
            m_items.push_back(std::make_shared<MessageFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "p") {
            m_items.push_back(std::make_shared<LevelFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "r") {
            m_items.push_back(std::make_shared<ElapseFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "t") {
            m_items.push_back(std::make_shared<ThreadIdFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "n") {
            m_items.push_back(std::make_shared<NewLineFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "d") {
            m_items.push_back(std::make_shared<DateTimeFormatItem>(std::get<1>(i)));
            continue;
        }
        if (std::get<0>(i) == "f") {
            m_items.push_back(std::make_shared<FilenameFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "l") {
            m_items.push_back(std::make_shared<LineFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "T") {
            m_items.push_back(std::make_shared<TabFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "F") {
            m_items.push_back(std::make_shared<FiberIdFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "N") {
            m_items.push_back(std::make_shared<ThreadNameFormatItem>());
            continue;
        }
        if (std::get<0>(i) == "s") {
            m_items.push_back(std::make_shared<StringFormatItem>(std::get<1>(i)));
            continue;
        }
        m_error = true;
        m_items.push_back(std::make_shared<StringFormatItem>("<<unkonw pattern>>"));
    }
}
}