//
// Created by swagger on 2021/6/1.
//

#include "EpollPoll.h"
#include "../base/Log.h"
#include <unistd.h>
#include "Channel.h"
#include "../base/Mutex.h"




int create_eventfd(){
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd < 0){
        Log(L_FATAL) << "eventfd error!";
        exit(1);
    }
    return fd;
}


EpollPoll::EpollPoll(int channelCount)
:   epfd_(epoll_create(5)),
    quit_(false),
    wakeupFd_(create_eventfd()),
    channelCount_(channelCount),
    threadID_(pthread_self())
{
    if (epfd_ < 0)
        Log(L_FATAL) << "epoll create error!";
    event_.reserve(channelCount_);
    wakeChannel_ = new Channel();
    wakeChannel_->setFd(wakeupFd_);
    wakeChannel_->setEvent(EPOLLIN);
    update(EPOLL_CTL_ADD, wakeChannel_);
}

EpollPoll::~EpollPoll() {
    /* 清空所有对象池 */
    assert(quit_);
    while (!queue_.empty()){
        Channel* node = queue_.back();
        delete node;
        queue_.pop_back();
    }
}


void EpollPoll::wakeup() {
    uint64_t one = 0;
    Log(L_INFO) << "wakeup called!";
    int ret = ::write(wakeupFd_, &one, sizeof one);
    if ( ret != sizeof one )
        Log(L_FATAL) << "wakeup write error!";
}


void EpollPoll::poll() {
    int ret = epoll_wait(epfd_, &*event_.begin(), event_.size(), 10);
    if (ret < 0){
        if (errno == EINTR)
            Log(L_DEBUG) << "epoll_wait ret EINTR could be debug";
    }else if (ret == 0)
        Log(L_DEBUG) << "epoll_wait nothing happened";
    else {
        isHandlingCallBack = true; // 直接处理回调函数
        for (int i=0; i<ret; i++){
            auto* channel = static_cast<Channel*>(event_[i].data.ptr);
            channel->setRetEvent(event_[i].events); // 真实的返回事件
            channel->handleEvent(); //处理回调函数
        }
        isHandlingCallBack = false;
    }
}

/* thread safe: 更多情况下其实是其他线程所调用的 */
void EpollPoll::runInLoop(EpollPoll::Functor cb) {
    if ( threadID_ == pthread_self()){ //  本线程中
        cb();
    }else {
        /* 如果不是本线程中运行，加入到回调函数中 */
        MutexLockGuard lock(mutex_);
        callBackQueue_.push_back(std::move(cb));
        /* ET模式下其实不需要提前叫醒？ */
        if (doPending_)// 正在处理回调函数
            wakeup();
    }
}


void EpollPoll::loop() {
    while (!quit_){
        /* 先处理回调函数，后处理poll事件 */
        doPendingFunctor();
        poll();
    }
}


/* 处理回调函数 */
void EpollPoll::doPendingFunctor() {
    doPending_ = true;
    while (!callBackQueue_.empty()){
        auto cb = callBackQueue_.front();
        cb();
        callBackQueue_.pop_front();
    }
    doPending_ = false;
}

/* 更新channel在epoll中的状态 */
void EpollPoll::update(int op, Channel *channel) {
    if (op == EPOLL_CTL_ADD ){
        epoll_event event{};
        event.data.ptr = channel;
        event.events = channel->getEvent();
        epoll_ctl(epfd_, op, channel->getFd(), &event);
        assert(!channel->getIsUsed()); // 还未被使用
        channel->setUsed(); // channel 已经被使用
    }else if (op == EPOLL_CTL_DEL){
        epoll_ctl(epfd_, op, channel->getFd(), nullptr);
        channel->reSet(); // 将channel重置，为下次使用做准备
    }else if (op == EPOLL_CTL_MOD ){ // MOD和ADD区别？
        epoll_event event{};
        event.data.ptr = channel;
        event.events = channel->getEvent();
        epoll_ctl(epfd_, op, channel->getFd(), &event);
        assert(!channel->getIsUsed()); // 还未被使用
        channel->setUsed(); // channel 已经被使用
    }else{
        Log(L_DEBUG) << "unknown bug in EpollPoll::update";
    }
}


