//
// Created by swagger on 2021/6/1.
//

#ifndef WEBSERVER_EPOLLPOLL_H
#define WEBSERVER_EPOLLPOLL_H

#include <functional>
#include <map>
#include <deque>
#include <sys/epoll.h>
#include <cassert>
#include <vector>
#include <pthread.h>
#include <cerrno>
#include <sys/eventfd.h>
#include "../base/Mutex.h"


class Channel;

class EpollPoll {
public:

    typedef std::vector<epoll_event> EventList; // 事件
    typedef std::function<void()> Functor; // 回调函数

    explicit EpollPoll(int);

    ~EpollPoll();

    /* 获取事件 */
    void poll();

    /* EPOLL主循环 */
    void loop();

    /* 提前叫醒epoll处理更新事件 */
    void runInLoop(Functor cb);

    void update(int op, Channel* channel);


private:

    /* 处理提交过来的回调函数 */
    void doPendingFunctor();

    void wakeup();

private:


    int epfd_;
    bool quit_; // 是否退出

    std::map<int, Channel*> map_; // 活跃的频道
    std::deque<Channel*> queue_; // channel 对象池
    EventList event_; // epoll事件返回的数据
    int channelCount_;
    bool isHandlingCallBack; // 处理回调函数中

    /* 用来提前叫醒 */
    int wakeupFd_;
    Channel* wakeChannel_;

    pthread_t threadID_; // 本线程id
    std::deque<Functor> callBackQueue_; // 回调函数队列
    bool doPending_; // 正在调用回调函数
    MutexLock mutex_; // 互斥锁

};


#endif //WEBSERVER_EPOLLPOLL_H
