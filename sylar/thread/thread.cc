#include "log.h"
#include "thread.h"
#include "util.h"

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");

thread_local Thread* t_thread = nullptr;
thread_local std::string t_thread_name = "UNKNOW";

Thread* Thread::GetThis() {
    return t_thread;
}

const std::string& Thread::GetName() {
    return t_thread_name;
}

void Thread::SetName(const std::string& name) {
    t_thread_name = name;
}

void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = GetThreadID();
    pthread_setname_np(thread->m_thread, thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();

    cb();
    return 0;
}

Thread::Thread(std::function<void()> cb, const std::string& name) : 
    m_id(-1), m_thread(0), m_cb(cb), m_name(name), m_semaphore(0) {
    if (m_name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        SYLAR_LOG_ERROR(g_logger) << "pthread_creat thread fail, rt=" << rt << " name=" << m_name;
        throw std::logic_error("pthread_creat error");
    }
    m_semaphore.wait();
}

Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}
}