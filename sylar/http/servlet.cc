#include "servlet.h"

namespace sylar {
namespace http {
FunctionServlet::FunctionServlet(callback cb) : Servlet("FunctionServlet") , m_cb(cb) {}

int32_t FunctionServlet::handle(HttpRequest::ptr request,
    HttpResponse::ptr response, HttpSession::ptr session) {
    return m_cb(request, response, session);
}

NotFoundServlet::NotFoundServlet(const std::string& name)
    : Servlet("NotFoundServlet"), m_name(name) {
    m_content = "<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center>" + name + "</center></body></html>";
}

int32_t NotFoundServlet::handle(HttpRequest::ptr request,
    HttpResponse::ptr response, HttpSession::ptr session) {
    response->setStatus(HttpStatus::NOT_FOUND);
    response->setHeader("Server", "sylar/1.0.0");
    response->setHeader("Content-Type", "text/html");
    response->setBody(m_content);
    return 0;
}
}   // http
}   // sylar