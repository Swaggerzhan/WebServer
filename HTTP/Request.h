//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include "../utility.h"
#include <sys/socket.h>
#include <cstring>
#include <iostream>


class Request {
public:

    int fd;

    char buf[BUFSIZE];

public:

    /**
     * 请求方法
     */
    Request(int fd);

    /**
     * 请求处理方法
     */
    void process();
};


#endif //WEBSERVER_REQUEST_H
