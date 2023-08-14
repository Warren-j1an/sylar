#include "config.h"
#include "fiber.h"
#include "hook.h"
#include "iomanager.h"
#include "log.h"

#include <dlfcn.h>
#include <utility>

void hook_init() {
    static bool is_inited = false;
    if (is_inited) {
        return;
    }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");
static sylar::ConfigVar<int>::ptr g_tcp_connect_timeout =
    sylar::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");

static uint64_t s_connect_timeout = -1;
struct _HookIniter {
    _HookIniter() {
        hook_init();
        s_connect_timeout = g_tcp_connect_timeout->getValue();
        g_tcp_connect_timeout->addListener([](const int& old_value, const int& new_value){
            SYLAR_LOG_INFO(g_logger) << "tcp connect timeout changed from " << old_value
                << " to " << new_value;
            s_connect_timeout = new_value;
        });
    }
};
static _HookIniter s_hook_initer;   // use for initial hook func befor main func

namespace sylar {
static thread_local bool t_hook_enable = false;

bool is_hook_enable() {
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}
}   // sylar}

extern "C" {
unsigned int sleep(unsigned int seconds) {
    if (!sylar::is_hook_enable()) {
        return sleep_f(seconds);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(seconds * 1000, std::bind((void(sylar::Scheduler::*)
        (sylar::Fiber::ptr, pid_t))&sylar::IOManager::schedule, iom, fiber, -1));
    sylar::Fiber::YeildToHold();
    return 0;
}

int usleep(useconds_t usec) {
    if (!sylar::is_hook_enable()) {
        return usleep_f(usec);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(usec / 1000, std::bind((void(sylar::Scheduler::*)
        (sylar::Fiber::ptr, pid_t))&sylar::IOManager::schedule, iom, fiber, -1));
    sylar::Fiber::YeildToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!sylar::is_hook_enable()) {
        return nanosleep_f(req, rem);
    }
    uint64_t timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000;
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(timeout_ms, std::bind((void(sylar::Scheduler::*)
        (sylar::Fiber::ptr, pid_t))&sylar::IOManager::schedule, iom, fiber, -1));
    sylar::Fiber::YeildToHold();
    return 0;
}
}