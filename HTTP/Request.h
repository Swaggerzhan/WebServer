//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include "../utility.h"
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../HTTP/HttpDecoder.h"
#include <fcntl.h>
#include <iostream>


class Request {
public:

    int fd;

    char *buf;

    static char* index_buf;


private:

    HttpDecoder* decoder;

public:

    /**
     * 请求方法
     */
    Request(int fd);

    ~Request();

    /**
     * 请求处理方法
     */
    int process();

    /**
     * 将index.html载入内存
     */
    static void bufInit();
};


#endif //WEBSERVER_REQUEST_H
