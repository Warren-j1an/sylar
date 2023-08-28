#include "log.h"
#include "socket.h"

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");

Socket::ptr Socket::CreateTCP(Address::ptr address) {
    Socket::ptr sock = std::make_shared<Socket>(address->getFamily(), TCP, 0);
    return sock;
}

Socket::ptr Socket::CreateUDP(Address::ptr address) {
    Socket::ptr sock = std::make_shared<Socket>(address->getFamily(), UDP, 0);
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::ptr Socket::CreateTCPSocket() {
    Socket::ptr sock = std::make_shared<Socket>(IPv4, TCP, 0);
    return sock;
}

Socket::ptr Socket::CreateUDPSocket() {
    Socket::ptr sock = std::make_shared<Socket>(IPv4, UDP, 0);
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::ptr Socket::CreateTCPSocket6() {
    Socket::ptr sock = std::make_shared<Socket>(IPv6, TCP, 0);
    return sock;
}

Socket::ptr Socket::CreateUDPSocket() {
    Socket::ptr sock = std::make_shared<Socket>(IPv6, UDP, 0);
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::ptr Socket::CreateUnixTCPSocket() {
    Socket::ptr sock = std::make_shared<Socket>(UNIX, TCP, 0);
    return sock;
}

Socket::ptr Socket::CreateUnixUDPSocket() {
    Socket::ptr sock = std::make_shared<Socket>(UNIX, UDP, 0);
    return sock;
}

Socket::Socket(int family, int type, int protocol)
    : m_socket(-1), m_family(family), m_type(type)
    , m_protocol(protocol), m_isConnected(false) {}


}