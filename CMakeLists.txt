cmake_minimum_required(VERSION 3.1.2)



INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(/usr/bin/gcc-7 GNU)
CMAKE_FORCE_CXX_COMPILER(/usr/bin/g++-7 GNU)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE DEBUG)



add_executable(WebServer main.cpp
    utility.h
    Timer/TimerHeap.h Timer/TimerHeap.cpp
        Process/ProcessPool.h Process/ProcessPool.cpp
        Thread/ThreadPool.h Thread/ThreadPool.cpp
        HTTP/Request.h HTTP/Request.cpp
        HTTP/HttpDecoder.h HTTP/HttpDecoder.cpp
        )

find_package(Threads)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})



