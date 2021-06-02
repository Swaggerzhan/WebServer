//
// Created by swagger on 2021/6/1.
//

#include "Channel.h"
#include <sys/epoll.h>
#include <cassert>


Channel::Channel()
:   fd_(-1),
    event_(0),
    retEvent_(0),
    isUsed_(false)
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
}

/* 重置 */
void Channel::reSet() {
    errorCallBack = nullptr;
    readCallBack = nullptr;
    writeCallBack = nullptr;
    isUsed_ = false;
    event_ = 0;
    retEvent_ = 0;
    fd_ = -1;
}


/* 通过返回的事件调用回调函数 */
void Channel::handleEvent() {
    if ( retEvent_ & (EPOLLERR) ){
        if (errorCallBack)
            errorCallBack();
    }
    if ( retEvent_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if (readCallBack)
            readCallBack();
    }
    if ( retEvent_ & (EPOLLOUT)){
        if (writeCallBack)
            writeCallBack();
    }
}
