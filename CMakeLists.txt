cmake_minimum_required(VERSION 3.1.2)



INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(/usr/bin/gcc-7 GNU)
CMAKE_FORCE_CXX_COMPILER(/usr/bin/g++-7 GNU)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE DEBUG)



add_executable(WebServer main.cpp HTTP/HttpHandler.h 
    utility.h BASE/Tree.h Server/Demo.h TEST/thread_test.h 
    TEST/threadPool.h TEST/DI.h TEST/DI.cpp 
    TEST/threadPool.cpp
    Timer/TimerHeap.h Timer/TimerHeap.cpp
        TEST/PointerSwap.h
    )

find_package(Threads)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})



