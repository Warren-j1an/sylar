#pragma once

#include <memory>
#include <cstdint>
#include <list>
#include <string>

#include "http.h"
#include "mutex.h"
#include "socket_stream.h"
#include "uri.h"

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

class HttpConnectionPool;

class HttpConnection : public SocketStream {
friend class HttpConnectionPool;
public:
    typedef std::shared_ptr<HttpConnection> ptr;

    static HttpResult::ptr DoGet(const std::string& uri, uint64_t timeout_ms,
        const std::map<std::string, std::string>& headers = {}, const std::string& body = "");
    static HttpResult::ptr DoGet(Uri::ptr uri, uint64_t timeout_ms,
        const std::map<std::string, std::string>& headers = {}, const std::string& body = "");
    static HttpResult::ptr DoPost(const std::string& uri, uint64_t timeout_ms,
        const std::map<std::string, std::string>& headers = {}, const std::string& body = "");
    static HttpResult::ptr DoPost(Uri::ptr uri, uint64_t timeout_ms,
        const std::map<std::string, std::string>& headers = {}, const std::string& body = "");
    static HttpResult::ptr DoRequest(HttpMethod method, const std::string& uri, uint64_t timout_ms,
        const std::map<std::string, std::string>& headers = {}, const std::string& body = "");
    static HttpResult::ptr DoRequest(HttpMethod method, Uri::ptr uri, uint64_t timout_ms,
        const std::map<std::string, std::string>& headers = {}, const std::string& body = "");
    static HttpResult::ptr DoRequest(HttpRequest::ptr req, Uri::ptr uri, uint64_t timeout_ms);
    static HttpResult::ptr DoRequest(HttpRequest::ptr req, Address::ptr addr, bool is_https,
        uint64_t timeout_ms);
    static HttpResult::ptr DoRequest(HttpRequest::ptr req, Socket::ptr sock, uint64_t timeout_ms);

    HttpConnection(Socket::ptr sock, bool owner = true);
    ~HttpConnection();
    HttpResponse::ptr recvResponse();
    int sendRequest(HttpRequest::ptr req);

private:
    uint64_t m_createTime = 0;
    uint64_t m_request = 0;
};

class HttpConnectionPool {
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;

    static HttpConnectionPool::ptr Create(const std::string& uri
                                   ,const std::string& vhost
                                   ,uint32_t max_size
                                   ,uint32_t max_alive_time
                                   ,uint32_t max_request);

    HttpConnectionPool(const std::string& host
                       ,const std::string& vhost
                       ,uint32_t port
                       ,bool is_https
                       ,uint32_t max_size
                       ,uint32_t max_alive_time
                       ,uint32_t max_request);

    HttpConnection::ptr getConnection();


    /**
     * @brief 发送HTTP的GET请求
     * @param[in] url 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doGet(const std::string& url
                          , uint64_t timeout_ms
                          , const std::map<std::string, std::string>& headers = {}
                          , const std::string& body = "");

    /**
     * @brief 发送HTTP的GET请求
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doGet(Uri::ptr uri
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * @param[in] url 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doPost(const std::string& url
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doPost(Uri::ptr uri
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] method 请求类型
     * @param[in] uri 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] method 请求类型
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] req 请求结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpRequest::ptr req
                            , uint64_t timeout_ms);
private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);
private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    uint32_t m_maxSize;
    uint32_t m_maxAliveTime;
    uint32_t m_maxRequest;
    bool m_isHttps;

    MutexType m_mutex;
    std::list<HttpConnection*> m_conns;
    std::atomic<int32_t> m_total = {0};
};
}   // sylar
}   // http