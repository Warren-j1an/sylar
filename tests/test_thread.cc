#include "config.h"
#include "log.h"
#include "mutex.h"
#include "thread.h"
#include "util.h"

#include <vector>
#include <yaml-cpp/yaml.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

int count = 0;
sylar::Mutex s_mutex;

void func() {
    SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName() << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadID() << " this.id: " << sylar::Thread::GetThis()->getID();

    for (int i = 0; i < 1000000; i++) {
        sylar::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    while (true) {
        SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while (true) {
        SYLAR_LOG_INFO(g_logger) << "========================================";
    }
}

int main() {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("../bin/conf/log.yaml");
    sylar::Config::LoadFromYaml(root);

    std::vector<sylar::Thread::ptr> thrs;
    for (int i = 0; i < 2; ++i) {
        sylar::Thread::ptr thr = std::make_shared<sylar::Thread>(&fun2, "name_" + std::to_string(i * 2));
        sylar::Thread::ptr thr2 = std::make_shared<sylar::Thread>(&fun3, "name_" + std::to_string(i * 2 + 1));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }
    for (size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "count=" << count;
    return 0;
}