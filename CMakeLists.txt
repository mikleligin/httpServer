cmake_minimum_required(VERSION 3.10)
project(ServerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(ServerApp server.cpp)

find_package(Threads REQUIRED)

target_link_libraries(ServerApp PRIVATE Threads::Threads)
