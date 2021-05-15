//
// Created by swagger on 2021/5/15.
//

#include "EventLoop.h"

#include "Channel.h"
#include "EpollPoller.h"


void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

