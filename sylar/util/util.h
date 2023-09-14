#pragma once

#include <assert.h>
#include <cxxabi.h>
#include <stdint.h>
#include <cstring>
#include <sys/types.h>
#include <vector>

#include "log.h"

#define SYLAR_ASSERT(x) \
    if (!(x)) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_NAME("root")) << "ASSERTION: " #x \
            << "\nbacktrace:\n" << sylar::BacktraceToString(100, 2, "   "); \
        assert(x); \
    }

namespace sylar {
pid_t GetThreadID();
uint32_t GetFiberID();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

template<typename T>
std::string TypetoName() {
    char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    std::string name = s_name;
    free(s_name);
    return name;
}

uint64_t GetCurrentMS();
uint64_t GetCurrentUS();

std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

class StringUtil {
public:
    static std::string Format(const char* fmt, ...);
    static std::string Formatv(const char* fmt, va_list ap);

    static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
    static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

    static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
    static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
    static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");


    static std::string WStringToString(const std::wstring& ws);
    static std::wstring StringToWString(const std::string& s);

};
}