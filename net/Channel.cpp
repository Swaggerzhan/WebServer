//
// Created by swagger on 2021/6/1.
//

#include "Channel.h"
#include <sys/epoll.h>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include "Request.h"

Channel::Channel(Request *request)
:   fd_(-1),
    event_(0),
    retEvent_(0),
    isUsed_(false),
    request(request)
{

}

Channel::Channel() // for listen channel
        :   fd_(-1),
            event_(0),
            retEvent_(0),
            isUsed_(false),
            request(nullptr)
{

}


Channel::~Channel() {

}


void Channel::setEvent(int ev) {
    event_ |= ev;
}

void Channel::setfd(int fd) {
    assert(!isUsed_);
    fd_ = fd;
}

void Channel::setRetEvent(int ev) {
    retEvent_ = ev;
}

void Channel::setUsed() {
    isUsed_ = true;
    /* 非阻塞io */
    int fd = fcntl(fd_, F_GETFL);
    fd |= O_NONBLOCK;
    fcntl(fd_, F_SETFL, fd);
}

/* 重置 */
void Channel::reSet() {
    errorCallBack = nullptr;
    readCallBack = nullptr;
    writeCallBack = nullptr;
    isUsed_ = false;
    event_ = 0;
    retEvent_ = 0;
    // 尝试
//    if ( fd_ == 0 )
//        std::cout << "close fd: " << fd_ << std::endl;
    ::close(fd_);
    fd_ = -1;
}


/* 通过返回的事件调用回调函数 */
void Channel::handleEvent() {
    if ( retEvent_ & (EPOLLERR | EPOLLHUP | EPOLLRDHUP) ){
        if (errorCallBack)
            errorCallBack();
    }
    if ( retEvent_ & (EPOLLIN | EPOLLPRI )){
        if (readCallBack)
            readCallBack();
    }
    if ( retEvent_ & (EPOLLOUT)){
        if (writeCallBack)
            writeCallBack();
    }
}
