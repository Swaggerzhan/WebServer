//
// Created by swagger on 2021/5/15.
//

#include "Channel.h"

#include "EventLoop.h"

const int Channel::kReadEvent = EPOLLIN | EPOLLONESHOT | EPOLLET;
const int Channel::kWriteEvent = EPOLLOUT | EPOLLONESHOT | EPOLLET;
const int Channel::kNoneEvent = 0;



/* 更新channel状态 */
void Channel::update() {
    addToLoop_ = true;
    loop_->updateChannel(this);
}