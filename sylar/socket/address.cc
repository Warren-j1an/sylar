#include "address.h"
#include "log.h"

#include <cstring>
#include <ifaddrs.h>

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");

template<typename T>
static uint32_t CountBytes(T value) {
    uint32_t result = 0;
    for (; value; ++result) {
        value &= value - 1;
    }
    return result;
}

bool Address::Lookup(std::vector<Address::ptr>& result, const std::string& host,
                     int family, int type, int protocol) {
    addrinfo hints, *results, *next;
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    std::string node;
    const char* service = nullptr;

    if (!host.empty() && host[0] == '[') {  // host is an address for ipv6
        const char* endipv6 = (const char*)memchr(host.c_str(), ']', host.size());
        if (endipv6) {
            if (*(endipv6 + 1) == ':') {
                service = endipv6 + 2;
            }
            node = host.substr(1, endipv6 - host.c_str() - 1);
        }
    }

    if (node.empty()) { // host is an address for ipv4 which includes node and service
        service = (const char*)memchr(host.c_str(), ':', host.size());
        if (service) {
            if (!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)) {
                node = host.substr(0, service - host.c_str());
                ++service;
            }
        }
    }

    if (node.empty()) { // host only includes node
        node = host;
    }

    int rt = getaddrinfo(node.c_str(), service, &hints, &results);
    if (rt != 0) {
        SYLAR_LOG_ERROR(g_logger) << "Address::Lookup getaddrinfo(" << host << ", " << family
            << ", " << type << ") err=" << rt << " errstr=" << gai_strerror(rt);
        return false;
    }
    next = results;
    while (next) {
        result.push_back(Create(next->ai_addr, (socklen_t)next->ai_addrlen));
        next = next->ai_next;
    }
    freeaddrinfo(results);
    return !result.empty();
}

Address::ptr Address::LookupAny(const std::string& host, int family, int type, int protocol) {
    std::vector<Address::ptr> result;
    if (Lookup(result, host, family, type, protocol)) {
        return result[0];
    }
    return nullptr;
}

std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host, int family,
                                              int type, int protocol) {
    std::vector<Address::ptr> result;
    if (Lookup(result, host, family, type, protocol)) {
        for (auto& i : result) {
            IPAddress::ptr v = std::dynamic_pointer_cast<IPAddress>(i);
            if (v) {
                return v;
            }
        }
    }
    return nullptr;
}

bool Address::GetInterfaceAddresses(std::multimap<std::string, std::pair<Address::ptr, uint32_t>>& result,
                                    int family) {
    struct ifaddrs *next, *results;
    if (getifaddrs(&results) != 0) {
        SYLAR_LOG_ERROR(g_logger) << "Address::GetInterfaceAddresses getifaddrs err=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    try {
        for (next = results; next; next = next->ifa_next) {
            Address::ptr addr;
            uint32_t prefix_len = ~0u;
            if (family != AF_UNSPEC && family != next->ifa_addr->sa_family) {
                continue;
            }
            if (next->ifa_addr->sa_family == AF_INET) {
                addr = Create(next->ifa_addr, sizeof(sockaddr_in));
                uint32_t netmask = ((sockaddr_in*)next->ifa_netmask)->sin_addr.s_addr;
                prefix_len = CountBytes(netmask);
            } else if (next->ifa_addr->sa_family == AF_INET6) {
                addr = Create(next->ifa_addr, sizeof(sockaddr_in6));
                in6_addr& netmask = ((sockaddr_in6*)next->ifa_netmask)->sin6_addr;
                prefix_len = 0;
                for (int i = 0; i < 16; i++) {
                    prefix_len += CountBytes(netmask.s6_addr[i]);
                }
            }
            if (addr) {
                result.insert(std::make_pair(next->ifa_name, std::make_pair(addr, prefix_len)));
            }
        }
    } catch (...) {
        SYLAR_LOG_ERROR(g_logger) << "Address::GetInterfaceAddresses exception";
        freeifaddrs(results);
        return false;
    }
    freeifaddrs(results);
    return !result.empty();
}

bool Address::GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t>>& result,
                                    const std::string& iface, int family) {
    if (iface.empty() || iface == '*') {
        if(family == AF_INET || family == AF_UNSPEC) {
            result.push_back(std::make_pair(Address::ptr(new IPv4Address()), 0u));
        }
        if(family == AF_INET6 || family == AF_UNSPEC) {
            result.push_back(std::make_pair(Address::ptr(new IPv6Address()), 0u));
        }
        return true;
    }

    std::multimap<std::string, std::pair<Address::ptr, uint32_t>> results;
    if (!GetInterfaceAddresses(results, family)) {
        return false;
    }

    auto its = results.equal_range(iface);
    for (; its.first != its.second; ++its.first) {
        result.push_back(its.first->second);
    }
    return !result.empty();
}

int Address::getFamily() const {
    return getAddr()->sa_family;
}

std::string Address::toString() const {
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address& rhs) const {
    socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
    int result = memcmp(getAddr(), rhs.getAddr(), minlen);
    if (result < 0) {
        return false;
    } else if (result > 0) {
        return false;
    } else if (getAddrLen() < rhs.getAddrLen()) {
        return true;
    } else {
        return false;
    }
}

bool Address::operator==(const Address& rhs) const {
    return getAddrLen() == rhs.getAddrLen()
        && memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}

bool Address::operator!=(const Address& rhs) const {
    return !(*this == rhs);
}

IPAddress::ptr IPAddress::Create(const char* addr, uint16_t port = 0) {
    addrinfo hints, *results;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AI_UNSPEC;
    int rt = getaddrinfo(address, NULL, &hints, &results);
    if (rt != 0) {
        SYLAR_LOG_ERROR(g_logger) << "IPAdress::Create(" << address << ", "
            << port << ") error=" << rt << " errno=" << gai_strerror(rt);
        return nullptr;
    }
    try {
        IPAdress::ptr result = std::dynamic_pointer_cast<IPAdress>(
                Adress::Create(results->ai_addr, (socklen_t)results->ai_addrlen));
        if (result) {
            result->setPort(port);
        }
        freeaddrinfo(results);
        return result;
    } catch (...) {
        freeaddrinfo(results);
        return nullptr;
    }
}
}