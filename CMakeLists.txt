cmake_minimum_required(VERSION 3.10)
project(echoServer)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(echoServer ${SOURCES})
