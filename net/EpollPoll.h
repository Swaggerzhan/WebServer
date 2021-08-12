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
#include "../base/BlockQueue.h"


class Request;
class Channel;
class Cache;

class EpollPoll {
public:

    typedef std::vector<epoll_event> EventList; // 事件
    typedef std::function<void()> Functor; // 回调函数

    explicit EpollPoll(int maxOpen=10000);

    ~EpollPoll();

    /* 获取事件 */
    void poll();

    /* EPOLL主循环 */
    void loop();

    /* 提前叫醒epoll处理更新事件 */
    void runInLoop(Functor cb);

    void update(int op, Channel* channel);

    BlockQueue<Request*>* getQueue();


private:

    /* 处理提交过来的回调函数 */
    void doPendingFunctor();

    void wakeup();

private:

    static int kMaxOpen; // epoll最大返回数量

    int epfd_;
    bool quit_; // 是否退出

    EventList event_; // epoll事件返回的数据
    bool isHandlingCallBack; // 处理回调函数中

    /* 用来提前叫醒 */
    int wakeupfd_;
    Channel* wakeChannel_;

    pthread_t threadID_; // 本线程id
    std::deque<Functor> callBackQueue_; // 回调函数队列
    bool doPending_; // 正在调用回调函数
    MutexLock mutex_; // 互斥锁

    BlockQueue<Request*> queue_;          //
    Cache* cache_;

};


#endif //WEBSERVER_EPOLLPOLL_H
