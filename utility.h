//
// Created by swagger on 2021/4/6.
//

#ifndef WEBSERVER_UTILITY_H
#define WEBSERVER_UTILITY_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cerrno>
#include <cstring>

#define TIMESLOT 1
#define HOST "0.0.0.0"
#define PORT 80
#define OPENMAX 20000
#define RECVBUF 1024
#define SENDBUF 2048
#define BUFSIZE 2048


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
