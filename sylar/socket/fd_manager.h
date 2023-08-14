#pragma once

#include <memory>
#include <stdint.h>
#include <vector>

#include "mutex.h"
#include "singleton.h"

namespace sylar {
class FdCtx {
public:
    typedef std::shared_ptr<FdCtx> ptr;

    FdCtx(int fd);
    ~FdCtx();
    bool isInit() const { return m_isInit; }
    bool isSocket() const { return m_isSocket; }
    bool isClose() const { return m_isClose; }
    void setUserNonblock(bool flag) { m_userNonblock = flag; }
    bool getUserNonblock() { return m_userNonblock; }
    void setSysNonblock(bool flag) { m_sysNonblock = flag; }
    bool getSysNonblock() { return m_sysNonblock; }
    void setTimeout(int type, uint64_t v);
    uint64_t getTimeot(int type);

private:
    bool init();

private:
    bool m_isInit: 1;
    bool m_isSocket: 1;
    bool m_sysNonblock: 1;
    bool m_userNonblock: 1;
    bool m_isClose: 1;
    int m_fd;
    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;
};

class FdManager {
friend class Singleton<FdManager>;
public:
    typedef RWMutex RWMutexType;

    static FdManager* GetInstance();
    FdCtx::ptr get(int fd, bool auto_creat = false);
    void del(int fd);

private:
    FdManager();

private:
    RWMutexType m_mutex;
    std::vector<FdCtx::ptr> m_datas;
};
}