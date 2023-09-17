#include "http_server.h"
#include "http_session.h"
#include "log.h"

namespace sylar {
namespace http {
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("root");

HttpServer::HttpServer(bool keepalive
               ,sylar::IOManager* worker
               ,sylar::IOManager* io_worker
               ,sylar::IOManager* accept_worker)
    :TcpServer(worker, io_worker, accept_worker)
    ,m_isKeepalive(keepalive) {
    m_type = "http";
}

void HttpServer::handleClient(Socket::ptr client) {
    SYLAR_LOG_DEBUG(g_logger) << "handleClient " << *client;
    HttpSession::ptr session(new HttpSession(client));
    do {
        auto req = session->recvRequest();
        if(!req) {
            SYLAR_LOG_DEBUG(g_logger) << "recv http request fail, errno="
                << errno << " errstr=" << strerror(errno)
                << " cliet:" << *client << " keep_alive=" << m_isKeepalive;
            break;
        }
        HttpResponse::ptr rsp(new HttpResponse(req->getVersion()
                            ,req->isClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        rsp->setBody("hello sylar");
        // m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);

        if(!m_isKeepalive || req->isClose()) {
            break;
        }
    } while(true);
    session->close();
}
}
}