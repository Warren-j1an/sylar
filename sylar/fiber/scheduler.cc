#include "scheduler.h"
#include "util.h"

namespace sylar {
void FiberAndFunction::reset() {
    fiber = nullptr;
    cb = nullptr;
    thread = -1;
}

Scheduler::Scheduler(size_t thread, bool use_caller, const std::string& name) : m_name(name) {

}
}