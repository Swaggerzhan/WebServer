cmake_minimum_required(VERSION 2.8)



INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(/usr/bin/gcc GNU)
CMAKE_FORCE_CXX_COMPILER(/usr/bin/g++ GNU)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE DEBUG)



add_executable(WebServer main.cpp
    utility.h
        Thread/ThreadPool.h Thread/ThreadPool.cpp
        HTTP/Request.h HTTP/Request.cpp
        Timer/TimerHandler.cpp Timer/TimerHandler.h)

find_package(Threads)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})



