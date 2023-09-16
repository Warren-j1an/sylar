#include "http_util.h"

#include <cstring>

namespace sylar {
namespace http {
HttpMethod StringToHttpMethod(const std::string& m) {
#define XX(num, name, string)       \
    if (m.compare(#string) == 0) {  \
        return HttpMethod::name;    \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID_METHOD;
}

HttpMethod CharsToHttpMethod(const char* m) {
#define XX(num, name, string)       \
    if (strncmp(m, #string, strlen(#string)) == 0) {  \
        return HttpMethod::name;    \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID_METHOD;
}

const char* HttpMethodToString(const HttpMethod& m) {
    switch (m) {
#define XX(num, name, string)   \
        case HttpMethod::name:  \
            return #string;
        HTTP_METHOD_MAP(XX)
#undef XX
        default:
            return "INVALID";
    }
}

const char* HttpStatusToString(const HttpStatus& s) {
    switch (s) {
#define XX(num, name, string)   \
        case HttpStatus::name:  \
            return #string;
        HTTP_STATUS_MAP(XX)
#undef XX
        default:
            return "Unknown";
    }
}

bool CaseInsensitiveLess::operator()(const std::string& lhs, const std::string& rhs) const {
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}
}   // http
}   // sylar