#include "config.h"

#include <algorithm>
#include <list>

namespace sylar {
void listAllMember(const std::string& name, const YAML::Node& node, 
                   std::list<std::pair<std::string, const YAML::Node>>& output) {
    if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
        SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "Config invalid name: " << name << " : " << node;
        return;
    }
    output.push_back(std::make_pair(name, node));
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            listAllMember(name.empty() ? it->first.Scalar() : name + "." + it->first.Scalar(), it->second, output);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    listAllMember("", root, all_nodes);
    for (auto& i : all_nodes) {
        std::string key = i.first;
        if (key.empty()) {
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);

        if (var) {
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
    auto m_map = GetDatas();
    for (auto it = m_map.begin(); it != m_map.end(); it++) {
        cb(it->second);
    }
}

/**
 * Set log from config
*/
ConfigVar<std::set<LogDefine>>::ptr g_log_defines = Config::Lookup("logs", std::set<LogDefine>(), "logs config");

struct LogIniter {
    LogIniter() {
        g_log_defines->addListener([](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value){
            SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) << "Logger config changed";
            for (auto& i : new_value) {
                Logger::ptr logger;
                auto it = old_value.find(i);
                if (it != old_value.end() && i == *it) {
                    continue;
                }
                logger = SYLAR_LOG_NAME(i.name);
                logger->setLevel(i.level);
                if (!i.formatter.empty()) {
                    logger->setFormatter(i.formatter);
                }

                logger->clearAppenders();
                for (auto& a : i.appenders) {
                    LogAppender::ptr ap;
                    if (a.type == 1) {
                        ap = std::make_shared<FileLogAppender>(a.file);
                    } else {
                        ap = std::make_shared<StdoutLogAppender>();
                    }
                    if (!a.formatter.empty()) {
                        LogFormatter::ptr fmt = std::make_shared<LogFormatter>(a.formatter);
                        if (fmt->isError()) {
                            std::cout << "log.name=" << i.name << " appender type=" << a.type << " formatter=" << a.formatter << " is invalid\n";
                        }else {
                            ap->setFormatter(fmt);
                        }
                    }
                    logger->addAppender(ap);
                }
            }

            for (auto& i : old_value) {
                auto it = new_value.find(i);
                if (it == new_value.end()) {
                    Logger::ptr logger = SYLAR_LOG_NAME(i.name);
                    logger->setLevel(LogLevel::UNKONW);
                    logger->clearAppenders();
                }
            }
        });
    }
};

static LogIniter __log_init;
}