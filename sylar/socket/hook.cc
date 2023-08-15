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

#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX

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
    sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd);
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
    sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd);
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
    if (!sylar::is_hook_enable()) {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if (fd == -1) {
        return fd;
    }
    sylar::FdManager::GetInstance()->get(fd, true);
    return fd;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    connect_with_timeout(sockfd, addr, addrlen, s_connect_timeout);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = do_io(sockfd, accept_f, "accept", sylar::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0 && sylar::is_hook_enable()) {
        sylar::FdManager::GetInstance()->get(fd, true);
    }
    return fd;
}

int close(int fd) {
    if (!sylar::is_hook_enable()) {
        return close_f(fd);
    }
    sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd);
    if (ctx) {
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        if (iom) {
            iom->cancelAll(fd);
        }
        sylar::FdManager::GetInstance()->del(fd);
    }
    return close_f(fd);
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", sylar::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", sylar::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", sylar::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", sylar::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", sylar::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", sylar::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", sylar::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return do_io(sockfd, send_f, "send", sylar::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    return do_io(sockfd, sendto_f, "sendto", sylar::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return do_io(sockfd, sendmsg_f, "sendmsg", sylar::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

int fcntl(int fd, int cmd, ... /* arg */ ) {
    va_list va;
    va_start(va, cmd);
    switch (cmd) {
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);
                if(ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK;
                } else {
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_f(fd, cmd, arg);
            }
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return arg;
                }
                if(ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;
                } else {
                    return arg & ~O_NONBLOCK;
                }
            }
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

int ioctl(int fd, unsigned long request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);
    if (FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        sylar::FdCtx::ptr ctx = sylar::FdManager::GetInstance()->get(fd);
        if(!ctx || ctx->isClose() || !ctx->isSocket()) {
            return ioctl_f(fd, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(fd, request, arg);
}
}