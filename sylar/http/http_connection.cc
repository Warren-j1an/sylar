#include "http_connection.h"
#include "http_parser.h"
#include "log.h"

namespace sylar {
namespace http {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");

std::string HttpResult::toString() const {
    std::stringstream ss;
    ss << "[HttpResult result=" << result << " error=" << error << " response="
        << (response ? response->toString() : "nullptr") << "]";
    return ss.str();
}

HttpConnection::HttpConnection(Socket::ptr sock, bool owner) :SocketStream(sock, owner) {}

HttpConnection::~HttpConnection() {
    SYLAR_LOG_DEBUG(g_logger) << "HttpConnection::~HttpConnection";
}

// HttpResponse::ptr HttpConnection::recvResponse() {
//     HttpResponseParser::ptr parser = std::make_shared<HttpResponseParser>();
//     uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
//     std::shared_ptr<char> buffer(new char[buff_size + 1], [](char* ptr){ delete[] ptr; });
//     char* data = buffer.get();
//     int offset = 0;
//     do {
//         int len = read(data + offset, buff_size - offset);
//         if (len <= 0) {
//             close();
//             return nullptr;
//         }
//         len += offset;
//         data[len] = '\0';
//         size_t nparser = parser->execute(data, len, false);
//         if (parser->hasError()) {
//             close();
//             return nullptr;
//         }
//         offset = len - nparser;
//         if (offset == (int)buff_size) {
//             close();
//             return nullptr;
//         }
//         if (parser->isFinished()) {
//             break;
//         }
//     } while (true);
//     const httpclient_parser& client_parser = parser->getParser();
//     std::string body;
//     if (client_parser.chunked) {
//         int len = offset;
//         do {
//             bool begin = true;
//             do {
//                 if (!begin || len == 0) {
//                     int rt = read(data + len, buff_size - len);
//                     if (rt <= 0) {
//                         close();
//                         return nullptr;
//                     }
//                     len += rt;
//                 }
//                 data[len] = '\0';
//                 size_t nparser = parser->execute(data, len, true);
//                 if (parser->hasError()) {
//                     goto out;
//                     close();
//                     return nullptr;
//                 }
//                 len -= nparser;
//                 if (len == (int)buff_size) {
//                     close();
//                     return nullptr;
//                 }
//                 begin = false;
//             } while (!parser->isFinished());

//             SYLAR_LOG_DEBUG(g_logger) << "content_len=" << client_parser.content_len;
//             if (client_parser.content_len + 2 <= len) {
//                 body.append(data, client_parser.content_len);
//                 memmove(data, data + client_parser.content_len + 2, len - client_parser.content_len - 2);
//                 len -= client_parser.content_len + 2;
//             } else {
//                 body.append(data, len);
//                 int left = client_parser.content_len - len + 2;
//                 while (left > 0) {
//                     int rt = read(data, left > (int)buff_size ? (int)buff_size : left);
//                     if(rt <= 0) {
//                         close();
//                         return nullptr;
//                     }
//                     body.append(data, rt);
//                     left -= rt;
//                 }
//                 body.resize(body.size() - 2);
//                 left = 0;
//             }
//         } while (!client_parser.chunks_done);
//     } else {
//         int64_t length = parser->getContentLength();
//         if(length > 0) {
//             body.resize(length);

//             int len = 0;
//             if(length >= offset) {
//                 memcpy(&body[0], data, offset);
//                 len = offset;
//             } else {
//                 memcpy(&body[0], data, length);
//                 len = length;
//             }
//             length -= offset;
//             if(length > 0) {
//                 if(readFixSize(&body[len], length) <= 0) {
//                     close();
//                     return nullptr;
//                 }
//             }
//         }
//     }
// out:
//     if (!body.empty()) {
//         auto content_encoding = parser->getData()->getHeader("content-encoding");
//         SYLAR_LOG_DEBUG(g_logger) << "content_encoding: " << content_encoding
//             << " size=" << body.size();
//         if(strcasecmp(content_encoding.c_str(), "gzip") == 0) {
//             auto zs = ZlibStream::CreateGzip(false);
//             zs->write(body.c_str(), body.size());
//             zs->flush();
//             zs->getResult().swap(body);
//         } else if(strcasecmp(content_encoding.c_str(), "deflate") == 0) {
//             auto zs = ZlibStream::CreateDeflate(false);
//             zs->write(body.c_str(), body.size());
//             zs->flush();
//             zs->getResult().swap(body);
//         }
//         parser->getData()->setBody(body);
//     }
// }

int HttpConnection::sendRequest(HttpRequest::ptr req) {
    std::stringstream ss;
    ss << *req;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}
}   // sylar
}   // http