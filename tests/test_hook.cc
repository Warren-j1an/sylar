#include "hook.h"
#include "iomanager.h"
#include "log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

void test_sleep() {
    sylar::set_hook_enable(true);
    sylar::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        SYLAR_LOG_INFO(g_logger) << "sleep 2";
    });
    iom.schedule([](){
        sleep(3);
        SYLAR_LOG_INFO(g_logger) << "sleep 3";
    });
    SYLAR_LOG_INFO(g_logger) << "test sleep";
}

int main() {
    test_sleep();
    return 0;
}