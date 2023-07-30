#include "fiber.h"
#include "log.h"

#include <vector>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

void run_in_fiber() {
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber() begin";
    sylar::Fiber::YeildToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber() end";
}

void test_fiber() {
    sylar::Fiber::ptr main_fiber = sylar::Fiber::GetThis();
    SYLAR_LOG_INFO(g_logger) << "test fiber: begin";
    sylar::Fiber::ptr fiber = std::make_shared<sylar::Fiber>(run_in_fiber);
    fiber->swapIn();
    SYLAR_LOG_INFO(g_logger) << "test fiber: after swapIn()";
    fiber->swapIn();
    SYLAR_LOG_INFO(g_logger) << "test fiber: end";
}

int main() {
    sylar::Thread::SetName("main");
    std::vector<sylar::Thread::ptr> thrs;
    for (size_t i = 0; i < 3; ++i) {
        thrs.push_back(std::make_shared<sylar::Thread>(&test_fiber, "name_" + std::to_string(i)));
    }
    for (auto i : thrs) {
        i->join();
    }
    return 0;
}