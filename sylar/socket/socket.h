#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "address.h"
#include "noncopyable.h"

namespace sylar {
class Socket : public Noncopyable {
public:
    typedef std::shared_ptr<Socket> ptr;

    enum Type {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM,
    };

    enum Family {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX,
    };

    static Socket::ptr CreateTCP(Address::ptr address);
    static Socket::ptr CreateUDP(Address::ptr address);
    static Socket::ptr CreateTCPSocket();
    static Socket::ptr CreateUDPSocket();
    static Socket::ptr CreateTCPSocket6();
    static Socket::ptr CreateUDPSocket();
    static Socket::ptr CreateUnixTCPSocket();
    static Socket::ptr CreateUnixUDPSocket();

    Socket(int family, int type, int protocol = 0);
    virtual ~Socket();
    uint64_t getSendTimeout();
    void setSendTimeout(uint64_t v);
    uint64_t getRecvTimeout();
    void setRecvTimeout(uint64_t v);
    bool getOption(int level, int option, void* result, socklen_t* len);
    bool setOption(int level, int option, const void* value, socklen_t len);
    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();
    int getFamily() const { return m_family; }
    int getType() const { return m_type; }
    int getProtocol() const { return m_protocol; }
    bool isConnected() const { return m_isConnected; }
    int getSocket() const { return m_socket; }
    bool isValid() const;
    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();
    int getError();

    template<typename T>
    bool getOption(int level, int option, T& result) {
        socklen_t length = sizeof(T);
        return getprotoent(level, option, &result, &length);
    }

    template<typename T>
    bool setOption(int level, int option, const T& value) {
        return setprotoent(level, option, &value, sizeof(T));
    }

    virtual Socket::ptr accept();
    virtual bool bind(const Address::ptr addres);
    virtual bool connect(const Address::ptr addres, uint64_t timeout_ms = -1);
    virtual bool reconnect(uint64_t timeout_ms = -1);
    virtual bool listen(int backlog = SOMAXCONN);
    virtual int send(const void* buffer, size_t length, int flags = 0);
    virtual int send(const iovec* buffers, size_t length, int flags = 0);
    virtual int sendTo(const void* buffer, size_t length, const Address::ptr to, int flags = 0);
    virtual int sendTo(const iovec* buffers, size_t length, const Address::ptr to, int flags = 0);
    virtual int recv(void* buffer, size_t length, int flags = 0);
    virtual int recv(iovec* buffers, size_t length, int flags = 0);
    virtual int recvFrom(void* buffer, size_t length, const Address::ptr from, int flags = 0);
    virtual int recvFrom(iovec* buffers, size_t length, const Address::ptr from, int flags = 0);
    virtual std::ostream& dump(std::ostream& os) const;
    virtual std::string toString() const;

protected:
    void initSocket();
    void newSock();

    virtual bool init(int sock);

protected:
    int m_socket;
    int m_family;
    int m_type;
    int m_protocol;
    bool m_isConnected;
    Address::ptr m_localAddress;
    Address::ptr m_remoteAddress;
};
}