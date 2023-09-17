#pragma once

#include <map>
#include <memory>

#include "http_util.h"

namespace sylar {
namespace http {
class HttpResponse;

class HttpRequest {
public:
    typedef std::shared_ptr<HttpRequest> ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

    HttpRequest(uint8_t version = 0x11, bool close = true);

    std::shared_ptr<HttpResponse> createResponse();

    void setMethod(HttpMethod v) { m_method = v; }
    HttpMethod getMethod() const { return m_method; }

    void setVersion(uint8_t v) { m_version = v;}
    uint8_t getVersion() const { return m_version; }

    void setClose(bool v) { m_close = v; }
    bool isClose() const { return m_close; }

    void setWebsocket(bool v) { m_websocket = v; }
    bool isWebsocket() const { return m_websocket; }

    void setPath(const std::string& v) { m_path = v; }
    const std::string& getPath() const { return m_path; }

    void setQuery(const std::string& v) { m_query = v; }
    const std::string& getQuery() const { return m_query; }

    void setFragment(const std::string& v) { m_fragment = v; }
    const std::string& getFragment() { return m_fragment; }

    void setBody(const std::string& v) { m_body = v; }
    const std::string& getBody() const { return m_body; }

    void setHeaders(const MapType& v) { m_headers = v; }
    void setHeader(const std::string& key, const std::string& val);
    const MapType& getHeaders() const { return m_headers; }
    std::string getHeader(const std::string& key, const std::string& def = "") const;
    void delHeader(const std::string& key);
    bool hasHeader(const std::string& key, std::string* val = nullptr);

    void setParams(const MapType& v) { m_params = v; }
    void setParam(const std::string& key, const std::string& val);
    const MapType& getParams() const { return m_params; }
    std::string getParam(const std::string& key, const std::string& def = "");
    void delParam(const std::string& key);
    bool hasParam(const std::string& key, std::string* val = nullptr);

    void setCookise(const MapType& v) { m_cookies = v; }
    void setCookie(const std::string& key, const std::string& val);
    const MapType& getCookies() const { return m_cookies; }
    std::string getCookie(const std::string& key, const std::string& def = "");
    void delCookie(const std::string& key);
    bool hasCookie(const std::string& key, std::string* val = nullptr);

    template<typename T>
    bool checkGetHeaderAs(const std::string& key, T& val, const T& def = T()) {
        return checkGetAs(m_headers, key, val, def);
    }

    template<typename T>
    T getHeaderAs(const std::string& key, const T& def = T()) {
        return getAs(m_headers, key, def);
    }

    template<typename T>
    bool checkGetParamAs(const std::string& key, T& val, const T& def = T()) {
        initQueryParam();
        initBodyParam();
        return checkGetAs(m_params, key, val, def);
    }

    template<typename T>
    T getParamAs(const std::string& key, const T& def = T()) {
        initQueryParam();
        initBodyParam();
        return getAs(m_params, key, def);
    }

    template<typename T>
    bool checkGetCookieAs(const std::string& key, T& val, const T& def = T()) {
        initCookies();
        return checkGetAs(m_cookies, key, val, def);
    }

    template<typename T>
    T getCookieAs(const std::string& key, const T& def = T()) {
        initCookies();
        return getAs(m_cookies, key, def);
    }

    std::ostream& dump(std::ostream& os) const;
    std::string toString() const;

public:
    void init();
    void initParam();
    void initQueryParam();
    void initBodyParam();
    void initCookies();

private:
    HttpMethod m_method;
    uint8_t m_version;
    bool m_close;
    bool m_websocket;
    uint8_t m_parserParamFlag;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    std::string m_body;
    MapType m_headers;
    MapType m_params;
    MapType m_cookies;
};

class HttpResponse {
public:
    typedef std::shared_ptr<HttpResponse> ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

    HttpResponse(uint8_t version = 0x11, bool close = true);

    void setStatus(HttpStatus v) { m_status = v; }
    HttpStatus getStatus() const { return m_status; }

    void setVersion(uint8_t v) { m_version = v; }
    uint8_t getVersion() const { return m_version; }

    void setClose(bool v) { m_close = v; }
    bool isClose() const { return m_close; }

    void setWebsocket(bool v) { m_websocket = v; }
    bool isWebsocket() const { return m_websocket; }

    void setBody(const std::string& v) { m_body = v; }
    const std::string& getBody() const { return m_body; }

    void setReason(const std::string& v) { m_reason = v; }
    const std::string& getReason() const { return m_reason; }

    void setHeaders(const MapType& v) { m_headers = v; }
    void setHeader(const std::string& key, const std::string& val);
    const MapType& getHeaders() const { return m_headers; }
    std::string getHeader(const std::string& key, const std::string& def = "") const;
    void delHeader(const std::string& key);

    void setCookie(const std::string& key, const std::string& val, time_t expired = 0,
        const std::string& path = "", const std::string& domain = "", bool secure = false);

    template<typename T>
    bool checkGetHeaderAs(const std::string& key, T& val, const T& def = T()) {
        return checkGetAs(m_headers, key, val, def);
    }

    template<typename T>
    T getHeaderAs(const std::string& key, const T& def = T()) {
        return getAs(m_headers, key, def);
    }

    std::ostream& dump(std::ostream& os) const;
    std::string toString() const;
    void setRedirect(const std::string& uri);

private:
    HttpStatus m_status;
    uint8_t m_version;
    bool m_close;
    bool m_websocket;
    std::string m_body;
    std::string m_reason;
    MapType m_headers;
    std::vector<std::string> m_cookies;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& req);
std::ostream& operator<<(std::ostream& os, const HttpResponse& rsp);
}   // http
}   // sylar