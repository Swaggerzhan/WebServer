//
// Created by swagger on 2021/5/15.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include <vector>
#include <functional>
#include <memory>

class Channel;
class EpollPoller;

class EventLoop {
public:

    EventLoop(EpollPoller* poller);
    /* 更新channel */
    void updateChannel(Channel* channel);
    /* 删除channel */
    void removeChannel(Channel* channel);
    /* 当前channel是否存在于当前的eventLoop中 */
    bool hasChannel(Channel* channel);

    /* 循环 */
    void loop();

    /* 处理回调函数 */
    void doPendingCallBack();


private:

    EpollPoller* poller_;
    bool eventHandling_; // 线程正在处理回调函数
    bool quit_;
    bool pendingCallBack_; // 正在处理回调函数
    std::vector<std::function<void()>> callBackQueue_; // 回调函数数组，需要锁
    std::vector<Channel*> activeChannel; // 活跃的channel
    Channel* currentActiveChannel_; // 当前正在处理的channel

    int wakeFd_;
    std::unique_ptr<Channel> wakeUpPtr_; // wakeFd_的channel，声明周期由unique_ptr管理
};


#endif //WEBSERVER_EVENTLOOP_H
