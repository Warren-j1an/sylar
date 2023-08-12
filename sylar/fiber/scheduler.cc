#include "log.h"
#include "scheduler.h"
#include "util.h"

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

void FiberAndFunction::reset() {
    fiber = nullptr;
    cb = nullptr;
    thread = -1;
}

Scheduler::Scheduler(size_t thread, bool use_caller, const std::string& name) : m_name(name) {
    SYLAR_ASSERT(thread > 0);
    if (use_caller) {
        Fiber::GetThis();
        --thread;
        SYLAR_ASSERT(!t_scheduler);
        t_scheduler = this;
        m_callerFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        t_fiber = m_callerFiber.get();
        m_rootThread = GetThreadID();
        m_threadIDs.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    Thread::SetName(m_name);
    m_threadCount = thread;
}

Scheduler::~Scheduler() {
    SYLAR_ASSERT(stopping());
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }
    m_stopping = false;
    SYLAR_ASSERT(m_threads.empty());
    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "-" + std::to_string(i)));
        m_threadIDs.push_back(m_threads[i]->getID());
    }
}

void Scheduler::stop() {
    // m_autoStop = true;
    /* if (m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::INIT ||
        m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::EXECPT)) {
        SYLAR_LOG_INFO(g_logger) << "stopped";
        m_stopping = true;
        if (stopping()) {
            return;
        }
    } */
    m_stopping = true;
    if (m_rootThread != -1) {
        SYLAR_ASSERT(GetThis() == this);
    } else {
        SYLAR_ASSERT(GetThis() == nullptr);
    }
    for (size_t i = 0; i < m_threadCount; i++) {
        tickle();
    }
    if (m_callerFiber) {
        tickle();
        m_callerFiber->swapIn();
    }
    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }
    for (auto& i : thrs) {
        i->join();
    }
}

void Scheduler::switchTo(pid_t thread) {
    SYLAR_ASSERT(Scheduler::GetThis() != nullptr);
    if (Scheduler::GetThis() == this) {
        if (thread == -1 || thread == GetThreadID()) {
            return;
        }
    }
    schedule(Fiber::GetThis(), thread);
    Fiber::YeildToHold();
}

std::ostream& Scheduler::dump(std::ostream& os) {
    os << "[Scheduler name=" << m_name
       << " size=" << m_threadCount
       << " active_count=" << m_activeThreadCount
       << " idle_count=" << m_idleThreadCount
       << " stopping=" << m_stopping
       << " ]" << std::endl << "    ";
    for(size_t i = 0; i < m_threadIDs.size(); ++i) {
        if(i) {
            os << ", ";
        }
        os << m_threadIDs[i];
    }
    return os;
}

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::run() {
    SYLAR_LOG_INFO(g_logger) << m_name << " run";
    setThis();
    if (GetThreadID() != m_rootThread) {
        t_fiber = Fiber::GetThis().get();
    }
    Fiber::ptr idle_fiber = std::make_shared<Fiber>(std::bind(&Scheduler::idle, this));
    Fiber::ptr cb_fiber;
    FiberAndFunction fiberfunc;
    while (true) {
        fiberfunc.reset();
        bool tickle_me = false;
        {
            MutexType::Lock lock(m_mutex);
            for (auto it = m_fibers.begin(); it != m_fibers.end(); ++it) {
                if (it->thread != -1 && it->thread != GetThreadID()) {
                    tickle_me = true;
                    continue;
                }
                SYLAR_ASSERT(it->fiber || it->cb);
                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    continue;
                }
                fiberfunc = *it;
                m_fibers.erase(it);
                m_activeThreadCount += 1;
                break;
            }
            tickle_me |= !m_fibers.empty();
        }
        if (tickle_me) {
            tickle();
        }
        if (fiberfunc.fiber && fiberfunc.fiber->getState() != Fiber::TERM
                            && fiberfunc.fiber->getState() != Fiber::EXECPT) {
            fiberfunc.fiber->swapIn();
            m_activeThreadCount -= 1;
            if (fiberfunc.fiber->getState() == Fiber::READY) {
                schedule(fiberfunc.fiber);
            }
            fiberfunc.reset();
        } else if (fiberfunc.cb) {
            if (cb_fiber) {
                cb_fiber->reset(fiberfunc.cb);
            } else {
                cb_fiber = std::make_shared<Fiber>(fiberfunc.cb);
            }
            fiberfunc.reset();
            cb_fiber->swapIn();
            m_activeThreadCount -= 1;
            if (cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if (cb_fiber->getState() == Fiber::EXECPT || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber.reset();
            }
        } else {
            if (idle_fiber->getState() == Fiber::TERM) {
                SYLAR_LOG_INFO(g_logger) << "idle fiber term";
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
        }
    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

void Scheduler::tickle() {
    SYLAR_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    SYLAR_LOG_INFO(g_logger) << "idle";
    while (!stopping()) {
        Fiber::YeildToHold();
    }
}
}