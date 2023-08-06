#pragma once

#include <functional>
#include <memory>
#include <set>
#include <stdint.h>
#include <vector>

#include "mutex.h"

namespace sylar {
class Timer {
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring);
    Timer(uint64_t next);

    struct Compare {
        bool operator()(const Timer::ptr lhs, const Timer::ptr rhs) const;
    };

private:
    bool m_recurring = false;
    uint64_t m_ms = 0;
    uint64_t m_next = 0;
    std::function<void()> m_cb;
};

class TimerManager {
public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, 
                                 std::weak_ptr<void> weak_cond, bool recurring = false);
    uint64_t getNextTimer();
    void listExpiredCb(std::vector<std::function<void()>>& cbs);
    bool hasTimer();
    bool cancel(Timer::ptr timer);
    bool refresh(Timer::ptr timer);
    bool reset(Timer::ptr timer, uint64_t ms, bool from_now);

protected:
    void addTimer(Timer::ptr val);
    bool detectClockRollover(uint64_t now_ms);
    virtual void onTimerInsertedAtFront() = 0;

private:
    RWMutexType m_mutex;
    std::set<Timer::ptr, Timer::Compare> m_timers;
    bool m_tickled = false;
    uint64_t m_previousTime = 0;
};
}