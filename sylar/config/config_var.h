#pragma once 

#include <algorithm>
#include <functional>
#include <memory>
#include <string>

#include "config_cast.h"
#include "log.h"
#include "util.h"

namespace sylar {
class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    ConfigVarBase(const std::string& name, const std::string& description = "") : 
        m_name(name), m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    virtual ~ConfigVarBase() {}
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
    virtual std::string getTypeName() const = 0;

protected:
    std::string m_name;
    std::string m_description;
};

template<typename T>
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

    ConfigVar(const std::string& name, const T& default_val, const std::string& description = "") : 
        ConfigVarBase(name, description), m_val(default_val) {}
    
    std::string toString() override {
        try {
            return LexicalCast<T, std::string>()(m_val);
        } catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "ConfigVar::toString exception " << e.what() 
                << " convert: " << getTypeName() << " to string" << " name=" << m_name;
        }
        return "";
    }

    bool fromString(const std::string& val) override {
        try {
            setValue(LexicalCast<std::string, T>()(val));
            return true;
        } catch (std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "ConfigVar::fromString exception " << e.what() 
                << " convert string=" << val << " to " << getTypeName() << " name=" << m_name;
            return false;
        }
    }

    std::string getTypeName() const override {
        return TypetoName<T>();
    }

    const T getValue() {
        return m_val;
    }

    void setValue(const T& v) {
        if (m_val != v) {
            for (auto cb : m_cbs) {
                cb.second(m_val, v);
            }
            m_val = v;
        } 
    }

    uint64_t addListener(on_change_cb cb) {
        static uint64_t s_fun_id = 0;
        s_fun_id++;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    void delListener(uint64_t key) {
        m_cbs.erase(key);
    }

    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener() {
        m_cbs.clear();
    }

private:
    T m_val;
    std::map<uint64_t, on_change_cb> m_cbs;
};
}