#include "iomanager.h"
#include "log.h"
#include "util.h"

#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");

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
    events = (Event)(event & ~event);
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
}