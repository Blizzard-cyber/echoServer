cmake_minimum_required(VERSION 3.10)
project(echoServer)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找线程库
find_package(Threads REQUIRED)

# 收集所有源文件，包括 global.cpp
file(GLOB COMMON_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 为 server 可执行文件设置入口文件
set(SERVER_ENTRY ${CMAKE_SOURCE_DIR}/server.cpp)
add_executable(server ${SERVER_ENTRY} ${COMMON_SOURCES})


# 为 client 可执行文件设置入口文件
set(CLIENT_ENTRY ${CMAKE_SOURCE_DIR}/client.cpp)
add_executable(client ${CLIENT_ENTRY} ${COMMON_SOURCES})
target_link_libraries(client Threads::Threads)
