#pragma once

#include "http.h"
#include "httpclient_parser.h"
#include "http11_parser.h"

namespace sylar {
namespace http {
class HttpRequestParser {
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;

    HttpRequestParser();
    size_t excute(char* data, size_t len);
    int isFinished();
    int hasError();
    HttpRequest::ptr getData() const { return m_data;}
    void setError(int v) { m_error = v;}
    uint64_t getContentLength();
    const http_parser& getParser() const { return m_parser;}

    static uint64_t GetHttpRequestBufferSize();
    static uint64_t GetHttpRequestMaxBodySize();

private:
    http_parser m_parser;
    HttpRequest::ptr m_data;
    int m_error;
};
}   // http
}   // sylar