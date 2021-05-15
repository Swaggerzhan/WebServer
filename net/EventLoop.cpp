//
// Created by swagger on 2021/5/15.
//

#include "EventLoop.h"

#include "Channel.h"
#include "EpollPoller.h"


void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    if (eventHandling_){
        //TODO:正在处理回调函数，没准其中正在处理的就有需要删除的channel
    }
    poller_->removeChannel(channel);
}


bool EventLoop::hasChannel(Channel *channel) {
    return poller_->hasChannel(channel);
}

void EventLoop::loop() {

    quit_ = false;
    while (!quit_){
        /* 获取活跃的channel */
        poller_->poll(&activeChannel);
        eventHandling_ = true;
        for (auto channel : activeChannel){
            currentActiveChannel_ = channel; // 当前正在处理的channel
            currentActiveChannel_->handleEvent();
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
    }
}
