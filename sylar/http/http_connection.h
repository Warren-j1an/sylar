#pragma once

#include <memory>
#include <string>

#include "http.h"
#include "socket_stream.h"

namespace sylar {
namespace http {
struct HttpResult {
    typedef std::shared_ptr<HttpResult> ptr;

    enum class Error {
        OK = 0,                         // 正常
        INVALID_URL = 1,                // 非法URL
        INVALID_HOST = 2,               // 无法解析HOST
        CONNECT_FAIL = 3,               // 连接失败
        SEND_CLOSE_BY_PEER = 4,         // 连接被对端关闭
        SEND_SOCKET_ERROR = 5,          // 发送请求产生Socket错误
        TIMEOUT = 6,                    // 超时
        CREATE_SOCKET_ERROR = 7,        // 创建Socket失败
        POOL_GET_CONNECTION = 8,        // 从连接池中取连接失败
        POOL_INVALID_CONNECTION = 9,    // 无效的连接
    };

    HttpResult(int _result, HttpResponse::ptr _response, const std::string& _error)
        : result(_result), response(_response), error(_error) {}

    std::string toString() const;

    int result;
    HttpResponse::ptr response;
    std::string error;
};

class HttpConnection : public SocketStream {

};
}   // sylar
}   // http