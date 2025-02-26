cmake_minimum_required(VERSION 3.10)
project(echoServer)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)

# 收集公共源文件（例如 tcpsocket.cpp、epoller.cpp、buffer.cpp、packetsocket.cpp 等）
file(GLOB COMMON_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 为 server 可执行文件设置入口文件
set(SERVER_ENTRY ${CMAKE_SOURCE_DIR}/server.cpp)
add_executable(server ${SERVER_ENTRY} ${COMMON_SOURCES})
# 可根据需要为 server 添加链接库或编译选项

# 为 client 可执行文件设置入口文件
set(CLIENT_ENTRY ${CMAKE_SOURCE_DIR}/client.cpp)
add_executable(client ${CLIENT_ENTRY} ${COMMON_SOURCES})
# 可根据需要为 client 添加链接库或编译选项
