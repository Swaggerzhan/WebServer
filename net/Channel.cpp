//
// Created by swagger on 2021/5/15.
//

#include "Channel.h"

#include "EventLoop.h"

const int Channel::kReadEvent = EPOLLIN | EPOLLONESHOT | EPOLLET;
const int Channel::kWriteEvent = EPOLLOUT | EPOLLONESHOT | EPOLLET;
const int Channel::kNoneEvent = 0;


Channel::Channel(EventLoop *loop, int fd)
 :  loop_(loop), // 初始化属于的EventLoop
    fd_(fd), // 初始化fd
    event_(0), // 初始事件
    epoll_ret_evnet_(0), // 初始化epoll真实的触发返回事件
    status_(-1) // 初始化当前channel状态 -1 为新
 {}


/* 更新channel状态 */
void Channel::update() {
    addToLoop_ = true;
    loop_->updateChannel(this);
}


void Channel::handleEvent() {
    if (epoll_ret_evnet_ & EPOLLHUP ||
    epoll_ret_evnet_ & EPOLLRDHUP ){ // 远程关闭情况
        //TODO:远程关闭情况，将其丢给上层？
        if (error_call_back) error_call_back();
    }
    if (epoll_ret_evnet_ & EPOLLIN ){
        if (read_call_back) read_call_back();
    }
    if (epoll_ret_evnet_ & EPOLLOUT ){
        if (write_call_back) write_call_back();
    }
}