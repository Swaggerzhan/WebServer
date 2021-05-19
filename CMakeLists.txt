cmake_minimum_required(VERSION 2.8)



INCLUDE(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER(/storage/app/gnu/gcc-8.2.0/bin/gcc GNU)
#CMAKE_FORCE_CXX_COMPILER(/storage/app/gnu/gcc-8.2.0/bin/g++ GNU)


CMAKE_FORCE_C_COMPILER(/usr/bin/gcc-7 GNU)
CMAKE_FORCE_CXX_COMPILER(/usr/bin/g++-7 GNU)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE DEBUG)



add_executable(WebServer main.cpp
    utility.h
        Thread/ThreadPool.h Thread/ThreadPool.cpp
        HTTP/Request.h HTTP/Request.cpp
        HTTP/Mime.h HTTP/Mime.cpp
        Timer/TimerNode.h Timer/TimerNode.cpp
        Timer/TimerHeap.h Timer/TimerHeap.cpp
        Timer/TimerHandler.h Timer/TimerHandler.cpp
        net/Channel.h net/Channel.cpp
        net/EventLoop.h net/EventLoop.cpp net/EpollPoller.h net/EpollPoller.cpp
        base/Mutex.h base/Condition.h base/Condition.cpp
        base/CountDownLatch.h base/CountDownLatch.cpp

        base/Thread.h base/ThreadPool.h base/ThreadPool.cpp
        base/Thread.cpp base/CurrentThread.h base/CurrentThread.cpp )

find_package(Threads)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})



