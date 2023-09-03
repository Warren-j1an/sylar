#pragma once

#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

namespace sylar {
    bool is_hook_enable();
    void set_hook_enable(bool flag);
}

extern "C" {
// sleep
typedef unsigned int (*sleep_fun)(unsigned int seconds);
typedef int (*usleep_fun)(useconds_t usec);
typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);

// socket
typedef int (*socket_fun)(int domain, int type, int protocol);
typedef int (*connect_fun)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*accept_fun)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*close_fun)(int fd);

// read
typedef ssize_t (*read_fun)(int fd, void *buf, size_t count);
typedef ssize_t (*readv_fun)(int fd, const struct iovec *iov, int iovcnt);
typedef ssize_t (*recv_fun)(int sockfd, void *buf, size_t len, int flags);
typedef ssize_t (*recvfrom_fun)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
typedef ssize_t (*recvmsg_fun)(int sockfd, struct msghdr *msg, int flags);

// write
typedef ssize_t (*write_fun)(int fd, const void *buf, size_t count);
typedef ssize_t (*writev_fun)(int fd, const struct iovec *iov, int iovcnt);
typedef ssize_t (*send_fun)(int sockfd, const void *buf, size_t len, int flags);
typedef ssize_t (*sendto_fun)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
typedef ssize_t (*sendmsg_fun)(int sockfd, const struct msghdr *msg, int flags);

// control
typedef int (*fcntl_fun)(int fd, int cmd, ... /* arg */ );
typedef int (*ioctl_fun)(int fd, unsigned long request, ...);
typedef int (*getsockopt_fun)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
typedef int (*setsockopt_fun)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
}

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(close) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)

#define XX(name) extern name ## _fun name ## _f;
    HOOK_FUN(XX);
#undef XX

extern int connect_with_timeout(int fd, const struct sockaddr *addr, socklen_t addrlen, uint64_t timeout_ms);
