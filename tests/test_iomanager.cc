#include "iomanager.h"
#include "log.h"
#include "timer.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

int sock = 0;

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "Test fiber";
    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "14.119.104.254", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
        });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            //close(sock);
            if (sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ)) {
                SYLAR_LOG_INFO(g_logger) << "cancel read";
            };
            close(sock);
        });
    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}

void test_iom() {
    sylar::IOManager iom(2, false);
    iom.schedule(&test_fiber);
}

sylar::Timer::ptr timer;
void test_timer() {
    sylar::IOManager iom(2);
    timer = iom.addTimer(500, [](){
        static int i = 0;
        SYLAR_LOG_INFO(g_logger) << "hello timer i=" << i;
        if (++i == 5) {
            sylar::IOManager::GetThis()->reset(timer, 2000, true);
            // timer->cancel();
        }
    }, true);
}

int main() {
    // test_iom();
    test_timer();
    return 0;
}