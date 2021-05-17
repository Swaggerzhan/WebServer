//
// Created by swagger on 2021/5/15.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H

#include <functional>
#include <utility>
#include <sys/epoll.h>

class EventLoop;


class Channel {
public:

    typedef std::function<void()> Functor;

    Channel(EventLoop* loop, int fd);
    ~Channel(){}

    void handleEvent();

    inline void setReadCallBack(Functor cb){
        read_call_back = std::move(cb);
    }
    inline void setWriteCallBack(Functor cb){
        write_call_back = std::move(cb);
    }
    inline void setErrorCallBack(Functor cb){
        error_call_back = std::move(cb);
    }
    inline void setCloseCallBack(Functor cb){

    }

    inline void enableEventRead(){event_ |= kReadEvent; update(); }
    inline void enableEventWrite(){event_ |= kWriteEvent; update(); }
    inline void disableEventRead(){event_ &= ~kReadEvent; update(); }
    inline void disableEventWrite(){event_ &= ~kWriteEvent; update(); }

    /* 更新状态 */
    void update();

    inline int getEvent() const { return event_; }
    inline int getFd() const { return fd_; }
    inline int getStatus() const { return status_; }
    inline void setStatus(int status) { status_ = status; }
    inline void setEpollRetEvent(int event){epoll_ret_evnet_ = event;}
    inline bool isNoneEvent(){return event_ == kNoneEvent;}


private:

    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kNoneEvent;

    EventLoop* loop_; // channel拥有者
    bool addToLoop_; // 是否已经添加到loop中

    Functor read_call_back;
    Functor write_call_back;
    Functor error_call_back;


    int fd_; // 当前channel所操作的fd，channel不拥有它
    int event_; // 当前channel所关注的事件
    int status_; // 当前channel的状态
    int epoll_ret_evnet_; // EPOLL返回的真正监听事件


};


#endif //WEBSERVER_CHANNEL_H
