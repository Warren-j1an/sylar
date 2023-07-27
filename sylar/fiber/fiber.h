#pragma once

#include <functional>
#include <stdint.h>
#include <ucontext.h>

namespace sylar {
class Fiber {
public:
    enum State { 
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXECPT 
    };

    Fiber(std::function<void()> cb, size_t stackSize = 0, bool use_caller = false);
    ~Fiber();
    void reset(std::function<void()> cb);
    void swapIn();
    void swapOut();
    void call();
    void back();
    uint64_t getID() const { return m_id; }
    State getState() const { return m_state; }

    static void SetThis(Fiber *f);
    static void YeildToReady();
    static void YeildToHold();
    static void MainFunc();
    static void CallerMainFunc();
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