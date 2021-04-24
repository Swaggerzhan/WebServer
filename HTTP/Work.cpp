//
// Created by swagger on 2021/4/24.
//

#include "Work.h"




void Work::close_conn() {
    printf("close %d\n", fd);
    removefd(epfd, fd);
}



void addfd(int epfd, int fd, bool oneShot){
    epoll_event event{};
    event.data.fd = fd;
    event.events = EPOLLET | EPOLLIN | EPOLLRDHUP;
    if (oneShot)
        event.events |= EPOLLONESHOT;
    /* 将套接字设置为非阻塞 */
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setNonBlock(fd);
}


void modfd(int epfd, int fd, int ev){
    epoll_event event{};
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}


void removefd(int epfd, int fd){
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
}



int setNonBlock(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
