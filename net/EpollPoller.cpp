//
// Created by swagger on 2021/5/15.
//

#include "EpollPoller.h"

#include "Channel.h"
#include "EventLoop.h"
#include <iostream>

const int EpollPoller::kNew = -1; // 还未添加进EPOLL中的channel
const int EpollPoller::kAdded = 1; // 已经添加进EPOLL中的channel
const int EpollPoller::kDelete = 2; // 删除掉的channel，没有在EPOLL中，但是在vector中



void EpollPoller::updateChannel(Channel *channel) {
    const int status = channel->getStatus();
    const int fd = channel->getFd();
    /* channel是新节点又或者是曾经被删除掉的节点，但是现在有关注的事件了，
     * 那就添加到EPOLL中去 */
    if (status == kNew || status == kDelete) {
        channel_[fd] = channel; // 添加进channel中
        channel->setStatus(kAdded);
        /* 添加到EPOLL中去 */
        update(EPOLL_CTL_ADD, channel);
    /* channel是存在于EPOLL中的节点，如果不关注事件就将其删除
     * 又或者可能是事件需要更新改变之类的 */
    }else {
        /* 当channel不再关注事件时就将其在EPOLL中删除 */
        if (channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel);
            channel->setStatus(kDelete);
        }
        else
            update(EPOLL_CTL_MOD, channel);
    }
}

/* 删除channel */
void EpollPoller::removeChannel(Channel *channel) {
    const int fd = channel->getFd();
    const int status = channel->getStatus();
    /* 如果EPOLL中有存在就将其删除掉 */
    if ( status == kAdded ){
        update(EPOLL_CTL_DEL, channel);
    }
    channel_.erase(fd);
    /* 将channel设置为新节点 */
    channel->setStatus(kNew);
}


void EpollPoller::poll(std::vector<Channel*> *activeChannels) {
    int ret = ::epoll_wait(epfd_,
                           &*events_.begin(),
                           events_.size(),
                           0);
    if (ret < 0){
        if (errno == EINTR){
            // TODO:可能是调试导致的，暂时不处理
            return;
        }
    }
    if (ret == 0){
        std::cout << "nothing happened!" << std::endl;
        // TODO:保存日志，这里什么都没发生
        return;
    }
    if (ret > 0){
        getActChannel(ret, activeChannels);
    }

}

/* 将活跃的channel返回 */
void EpollPoller::getActChannel(int nums, std::vector<Channel*> *activeChannels) {
    for (int i=0; i<nums; i++){
        auto* channel = static_cast<Channel*>(events_[i].data.ptr);
        activeChannels->push_back(channel);
    }
}

/* 实际对EPOLL的操作，可能添加新节点，也可能是删除节点等等.. */
void EpollPoller::update(int option, Channel *channel) {
    struct epoll_event event{};
    event.events = channel->getEvent();
    event.data.ptr = channel;
    int ret = epoll_ctl(epfd_, option, channel->getFd(), &event);
    if (ret < 0){
        // TODO:wrong_handler
    }
}
