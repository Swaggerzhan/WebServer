//
// Created by swagger on 2021/4/6.
//

#ifndef WEBSERVER_UTILITY_H
#define WEBSERVER_UTILITY_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

#define TIMESLOT 1
#define HOST "0.0.0.0"
#define PORT 7999
#define OPENMAX 20000
#define RECVBUF 1024
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
 * 网络初始化，返回监听套接字
 * @return
 */
int network_init(){
    int demo = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_addr.s_addr = inet_addr(HOST);
    local_addr.sin_port = htons(PORT);
    local_addr.sin_family = AF_INET;
    /* 设置重复使用地址便于测试 */
    int reuse = 1;
    setsockopt(demo, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    /* 将地址和套接字绑定 */
    if (bind(demo, (sockaddr*)&local_addr, sizeof(local_addr)) == -1){
        printf("bind() error!\n");
        exit(1);
    }
    /* 监听端口 */
    if ( listen(demo, 5) == -1){
        printf("listen() error!\n");
        exit(1);
    }
}

/**
 * 异常处理
 * @param msg
 * @param code
 */
void exit_error(const char* msg, bool ex, int code=1){
    printf("%s error!\n", msg);
    printf("reason %s\n", strerror(errno));
    if (ex) {
        exit(code);
    }
}





#endif //WEBSERVER_UTILITY_H
