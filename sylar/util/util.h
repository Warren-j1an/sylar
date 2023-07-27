#pragma once

#include <cxxabi.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>

namespace sylar {
pid_t GetThreadID();

uint32_t GetFiberID();

template<typename T>
std::string TypetoName() {
    char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    std::string name = s_name;
    free(s_name);
    return name;
}
}