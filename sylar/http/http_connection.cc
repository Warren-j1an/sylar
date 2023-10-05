#include "http_connection.h"

namespace sylar {
namespace http {
std::string HttpResult::toString() const {
    std::stringstream ss;
    ss << "[HttpResult result=" << result << " error=" << error << " response="
        << (response ? response->toString() : "nullptr") << "]";
    return ss.str();
}
}   // sylar
}   // http