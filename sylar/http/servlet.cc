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

ServletDispatch::ServletDispatch() : Servlet("ServletDispatch") {
    m_default = std::make_shared<NotFoundServlet>("sylar/1.0");
}

int32_t ServletDispatch::handle(HttpRequest::ptr request,
    HttpResponse::ptr response, HttpSession::ptr session) {
    auto slt = getMatchedServlet(request->getPath());
    if (slt) {
        slt->handle(request, response, session);
    }
    return 0;
}

void ServletDispatch::addServlet(const std::string& uri, Servlet::ptr slt) {
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = std::make_shared<HoldServletCreator>(slt);
}

void ServletDispatch::addServlet(const std::string& uri, FunctionServlet::callback cb) {
    Servlet::ptr slt = std::make_shared<FunctionServlet>(cb);
    addServlet(uri, slt);
}

void ServletDispatch::addGlobalServlet(const std::string& uri, Servlet::ptr slt) {
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globals.begin(); it != m_globals.end(); ++it) {
        if (it->first == uri) {
            m_globals.erase(it);
            break;
        }
    }
    m_globals.push_back(std::make_pair(uri, std::make_shared<HoldServletCreator>(slt)));
}

void ServletDispatch::addGlobalServlet(const std::string& uri, FunctionServlet::callback cb) {
    Servlet::ptr slt = std::make_shared<FunctionServlet>(cb);
    addGlobalServlet(uri, slt);
}

void ServletDispatch::addServletCreator(const std::string& uri, IServletCreator::ptr creator) {
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = creator;
}

void ServletDispatch::addGlobalServletCreator(const std::string& uri,
                                              IServletCreator::ptr creator) {
    RWMutexType::WriteLock lock(m_mutex);
    for (auto it = m_globals.begin(); it != m_globals.end(); ++it) {
        if (it->first == uri) {
            m_globals.erase(it);
            break;
        }
    }
    m_globals.push_back(std::make_pair(uri, creator));
}
}   // http
}   // sylar