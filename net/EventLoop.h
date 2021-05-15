//
// Created by swagger on 2021/5/15.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include <vector>

class Channel;
class EpollPoller;

class EventLoop {
public:

    EventLoop(EpollPoller* poller):poller_(poller){}
    /* 更新channel */
    void updateChannel(Channel* channel);
    /* 删除channel */
    void removeChannel(Channel* channel);
    /* 当前channel是否存在于当前的eventLoop中 */
    bool hasChannel(Channel* channel);

    /* 循环 */
    void loop();


private:

    EpollPoller* poller_;
    bool eventHandling_; // 线程正在处理回调函数
    bool quit_;
    std::vector<Channel*> activeChannel; // 活跃的channel
    Channel* currentActiveChannel_; // 当前正在处理的channel
};


#endif //WEBSERVER_EVENTLOOP_H
