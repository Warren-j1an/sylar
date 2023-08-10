#include "iomanager.h"
#include "log.h"
#include "util.h"

#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");
static const uint64_t MAX_EVENT = 256;
static const int MAX_TIMEOUT = 3000;

enum EpollCtlOp {

};

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(Event event) {
    switch (event) {
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return write;
        default:
            SYLAR_ASSERT(false);
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx) {
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(Event event) {
    if (!(events & event)) {
        SYLAR_LOG_ERROR(g_logger) << "fd=" << fd << " triggerEvent event=" << event << " events="
            << events << "\nbacktrace:\n" << BacktraceToString(100, 2, "    ");
        return;
    }
    events = (Event)(events & ~event);
    EventContext& ctx = getContext(event);
    if (ctx.cb) {
        ctx.scheduler->schedule(&ctx.cb);
    } else {
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
}

IOManager::IOManager(size_t thread, bool use_caller, const std::string& name)
    : Scheduler(thread, use_caller, name) {
    m_epfd = epoll_create(1);
    SYLAR_ASSERT(m_epfd != -1);

    int rt = pipe(m_tickleFds);
    SYLAR_ASSERT(rt != -1);

    epoll_event event;
    // memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    SYLAR_ASSERT(rt != -1);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    SYLAR_ASSERT(rt != -1);

    contextResize(32);
    start();
}

IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock_r(m_mutex);
    if ((int)m_fdContexts.size() > fd) {
        fd_ctx = m_fdContexts[fd];
        lock_r.unlock();
    } else {
        lock_r.unlock();
        RWMutexType::WriteLock lock_w(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    FdContext::MutexType::Lock lock(fd_ctx->mutex);
    if (fd_ctx->events & event) {
        SYLAR_LOG_ERROR(g_logger) << "addEvent assert fd=" << fd << " event="
            << (EPOLL_EVENTS)event << " fd_ctx.event=" << (EPOLL_EVENTS)fd_ctx->events;
        SYLAR_ASSERT(!(fd_ctx->events & event));
    }
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event ep_event;
    ep_event.events = EPOLLET | fd_ctx->events | event;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if (rt == -1) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op  << ", "
            << fd << ", " << (EPOLL_EVENTS)ep_event.events << "):" << rt << " (" << errno
            << ")(" << strerror(errno) << ") fd_ctx->envents=" << (EPOLL_EVENTS)fd_ctx->events;
        return -1;
    }
    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    SYLAR_ASSERT(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
    event_ctx.scheduler = Scheduler::GetThis();
    if (cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::GetThis();
        SYLAR_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
    RWMutexType::ReadLock lock_r(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock_r.unlock();
    FdContext::MutexType::Lock lock(fd_ctx->mutex);
    if (!(fd_ctx->events & event)) {
        return false;
    }
    Event new_event = (Event)(fd_ctx->events & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if (rt == -1) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op  << ", "
            << fd << ", " << (EPOLL_EVENTS)ep_event.events << "):" << rt << " (" << errno
            << ")(" << strerror(errno) << ") fd_ctx->envents=" << (EPOLL_EVENTS)fd_ctx->events;
        return false;
    }
    --m_pendingEventCount;
    fd_ctx->events = new_event;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
    RWMutexType::ReadLock lock_r(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock_r.unlock();
    FdContext::MutexType::Lock lock(fd_ctx->mutex);
    if (!(fd_ctx->events & event)) {
        return false;
    }
    Event new_event = (Event)(fd_ctx->events & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if (rt == -1) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op  << ", "
            << fd << ", " << (EPOLL_EVENTS)ep_event.events << "):" << rt << " (" << errno
            << ")(" << strerror(errno) << ") fd_ctx->envents=" << (EPOLL_EVENTS)fd_ctx->events;
        return false;
    }
    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd) {
    RWMutexType::ReadLock lock_r(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock_r.unlock();
    FdContext::MutexType::Lock lock(fd_ctx->mutex);
    if (!fd_ctx->events) {
        return false;
    }
    int op = EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.events = Event::NONE;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);
    if (rt == -1) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op  << ", "
            << fd << ", " << (EPOLL_EVENTS)ep_event.events << "):" << rt << " (" << errno
            << ")(" << strerror(errno) << ") fd_ctx->envents=" << (EPOLL_EVENTS)fd_ctx->events;
        return false;
    }
    if (fd_ctx->events & Event::READ) {
        fd_ctx->triggerEvent(Event::READ);
        --m_pendingEventCount;
    }
    if (fd_ctx->events & Event::WRITE) {
        fd_ctx->triggerEvent(Event::WRITE);
        --m_pendingEventCount;
    }
    SYLAR_ASSERT(fd_ctx->events == Event::NONE);
    return true;
}

IOManager* IOManager::GetThis() {
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

void IOManager::tickle() {
    if (!hasIdleThreads()) {
        return;
    }
    int rt = write(m_tickleFds[1], "T", 1);
    SYLAR_ASSERT(rt == 1);
}

bool IOManager::stopping() {
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IOManager::idle() {
    SYLAR_LOG_INFO(g_logger) << "idle";
    epoll_event* events = new epoll_event[MAX_EVENT];
    std::shared_ptr<epoll_event> shared_event(events, [](epoll_event* ptr){
        delete[] ptr;
    });

    while (true) {
        uint64_t next_timeout = 0;
        if (stopping(next_timeout)) {
            SYLAR_LOG_INFO(g_logger) << "name=" << getName() << " idle stopping exit";
            break;
        }
        int event_num;
        do {
            if (next_timeout != ~0ull) {
                next_timeout = (int)next_timeout > MAX_TIMEOUT ? MAX_TIMEOUT : next_timeout;
            } else {
                next_timeout = MAX_TIMEOUT;
            }
            event_num = epoll_wait(m_epfd, events, MAX_EVENT, next_timeout);
            if (event_num >= 0 || errno == EINTR) {
                break;
            }
        } while (true);
        // timer callbacks
        std::vector<std::function<void()>> cbs;
        listExpiredCb(cbs);
        if (!cbs.empty()) {
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }
        // epoll event
        for (int i = 0; i < event_num; ++i) {
            epoll_event& event = events[i];
            if (event.data.fd == m_tickleFds[0]) {
                uint8_t dummy[256];
                while(read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
                continue;
            }
            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            if (event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_event = NONE;
            if (event.events & EPOLLIN) {
                real_event |= EPOLLIN;
            }
            if (event.events & EPOLLOUT) {
                real_event |= EPOLLOUT;
            }
            if ((fd_ctx->events & real_event) == NONE) {
                continue;
            }
            int left_event = fd_ctx->events & ~real_event;
            int op = left_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_event;
            int rt = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if (rt == -1) {
                SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op  << ", "
                    << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):" << rt << " (" << errno
                    << ")(" << strerror(errno) << ") fd_ctx->envents=" << (EPOLL_EVENTS)fd_ctx->events;
                continue;
            }
            if (real_event & Event::READ) {
                fd_ctx->triggerEvent(Event::READ);
                --m_pendingEventCount;
            }
            if (real_event & Event::WRITE) {
                fd_ctx->triggerEvent(Event::WRITE);
                --m_pendingEventCount;
            }
        }
        Fiber::YeildToHold();
    }
}

void IOManager::onTimerInsertedAtFront() {
    tickle();
}

void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);
    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

bool IOManager::stopping(uint64_t& timeout) {
    timeout = getNextTimer();
    return timeout == ~0ull && m_pendingEventCount == 0 && Scheduler::stopping();
}
}