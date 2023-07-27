#pragma once

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "log_level.h"
#include "yaml-cpp/yaml.h"

namespace sylar {
template<typename From, typename To>
class LexicalCast {
public:
    To operator()(const From& v) {
        return boost::lexical_cast<To>(v);
    }
};

template<typename T>
class LexicalCast<std::string, std::vector<T>> {
public:
    std::vector<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename F>
class LexicalCast<std::vector<F>, std::string> {
public:
    std::string operator()(const std::vector<F>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::list<T>> {
public:
    std::list<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::list<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename F>
class LexicalCast<std::list<F>, std::string> {
public:
    std::string operator()(const std::list<F>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename F>
class LexicalCast<std::set<F>, std::string> {
public:
    std::string operator()(const std::set<F>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::unordered_set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename F>
class LexicalCast<std::unordered_set<F>, std::string> {
public:
    std::string operator()(const std::unordered_set<F>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::map<std::string, T> map;
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            map.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return map;
    }
};

template<typename F>
class LexicalCast<std::map<std::string, F>, std::string> {
public:
    std::string operator()(const std::map<std::string, F>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<F, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::unordered_map<std::string, T>> {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::unordered_map<std::string, T> map;
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it++) {
            ss.str("");
            ss << it->second;
            map.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return map;
    }
};

template<typename F>
class LexicalCast<std::unordered_map<std::string, F>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, F>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<F, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * LexicalCast between LogDefine and string
 * When change config, use call back function to set Log
*/
struct LogAppenderDefine {
    int type = 0;   // 1 for file, 0 for stdout
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKONW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }
};

template<>
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        LogDefine log_define;

        if (!node["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << node << std::endl;
			throw std::logic_error("log config name null");
        }
        log_define.name = node["name"].as<std::string>();
        log_define.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
        log_define.formatter = node["formatter"].IsDefined() ? node["formatter"].as<std::string>() : "";
        if (node["appenders"].IsDefined()) {
            for (size_t i = 0; i < node["appenders"].size(); i++) {
                YAML::Node appender = node["appenders"][i];
                if (!appender["type"].IsDefined()) {
                    std::cout << "log config error: appender type is null, " << appender << std::endl;
					continue;
                }
                std::string type = appender["type"].as<std::string>();
                LogAppenderDefine appender_define;
                if (type == "FileLogAppender") {
                    appender_define.type = 1;
                    if (!appender["file"].IsDefined()) {
                        std::cout << "log config error: file_appender file is null, " << appender << std::endl;
					    continue;
                    }
                    appender_define.file = appender["file"].as<std::string>();
                    appender_define.formatter = appender["formatter"].IsDefined() ? appender["formatter"].as<std::string>(): "";
                } else if (type == "StdoutLogAppender") {
                    appender_define.type = 0;
                    appender_define.formatter = appender["formatter"].IsDefined() ? appender["formatter"].as<std::string>(): "";
                } else {
                    std::cout << "log config error: appender type is invalid, " << appender << std::endl;
					continue;
                }
                log_define.appenders.push_back(appender_define);
            }
        }
        return log_define;
    }
};

template<>
class LexicalCast<LogDefine, std::string> {
public:
    std::string operator()(const LogDefine& log) {
        YAML::Node node;
        node["name"] = log.name;
        node["level"] = LogLevel::ToString(log.level);
        if (!log.formatter.empty()) {
            node["formatter"] = log.formatter;
        }
        for (auto& appender : log.appenders) {
            YAML::Node sub_node;
            if (appender.type == 1) {
                sub_node["type"] = "FileLogAppender";
                sub_node["file"] = appender.file;
            } else {
                sub_node["type"] = "StdoutLogAppender";
            }
            if (!appender.formatter.empty()) {
                sub_node["formatter"] = appender.formatter;
            }
            node.push_back(sub_node);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}