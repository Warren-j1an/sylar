#pragma once

#include <atomic>
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>

#include "noncopyable.h"

namespace sylar {
class Semaphore : Noncopyable{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();

private:
    sem_t m_semaphore;
};

template<typename T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_locked = false;
            m_mutex.unlock();
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template<typename T>
class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_locked = false;
            m_mutex.unlock();
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template<typename T>
class WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_locked = false;
            m_mutex.unlock();
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

class Mutex : Noncopyable {
public:
    typedef ScopedLockImpl<Mutex> Lock;

    Mutex();
    ~Mutex();
    void lock();
    void unlock();

private:
    pthread_mutex_t m_mutex;
};

class RWMutex : Noncopyable {
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex();
    ~RWMutex();
    void rdlock();
    void wrlock();
    void unlock();

private:
    pthread_rwlock_t m_lock;
};

class Spinlock : Noncopyable {
public:
    typedef ScopedLockImpl<Spinlock> Lock;

    Spinlock();
    ~Spinlock();
    void lock();
    void unlock();

private:
    pthread_spinlock_t m_lock;
};

class CASLock : Noncopyable {
public:
    typedef ScopedLockImpl<CASLock> Lock;

    CASLock();
    ~CASLock();
    void lock();
    void unlock();

private:
    volatile std::atomic_flag m_mutex;
};

class NullMutex : Noncopyable{
public:
    typedef ScopedLockImpl<NullMutex> Lock;

    NullMutex() {}

    ~NullMutex() {}

    void lock() {}

    void unlock() {}
};
}