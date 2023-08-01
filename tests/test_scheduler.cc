#include "log.h"
#include "scheduler.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

void test_fiber() {
    static int count = 5;
    SYLAR_LOG_INFO(g_logger) << "Test in fiber, count=" << count;
    if (count-- > 0) {
        sylar::Scheduler::GetThis()->schedule(&test_fiber, sylar::GetThreadID());
    }
}

int main() {
    SYLAR_LOG_INFO(g_logger) << "Test main";
    sylar::Scheduler sc(2, true, "test");
    sc.start();
    sc.schedule(&test_fiber);
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "Test end";
    return 0;
}