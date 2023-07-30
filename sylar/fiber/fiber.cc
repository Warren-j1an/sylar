#include "config.h"
#include "fiber.h"
#include "log.h"
#include "util.h"

#include <atomic>

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");
static std::atomic<uint64_t> s_fiber_id = 0;
static std::atomic<uint64_t> s_fiber_count = 0;

thread_local Fiber *t_fiber = nullptr;              // current fiber
thread_local Fiber::ptr t_threadFiber = nullptr;    // the first fiber (main fiber)

Fiber::Fiber() : m_id(0), m_stackSize(0), m_stack(nullptr){
    m_state = EXEC;
    SetThis(this);
    SYLAR_ASSERT(!getcontext(&m_ctx));
    ++s_fiber_count;
}

Fiber::Fiber(std::function<void()> cb, size_t stackSize, bool use_caller)
    : m_id(++s_fiber_id), m_state(INIT) {
    ++s_fiber_count;
    m_stackSize = stackSize ? stackSize : g_fiber_stack_size->getValue();
    m_stack = malloc(m_stackSize);
    SYLAR_ASSERT(!getcontext(&m_ctx));
    m_ctx.uc_link = nullptr;                // The context of associated Fiber
    m_ctx.uc_stack.ss_sp = m_stack;         // The pointer to stack of this Fiber
    m_ctx.uc_stack.ss_size = m_stackSize;   // The size of this Fiber's stack

    if (use_caller) {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    }
}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        SYLAR_ASSERT(m_state == TERM || m_state == EXECPT || m_state == INIT);
        free(m_stack);
    } else {
        SYLAR_ASSERT(!m_cb && m_state == EXEC);
        if (t_fiber == this) {
            SetThis(nullptr);
        }
    }
}

void Fiber::reset(std::function<void()> cb) {
    SYLAR_ASSERT(m_stack);
    SYLAR_ASSERT(m_state == TERM || m_state == EXECPT || m_state == INIT);
    m_cb = cb;
    SYLAR_ASSERT(!getcontext(&m_ctx));
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::swapIn() {
    SetThis(this);
    SYLAR_ASSERT(m_state != EXEC);
    m_state = EXEC;
    SYLAR_ASSERT(!swapcontext(&t_threadFiber->m_ctx, &m_ctx));
}

void Fiber::swapOut() {
    SetThis(t_threadFiber.get());
    SYLAR_ASSERT(!swapcontext(&m_ctx, &t_threadFiber->m_ctx));
}

void Fiber::call() {
    SetThis(this);
    SYLAR_ASSERT(m_state != EXEC);
    m_state = EXEC;
    SYLAR_ASSERT(!swapcontext(&t_threadFiber->m_ctx, &m_ctx));
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    SYLAR_ASSERT(!swapcontext(&m_ctx, &t_threadFiber->m_ctx));
}

void Fiber::SetThis(Fiber *f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if (!t_fiber) {
        Fiber::ptr main_fiber = std::make_shared<Fiber>();
        SYLAR_ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
    }
}

void Fiber::YeildToReady() {

}

void Fiber::YeildToHold() {

}

void Fiber::MainFunc() {

}

void Fiber::CallerMainFunc() {

}

uint64_t Fiber::GetFiberID() {

}

uint64_t Fiber::TotalFibers() {
    
}
}