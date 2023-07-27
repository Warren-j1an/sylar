#pragma once 

#include <functional>
#include <memory>

#include "mutex.h"

namespace sylar {
class Thread : Noncopyable {
public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();
    void join();
    pid_t getID() const { return m_id; }
    const std::string& getName() { return m_name; }
    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);

private:
    static void* run(void* arg);

private:
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_cb;
    std::string m_name;
    Semaphore m_semaphore;
};
}