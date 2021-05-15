//
// Created by swagger on 2021/5/15.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

class Channel;
class EpollPoller;

class EventLoop {
public:

    EventLoop(EpollPoller* poller):poller_(poller){}

    void updateChannel(Channel* channel);

    void poll();

    /* 循环 */
    void loop();


private:

    EpollPoller* poller_;
};


#endif //WEBSERVER_EVENTLOOP_H
