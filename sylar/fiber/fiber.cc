#include "config.h"
#include "fiber.h"
#include "log.h"
#include "scheduler.h"
#include "util.h"

#include <atomic>

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");
static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber *t_fiber = nullptr;              // current fiber
static thread_local Fiber::ptr t_threadFiber = nullptr;    // the first fiber (main fiber)

Fiber::Fiber() : m_id(0), m_stackSize(0), m_stack(nullptr){
    m_state = EXEC;
    SetThis(this);
    SYLAR_ASSERT(!getcontext(&m_ctx));
    ++s_fiber_count;
}

Fiber::Fiber(std::function<void()> cb, size_t stackSize, bool use_caller)
    : m_id(++s_fiber_id), m_useCaller(use_caller), m_state(INIT), m_cb(cb) {
    ++s_fiber_count;
    m_stackSize = stackSize ? stackSize : g_fiber_stack_size->getValue();
    m_stack = malloc(m_stackSize);
    SYLAR_ASSERT(!getcontext(&m_ctx));
    m_ctx.uc_link = nullptr;                // The context of associated Fiber
    m_ctx.uc_stack.ss_sp = m_stack;         // The pointer to stack of this Fiber
    m_ctx.uc_stack.ss_size = m_stackSize;   // The size of this Fiber's stack
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
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
    if (m_useCaller) {
        SYLAR_ASSERT(!swapcontext(&t_threadFiber->m_ctx, &m_ctx));
    } else {
        SYLAR_ASSERT(!swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx));
    }
}

void Fiber::swapOut() {
    SetThis(t_threadFiber.get());
    if (m_useCaller) {
        SYLAR_ASSERT(!swapcontext(&m_ctx, &t_threadFiber->m_ctx));
    } else {
        SYLAR_ASSERT(!swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx));
    }
}

void Fiber::SetThis(Fiber *f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if (!t_fiber) {
        Fiber::ptr main_fiber(new Fiber);
        SYLAR_ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
    }
    return t_fiber->shared_from_this();
}

void Fiber::YeildToReady() {
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YeildToHold() {
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::GetFiberID() {
    if (t_fiber) {
        return t_fiber->getID();
    }
    return 0;
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch(const std::exception& e)
    {
        cur->m_state = EXECPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << e.what() << " fiber id="
                                  << cur->getID() << std::endl << BacktraceToString();
    }
    Fiber *raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    SYLAR_LOG_ERROR(g_logger) << "Never reach fiber_id=" << std::to_string(raw_ptr->getID());
    SYLAR_ASSERT(false);
}
}