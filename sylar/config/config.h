#pragma once

#include <string>
#include <unordered_map>

#include "config_var.h"

namespace sylar {
class Config {
public:
    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description = "") {
        auto it = GetDatas().find(name);
        if (it != GetDatas().end()) {
            auto temp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (temp) {
                SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) << "Config::Lookup name=" << name << " exists";
                return temp;
            } else {
                SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "Config::Lookup name=" << name << " exists but type not "
                    << TypetoName<T>() << ", real type=" << it->second->getTypeName() << " : " << it->second->toString();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
            SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "Config::Lookup invalid name=" << name;
            throw std::invalid_argument(name);
        }

        ConfigVarBase::ptr v = std::make_shared<ConfigVar<T>>(name, default_value, description);
        GetDatas()[name] = v;
        return std::dynamic_pointer_cast<ConfigVar<T>>(v);
    }

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        std::unordered_map<std::string, ConfigVarBase::ptr> m_map = GetDatas();
        auto it = m_map.find(name);
        if (it == m_map.end()) {
            return nullptr;
        } else {
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
    }

    static void LoadFromYaml(const YAML::Node& root);
    // static void LoadFromConfDir(const std::string& path, bool force = false);
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
    static std::unordered_map<std::string, ConfigVarBase::ptr>& GetDatas() {
        static std::unordered_map<std::string, ConfigVarBase::ptr> m_datas;
        return m_datas;
    }
};
}