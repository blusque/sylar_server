cmake_minimum_required(VERSION 3.10.0)
project(SylarServer)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sylar/vendor/spdlog 
    ${CMAKE_CURRENT_SOURCE_DIR}/sylar/vendor/spdlog/build)

add_executable(sylar
    sylar/sylar.cpp
    sylar/src/log.cpp
    # src/log.cc
    # src/util.cc
    # src/config.cc
    # src/thread.cc
    # src/fiber.cc
    # src/scheduler.cc
    # src/iomanager.cc
    # src/timer.cc
    # src/hook.cc
    # src/address.cc
    # src/socket.cc
    # src/http/http.cc
    # src/http/http11_parser.cc
    # src/http/httpclient_parser.cc
    # src/http/http_parser.cc
    # src/http/http_session.cc
    # src/http/http_server.cc
    # src/http/servlet.cc
    # src/http/servlets/echo_servlet.cc
    # src/http/servlets/status_servlet.cc
    # src/http/servlets/default_servlet.cc
    # src/http/servlets/upload)
)
target_include_directories(sylar PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/sylar/src
    ${CMAKE_CURRENT_SOURCE_DIR}/sylar/vendor/spdlog/include
)

message("${CMAKE_BINARY_DIR}")