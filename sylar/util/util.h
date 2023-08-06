#pragma once

#include <assert.h>
#include <cxxabi.h>
#include <stdint.h>
#include <string>
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
}