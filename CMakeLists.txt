cmake_minimum_required(VERSION 2.8)



INCLUDE(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER(/storage/app/gnu/gcc-8.2.0/bin/gcc GNU)
#CMAKE_FORCE_CXX_COMPILER(/storage/app/gnu/gcc-8.2.0/bin/g++ GNU)


#CMAKE_FORCE_C_COMPILER(/usr/bin/gcc-7 GNU)
#CMAKE_FORCE_CXX_COMPILER(/usr/bin/g++-7 GNU)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE DEBUG)



add_executable(WebServer main.cpp
    utility.h
#        Thread/ThreadPool.h Thread/ThreadPool.cpp
        net/Request.h net/Request.cpp
        net/Mime.h net/Mime.cpp
        base/Mutex.h base/Condition.h base/Condition.cpp
        base/Log.h base/Log.cpp base/AsyncLog.h base/AsyncLog.cpp
        base/Buffer.h base/Buffer.cpp
        base/TimeStamp.h base/TimeStamp.cpp

        base/BlockQueue.cpp base/BlockQueue.h

        base/CountDownLatch.h base/CountDownLatch.cpp

        base/Thread.h base/ThreadPool.h base/ThreadPool.cpp
        base/Thread.cpp base/CurrentThread.h base/CurrentThread.cpp

        net/EpollPoll.h net/EpollPoll.cpp

        net/Channel.h net/Channel.cpp net/HttpServer.cpp net/HttpServer.h )

find_package(Threads)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})



