cmake_minimum_required(VERSION 3.1.2)



INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(/usr/bin/gcc-7 GNU)
CMAKE_FORCE_CXX_COMPILER(/usr/bin/g++-7 GNU)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)





add_executable(WebServer main.cpp HTTP/HttpHandler.h utility.h BASE/Tree.h Server/Demo.h TEST/thread_test.h TEST/threadPool.h)

find_package(Threads)
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})



