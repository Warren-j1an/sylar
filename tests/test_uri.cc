#include "uri.h"
#include "log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

int main() {
    // sylar::Uri::ptr uri = sylar::Uri::Create("http://www.sylar.top/test/uri?id=100&name=sylar#frg");
    // sylar::Uri::ptr uri = sylar::Uri::Create("http://admin@www.sylar.top/test/中文/uri?id=100&name=sylar&vv=中文#frg中文");
    sylar::Uri::ptr uri = sylar::Uri::Create("http://admin@www.sylar.top");
    // sylar::Uri::ptr uri = sylar::Uri::Create("http://www.sylar.top/test/uri");
    SYLAR_LOG_DEBUG(g_logger) << uri->toString();
    auto addr = uri->createAddress();
    SYLAR_LOG_DEBUG(g_logger) << *addr;
    return 0;
}