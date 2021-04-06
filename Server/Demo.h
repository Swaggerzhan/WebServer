//
// Created by swagger on 2021/4/6.
//

#ifndef WEBSERVER_DEMO_H
#define WEBSERVER_DEMO_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include "../utility.h"

class Demo{


private:

    int demo_sock;

    int epfd;

    struct epoll_event *epollArray;

    struct sockaddr_in local_addr;



public:
    Demo(){

        demo_sock = socket(PF_INET, SOCK_STREAM, 0);
        int reuse = 1;
        setsockopt(demo_sock, SO_REUSEADDR, &reuse, sizeof(reuse));

        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_addr.s_addr = inet_addr(HOST);
        local_addr.sin_port = htons(PORT);
        local_addr.sin_family = PF_INET;

        if ( bind(demo_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) == -1)
            perror("bind() error!");

        if ( listen(demo_sock, 5) == -1)
            perror("listen() error!");

        epfd = epoll_create(5);

        epollArray = new struct epoll_event[OPENMAX];


    }

    ~Demo(){
        delete [] epollArray;
    }
};

#endif //WEBSERVER_DEMO_H
