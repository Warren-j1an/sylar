#pragma once

#include <memory>
#include <string>
#include <vector>

#include "fiber.h"
#include "mutex.h"
#include "thread.h"

namespace sylar {
struct FiberAndFunction {
    Fiber::ptr fiber;
    std::function<void()> cb;
    pid_t thread;

    FiberAndFunction(Fiber::ptr f, pid_t thr) : fiber(f), thread(thr) {}
    FiberAndFunction(Fiber::ptr* f, pid_t thr) : thread(thr) { fiber.swap(*f); }
    FiberAndFunction(std::function<void()> f, pid_t thr) : cb(f), thread(thr) {}
    FiberAndFunction(std::function<void()>* f, pid_t thr) : thread(thr) { cb.swap(*f); }
    FiberAndFunction() : thread(-1) {}
    void reset();
};

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;
    
    Scheduler(size_t thread = 1, bool use_caller = false, const std::string& name = "");
    virtual ~Scheduler();
    void start();
    void stop();
    const std::string& getName() const { return m_name; }
    static Scheduler *GetThis();
    static Fiber *GetMainFiber();

protected:
    void run();
    void setThis();
    virtual void tickle();
    virtual bool stopping();
    virtual void idle();
    bool hasIdleThreads() { return m_idleThreadCount > 0; }

public:
    template<typename FiberOrFunc>
    void schedule(FiberOrFunc fc, pid_t thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if (need_tickle) {
            tickle();
        }
    }

    template<typename InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end) {
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                ++begin;
            }
        }
        if (need_tickle) {
            tickle();
        }
    }

private:
    template<typename FiberOrFunc>
    bool scheduleNoLock(FiberOrFunc fc, pid_t thread) {
        bool need_tickle = m_fibers.empty();
            FiberAndFunction ft(fc, thread);
            if(ft.fiber || ft.cb) {
                m_fibers.push_back(ft);
            }
            return need_tickle;
    }

private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;         // thread pool
    std::vector<FiberAndFunction> m_fibers;     // function is same as fiber to use
    Fiber::ptr m_callerFiber;
    std::string m_name;

protected:
    std::vector<pid_t> m_threadIDs;
    size_t m_threadCount {0};
    std::atomic<size_t> m_activeThreadCount {0};
    std::atomic<size_t> m_idleThreadCount {0};
    bool m_autoStop = false;
    bool m_stopping = true;
    pid_t m_rootThread;
};
}