#pragma once

#include <atomic>
#include <stdint.h>
#include <vector>

#include "mutex.h"
#include "scheduler.h"
#include "timer.h"

namespace sylar {

class IOManager : public Scheduler, public TimerManager {
public:
    typedef RWMutex RWMutexType;

    enum Event {
        NONE  = 0x0,
        READ  = 0x1,
        WRITE = 0x4,
    };

private:
    struct FdContext {
        typedef Spinlock MutexType;

        struct EventContext {
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber = nullptr;
            std::function<void()> cb = nullptr;
        };

        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggerEvent(Event event);

        EventContext read;
        EventContext write;
        int fd = 0;
        Event events = NONE;
        MutexType mutex;
    };

public:
    IOManager(size_t thread = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();
    int addEvent(int fd, Event event, std::function<void()> cb);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);
    bool cancelAll(int fd);

    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertedAtFront() override;
    void contextResize(size_t size);
    bool stopping(uint64_t& timeout);

private:
    int m_epfd = 0;
    int m_tickleFds[2];
    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;
};
}