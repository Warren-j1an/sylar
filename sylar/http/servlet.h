#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "http.h"
#include "http_session.h"
#include "mutex.h"
#include "util.h"

namespace sylar {
namespace http {
class Servlet {
public:
    typedef std::shared_ptr<Servlet> ptr;

    Servlet(const std::string& name) : m_name(name) {}

    const std::string& getName() const { return m_name; }

    virtual ~Servlet() {}
    virtual int32_t handle(HttpRequest::ptr request,
        HttpResponse::ptr response, HttpSession::ptr session) = 0;

protected:
    std::string m_name;
};

class FunctionServlet : public Servlet {
public:
    typedef std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<int32_t (HttpRequest::ptr, HttpResponse::ptr, HttpSession::ptr)> callback;

    FunctionServlet(callback cb);
    virtual int32_t handle(HttpRequest::ptr request,
        HttpResponse::ptr response, HttpSession::ptr session) override;

private:
    callback m_cb;
};

class NotFoundServlet : public Servlet {
public:
    typedef std::shared_ptr<NotFoundServlet> ptr;

    NotFoundServlet(const std::string& name);
    virtual int32_t handle(HttpRequest::ptr request,
        HttpResponse::ptr response, HttpSession::ptr session) override;

private:
    std::string m_name;
    std::string m_content;
};

class IServletCreator {
public:
    typedef std::shared_ptr<IServletCreator> ptr;
    virtual ~IServletCreator() {}
    virtual Servlet::ptr get() const = 0;
    virtual std::string getName() const = 0;
};

class HoldServletCreator : public IServletCreator {
public:
    typedef std::shared_ptr<HoldServletCreator> ptr;

    HoldServletCreator(Servlet::ptr slt) : m_servlet(slt) {}

    Servlet::ptr get() const override {
        return m_servlet;
    }

    std::string getName() const override {
        return m_servlet->getName();
    }

private:
    Servlet::ptr m_servlet;
};

template<typename T>
class ServletCreator : IServletCreator {
public:
    typedef std::shared_ptr<ServletCreator> ptr;

    ServletCreator() {}

    Servlet::ptr get() const override {
        return std::make_shared<T>();
    }

    std::string getName() const override {
        return TypetoName<T>();
    }
};

class ServletDispatch : public Servlet {
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef RWMutex RWMutexType;

    ServletDispatch();
    virtual int32_t handle(HttpRequest::ptr request,
        HttpResponse::ptr response, HttpSession::ptr session) override;
    void addServlet(const std::string& uri, Servlet::ptr slt);
    void addServlet(const std::string& uri, FunctionServlet::callback cb);
    void addGlobServlet(const std::string& uri, Servlet::ptr slt);
    void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);
    void addServletCreator(const std::string& uri, IServletCreator::ptr creator);
    void addGlobServletCreator(const std::string& uri, IServletCreator::ptr creator);
    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);
    void setDefault(Servlet::ptr v) { m_default = v;}
    Servlet::ptr getDefault() const { return m_default;}
    Servlet::ptr getServlet(const std::string& uri);
    Servlet::ptr getGlobServlet(const std::string& uri);
    Servlet::ptr getMatchedServlet(const std::string& uri);
    void listAllServletCreator(std::map<std::string, IServletCreator::ptr>& infos);
    void listAllGlobServletCreator(std::map<std::string, IServletCreator::ptr>& infos);

    template<typename T>
    void addServletCreator(const std::string& uri) {
        addServletCreator(uri, std::make_shared<ServletCreator<T>>());
    }

    template<typename T>
    void addGlobServletCreator(const std::string& uri) {
        addGlobServletCreator(uri, std::make_shared<ServletCreator<T>>());
    }

private:
    RWMutexType m_mutex;
    std::unordered_map<std::string, IServletCreator::ptr> m_datas;
    std::vector<std::pair<std::string, IServletCreator::ptr>> m_globs;
    Servlet::ptr m_default;
};
}   // http
}   // sylar