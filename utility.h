//
// Created by swagger on 2021/4/6.
//

#ifndef WEBSERVER_UTILITY_H
#define WEBSERVER_UTILITY_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>


#define TIMESLOT 20
#define HOST "0.0.0.0"
#define PORT 80
#define OPENMAX 20000
#define RECVBUF 1024
#define SENDBUF 2048
#define BUFSIZE 2048
#define FILEBUF 16392
#define ROUTE_LENGTH 256



const int TIME_OUT = 1;
const int ADD_TIMER = 0;
const int RESET_TIMER = 2;
const int SLEEP_FOREVER = 3; // 永久睡眠，直到等待io线程叫醒


enum CHECK_STATUS{
    CHECK_REQUEST_LINE,
    CHECK_HEADER,
};


enum LINE_STATUS{
    LINE_OK,
    LINE_BAD,
    LINE_INCOMPLETE,
};


enum HTTP_CODE{
    GET_REQUEST,
    POST_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION,
    INCOMPLETE_REQUEST,
    NOT_FOUND,
};

enum CODE{
    ERROR, // 错误
    CLOSE, // 客户端关闭请求
    KEEP, // 保持链接
};







/**
 * 异常处理
 * @param msg
 * @param code
 */
static void exit_error(const char* msg, bool ex, int code=1){
    printf("%s error!\n", msg);
    printf("reason %s\n", strerror(errno));
    if (ex) {
        exit(code);
    }
}




#endif //WEBSERVER_UTILITY_H
