cmake_minimum_required(VERSION 3.0)
project(sylar VERSION 1.0.0 LANGUAGES C CXX)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++14 -Wall -Werror")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -fPIC")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-deprecated -Wno-unused-function")

include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/sylar/config)
include_directories(${PROJECT_SOURCE_DIR}/sylar/fiber)
include_directories(${PROJECT_SOURCE_DIR}/sylar/log)
include_directories(${PROJECT_SOURCE_DIR}/sylar/socket)
include_directories(${PROJECT_SOURCE_DIR}/sylar/thread)
include_directories(${PROJECT_SOURCE_DIR}/sylar/util)
link_directories(${PROJECT_SOURCE_DIR}/lib)

option(BUILD_TEST "ON for complile test" ON)

set(LIB_SRC
    sylar/config/config.cc
    sylar/fiber/fiber.cc
    sylar/fiber/iomanager.cc
    sylar/fiber/scheduler.cc
    sylar/fiber/timer.cc
    sylar/log/log_level.cc
    sylar/log/log_event.cc
    sylar/log/log_formatter.cc
    sylar/log/log_appender.cc
    sylar/log/log.cc
    sylar/socket/fd_manager.cc
    sylar/socket/address.cc
    sylar/socket/hook.cc
    sylar/thread/mutex.cc
    sylar/thread/thread.cc
    sylar/util/util.cc
    )

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)

set(LIBS
        sylar
        dl
        pthread
        yaml-cpp
    )

if(BUILD_TEST)
sylar_add_executable(test_config "tests/test_config.cc" sylar "${LIBS}")
sylar_add_executable(test_fiber "tests/test_fiber.cc" sylar "${LIBS}")
sylar_add_executable(test_hook "tests/test_hook.cc" sylar "${LIBS}")
sylar_add_executable(test_iomanager "tests/test_iomanager.cc" sylar "${LIBS}")
sylar_add_executable(test_log "tests/test_log.cc" sylar "${LIBS}")
sylar_add_executable(test_scheduler "tests/test_scheduler.cc" sylar "${LIBS}")
sylar_add_executable(test_thread "tests/test_thread.cc" sylar "${LIBS}")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)