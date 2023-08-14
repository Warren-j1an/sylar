#include "config.h"
#include "fd_manager.h"
#include "fiber.h"
#include "hook.h"
#include "iomanager.h"
#include "log.h"

#include <dlfcn.h>
#include <cerrno>
#include <memory>
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
}   // sylar

struct TimerInfo {
    int cancelled = 0;
};

template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
                     uint32_t event, int timeout_type, Args&&... args) {
    if (!sylar::is_hook_enable()) {
        return fun(fd, std::forward<Args>(args)...);
    }
    sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd, false);
    if (!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }
    if (ctx->isClose()) {
        errno = EBADF;
        return -1;
    }
    if (!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }
    uint64_t timeout = ctx->getTimeot(timeout_type);
    std::shared_ptr<TimerInfo> timer_info = std::make_shared<TimerInfo>();

retry:
    ssize_t rt = fun(fd, std::forward<Args>(args)...);
    while (rt == -1 && errno == EINTR) {    // system interrupt, try again
        rt = fun(fd, std::forward<Args>(args)...);
    }
    if (rt == -1 && errno == EAGAIN) {  //
        sylar::IOManager* ioManager = sylar::IOManager::GetThis();
        sylar::Timer::ptr timer;
        std::weak_ptr<TimerInfo> weak_info(timer_info);
        if (timeout != ~0ull) {
            timer = ioManager->addConditionTimer(timeout, [weak_info, fd, ioManager, event]{
                std::shared_ptr<TimerInfo> tf = weak_info.lock();
                if (!tf || tf->cancelled) {
                    return;
                }
                tf->cancelled = ETIMEDOUT;
                ioManager->cancelEvent(fd, (sylar::IOManager::Event)event);
            }, weak_info);
        }
        int err = ioManager->addEvent(fd, (sylar::IOManager::Event)event);
        if (err == -1) {
            SYLAR_LOG_ERROR(g_logger) << hook_fun_name << " addEvent("
                << fd << ", " << event << ")";
            if(timer) {
                ioManager->cancel(timer);
            }
            return -1;
        } else {
            sylar::Fiber::YeildToHold();
            if (timer) {
                ioManager->cancel(timer);
            }
            if (timer_info->cancelled) {
                errno = timer_info->cancelled;
                return -1;
            }
            goto retry;
        }
    }
    return rt;
}

int connect_with_timeout(int fd, const struct sockaddr *addr,
    socklen_t addrlen, uint64_t timeout_ms) {
    if (!sylar::is_hook_enable()) {
        return connect_f(fd, addr, addrlen);
    }
    sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd, false);
    if (!ctx || ctx->isClose()) {
        errno = EBADF;
        return -1;
    }
    if (!ctx->isSocket() || ctx->getUserNonblock()) {
        return connect_f(fd, addr, addrlen);
    }
    int rt = connect_f(fd, addr, addrlen);
    if (rt == 0) {
        return 0;
    } else if (errno != EINPROGRESS) {
        return rt;
    }
    sylar::IOManager* ioManager = sylar::IOManager::GetThis();
    sylar::Timer::ptr timer;
    std::shared_ptr<TimerInfo> timer_info = std::make_shared<TimerInfo>();
    std::weak_ptr<TimerInfo> weak_info(timer_info);
    if (timeout_ms != ~0ull) {
        timer = ioManager->addConditionTimer(timeout_ms, [weak_info, fd, ioManager]{
            std::shared_ptr<TimerInfo> tf = weak_info.lock();
            if (!tf || tf->cancelled) {
                return;
            }
            tf->cancelled = ETIMEDOUT;
            ioManager->cancelEvent(fd, sylar::IOManager::WRITE);
        }, weak_info);
    }
    int err = ioManager->addEvent(fd, sylar::IOManager::WRITE);
    if (err == -1) {
        SYLAR_LOG_ERROR(g_logger) << "connect addEvent(" << fd << ", WRITE) error";
        if(timer) {
            ioManager->cancel(timer);
        }
    } else {
        sylar::Fiber::YeildToHold();
        if (timer) {
            ioManager->cancel(timer);
        }
        if (timer_info->cancelled) {
            errno = timer_info->cancelled;
            return -1;
        }
    }
    int error = 0;
    socklen_t len = sizeof(error);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if(!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

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

int socket(int domain, int type, int protocol) {
    if (sylar::is_hook_enable()) {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if (fd == -1) {
        return fd;
    }
    sylar::FdManager::GetInstance()->get(fd, true);
    return fd;
}
}