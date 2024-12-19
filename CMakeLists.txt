cmake_minimum_required(VERSION 3.10)
project(ServerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/src/headers)

set(SOURCE_FILES
    main.cpp
    src/core/server.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(ServerApp ${SOURCE_FILES})

find_package(Threads REQUIRED)

target_link_libraries(ServerApp PRIVATE Threads::Threads)
