#pragma once

#include <functional>
#include <memory>
#include <stdint.h>
#include <ucontext.h>

namespace sylar {
class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    enum State { 
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXECPT 
    };
    typedef std::shared_ptr<Fiber> ptr;

    Fiber(std::function<void()> cb, size_t stackSize = 0);
    ~Fiber();
    void reset(std::function<void()> cb);
    void swapIn();
    void swapOut();
    uint64_t getID() const { return m_id; }
    State getState() const { return m_state; }

    static void SetThis(Fiber *f);
    static Fiber::ptr GetThis();
    static void YeildToReady();
    static void YeildToHold();
    static void MainFunc();
    static uint64_t GetFiberID();
    static uint64_t TotalFibers();

private:
    Fiber();

private:
    uint64_t m_id;
    uint32_t m_stackSize;
    Fiber::State m_state;
    ucontext_t m_ctx;
    void *m_stack;
    std::function<void()> m_cb;
};
}