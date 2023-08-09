#include "timer.h"
#include "util.h"

namespace sylar {
bool Timer::Compare::operator()(const Timer::ptr lhs, const Timer::ptr rhs) const {
    if (!rhs) return false;
    if (!lhs) return true;
    if (lhs->m_next < rhs->m_next) return true;
    if (lhs->m_next > rhs->m_next) return false;
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring)
    : m_recurring(recurring), m_ms(ms), m_cb(cb) {
    m_next = GetCurrentMS() + m_ms;
}

Timer::Timer(uint64_t next) : m_next(next) {}

TimerManager::TimerManager() {
    m_previousTime = GetCurrentMS();
}

TimerManager::~TimerManager() {}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring));
    addTimer(timer);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> temp = weak_cond.lock();
    if (temp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb,
                                           std::weak_ptr<void> weak_cond, bool recurring) {
    return addTimer(ms, std::bind(OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = false;
    if (m_timers.empty()) {
        return ~0ull;
    }
    const Timer::ptr next = *m_timers.begin();
    uint64_t now_ms = GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()>>& cbs) {
    uint64_t now_ms = GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lcok(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }
    RWMutexType::WriteLock lock(m_mutex);
    if (m_timers.empty()) {
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }
    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());
    for (Timer::ptr timer : expired) {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}

bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

bool TimerManager::cancel(Timer::ptr timer) {
    RWMutexType::WriteLock lock(m_mutex);
    if (!timer->m_cb) {
        return false;
    }
    timer->m_cb = nullptr;
    auto it = m_timers.find(timer);
    if (it == m_timers.end()) {
        return false;
    }
    m_timers.erase(it);
    return true;
}

bool TimerManager::refresh(Timer::ptr timer) {
    RWMutexType::WriteLock lock(m_mutex);
    if (!timer->m_cb) {
        return false;
    }
    auto it = m_timers.find(timer);
    if (it == m_timers.end()) {
        return false;
    }
    m_timers.erase(it);
    timer->m_next = GetCurrentMS() + timer->m_ms;
    addTimer(timer);
    return true;
}

bool TimerManager::reset(Timer::ptr timer, uint64_t ms, bool from_now) {
    RWMutexType::WriteLock lock(m_mutex);
    if (timer->m_ms == ms && !from_now) {
        return true;
    }
    if (!timer->m_cb) {
        return false;
    }
    auto it = m_timers.find(timer);
    if (it == m_timers.end()) {
        return false;
    }
    m_timers.erase(it);
    uint64_t start = 0;
    if (from_now) {
        start = GetCurrentMS();
    } else {
        start = timer->m_next - timer->m_ms;
    }
    timer->m_ms = ms;
    timer->m_next = start + ms;
    addTimer(timer);
    return true;
}

void TimerManager::addTimer(Timer::ptr val) {
    RWMutexType::WriteLock lock(m_mutex);
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if(at_front) {
        m_tickled = true;
    }
    lock.unlock();
    if(at_front) {
        onTimerInsertedAtFront();
    }
}

bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}
}