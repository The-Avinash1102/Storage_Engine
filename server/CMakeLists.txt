cmake_minimum_required(VERSION 3.16)
project(WebSocketServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

add_subdirectory(libs)

add_executable(WebSocketServer test_Server.cpp)

add_executable(SessionsTest test_Sessions.cpp)

target_link_libraries(WebSocketServer PRIVATE Server)
target_link_libraries(SessionsTest PRIVATE Sessions)

