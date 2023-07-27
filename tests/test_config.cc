#include <iostream>

#include "yaml-cpp/yaml.h"

#include "config_cast.h"
#include "config_var.h"
#include "config.h"
#include "log.h"
#include "util.h"

std::string type(YAML::Node n) {
    if (n.IsScalar()) return "Scalar";
    if (n.IsSequence()) return "Sequence";
    if (n.IsMap()) return "Map";
    return "Null";
}

void print_yaml(const YAML::Node& node, int level) {
    if (node.IsScalar()) {
        SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) << std::string(level * 4, ' ') << node.Scalar() << " - " << type(node) << " - " << level;
    } else if (node.IsNull()) {
        SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) <<  std::string(level * 4, ' ') << "NULL - " << type(node) << " - " << level;
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) << std::string(level * 4, ' ') << it->first << " - " << type(node) << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); i++) {
            SYLAR_LOG_INFO(SYLAR_LOG_NAME("root")) << std::string(level * 4, ' ') << i << " - " << type(node[i]) << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    sylar::ConfigVar<int>::ptr c_int = 
        sylar::Config::Lookup("config.int", 1, "config int test");
    sylar::ConfigVar<float>::ptr c_float = 
        sylar::Config::Lookup("config.float", (float)9.01, "config float test");
    sylar::ConfigVar<std::string>::ptr c_string = 
        sylar::Config::Lookup("config.string", std::string("test"), "config string test");
    sylar::ConfigVar<std::vector<int>>::ptr c_vec_int = 
        sylar::Config::Lookup("config.vec_int", std::vector<int>{1, 2}, "config vec_int test");
    sylar::ConfigVar<std::set<int>>::ptr c_set_int = 
        sylar::Config::Lookup("config.set_int", std::set<int>{3, 4}, "config set_int test");
    sylar::ConfigVar<std::unordered_set<int>>::ptr c_unset_int = 
        sylar::Config::Lookup("config.unset_int", std::unordered_set<int>{5, 6}, "config unset_int test");
    sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr c_unmap_int = 
        sylar::Config::Lookup("config.map_int", std::unordered_map<std::string, int>{{"key", 2}}, "config map_int test");
    sylar::ConfigVar<std::map<std::string, std::vector<int>>>::ptr c_map_vec_int = 
        sylar::Config::Lookup("config.map_vec_int", std::map<std::string, std::vector<int>>{{"key", {1, 2}}}, "config int test");
    
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_float->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_string->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_vec_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_set_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_unset_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_unmap_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_map_vec_int->toString();

    SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "---------------------------befor-------------------------"; 

    YAML::Node node = YAML::LoadFile("../bin/conf/test.yaml");
    // print_yaml(node, 0);
    sylar::Config::LoadFromYaml(node);

    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_float->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_string->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_vec_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_set_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_unset_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_unmap_int->toString();
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "\n" << c_map_vec_int->toString();
}

void test_log() {
    sylar::Logger::ptr log_system = SYLAR_LOG_NAME("system");
    sylar::Logger::ptr log_root = SYLAR_LOG_NAME("root");
    SYLAR_LOG_DEBUG(log_system) << "system hello";
    SYLAR_LOG_DEBUG(log_root) << "root hello";
    YAML::Node node = YAML::LoadFile("../bin/conf/log.yaml");
    sylar::Config::LoadFromYaml(node);
    std::cout << "=============" << std::endl;
    std::cout << sylar::LoggerManager::GetInstance()->toYamlString() << std::endl;
    std::cout << "=============" << std::endl;
    SYLAR_LOG_DEBUG(log_system) << "system hello";
    SYLAR_LOG_DEBUG(log_root) << "root hello";
}

int main() {
    test_log();
    return 0;
}