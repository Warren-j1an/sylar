cmake_minimum_required(VERSION 3.0)
project(sylar VERSION 1.0.0 LANGUAGES C CXX)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/sylar/config)
include_directories(${PROJECT_SOURCE_DIR}/sylar/fiber)
include_directories(${PROJECT_SOURCE_DIR}/sylar/http)
include_directories(${PROJECT_SOURCE_DIR}/sylar/log)
include_directories(${PROJECT_SOURCE_DIR}/sylar/socket)
include_directories(${PROJECT_SOURCE_DIR}/sylar/stream)
include_directories(${PROJECT_SOURCE_DIR}/sylar/transmission)
include_directories(${PROJECT_SOURCE_DIR}/sylar/thread)
include_directories(${PROJECT_SOURCE_DIR}/sylar/util)

include_directories(${PROJECT_SOURCE_DIR}/third_party)

link_directories(${PROJECT_SOURCE_DIR}/lib)

option(BUILD_TEST "ON for complile test" ON)

set(LIB_SRC
    sylar/config/config.cc
    sylar/fiber/fiber.cc
    sylar/fiber/iomanager.cc
    sylar/fiber/scheduler.cc
    sylar/fiber/timer.cc
    sylar/http/http.cc
    sylar/http/httpclient_parser.rl.cc
    sylar/http/http_connection.cc
    sylar/http/http_parser.cc
    sylar/http/http_server.cc
    sylar/http/http_session.cc
    sylar/http/http_util.cc
    sylar/http/http11_parser.rl.cc
    sylar/http/servlet.cc
    sylar/http/uri.cc
    sylar/log/log_level.cc
    sylar/log/log_event.cc
    sylar/log/log_formatter.cc
    sylar/log/log_appender.cc
    sylar/log/log.cc
    sylar/socket/address.cc
    sylar/socket/fd_manager.cc
    sylar/socket/hook.cc
    sylar/socket/socket.cc
    sylar/stream/bytearray.cc
    sylar/stream/socket_stream.cc
    sylar/stream/stream.cc
    sylar/transmission/tcp_server.cc
    sylar/thread/mutex.cc
    sylar/thread/thread.cc
    sylar/util/fsUtil.cc
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
sylar_add_executable(echo_server "examples/echo_server.cc" sylar "${LIBS}")
sylar_add_executable(test_address "tests/test_address.cc" sylar "${LIBS}")
sylar_add_executable(test_bytearray "tests/test_bytearray.cc" sylar "${LIBS}")
sylar_add_executable(test_config "tests/test_config.cc" sylar "${LIBS}")
sylar_add_executable(test_fiber "tests/test_fiber.cc" sylar "${LIBS}")
sylar_add_executable(test_hook "tests/test_hook.cc" sylar "${LIBS}")
sylar_add_executable(test_http "tests/test_http.cc" sylar "${LIBS}")
sylar_add_executable(test_http_parser "tests/test_http_parser.cc" sylar "${LIBS}")
sylar_add_executable(test_http_server "tests/test_http_server.cc" sylar "${LIBS}")
sylar_add_executable(test_iomanager "tests/test_iomanager.cc" sylar "${LIBS}")
sylar_add_executable(test_log "tests/test_log.cc" sylar "${LIBS}")
sylar_add_executable(test_scheduler "tests/test_scheduler.cc" sylar "${LIBS}")
sylar_add_executable(test_socket "tests/test_socket.cc" sylar "${LIBS}")
sylar_add_executable(test_tcp_server "tests/test_tcp_server.cc" sylar "${LIBS}")
sylar_add_executable(test_thread "tests/test_thread.cc" sylar "${LIBS}")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)