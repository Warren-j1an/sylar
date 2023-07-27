#include "util.h"

#include <syscall.h>
#include <unistd.h>

namespace sylar {
pid_t GetThreadID() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberID() {
    return 0;
}
}