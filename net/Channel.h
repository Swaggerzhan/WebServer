//
// Created by swagger on 2021/6/1.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H


#include <functional>

class Request;

class Channel {

public:

    typedef std::function<void()> callBack;

    Channel(Request* request);
    Channel();

    ~Channel();

    int getfd() const { return fd_; }
    int getRetEvent() const { return retEvent_; }
    int getEvent() const { return event_; }
    bool getIsUsed() const { return isUsed_; }

    /* 为channel设置监听事件 */
    void setEvent(int ev);
    void setRetEvent(int ev);
    void setfd(int fd);
    void setUsed();
    void reSet(); // 重置

    /* 设置回调函数 */
    void setReadCallBack(callBack cb){ readCallBack = std::move(cb); }
    void setWriteCallBack(callBack cb){ writeCallBack = std::move(cb); }
    void setErrorCallBack(callBack cb){ errorCallBack = std::move(cb); }


    void handleEvent();

    Request* request;

private:

    bool isUsed_;
    int fd_;
    int event_; // 监听事件
    int retEvent_; // epoll返回事件

    // 各种事件发生时的回调函数，回调函数的由上层实现
    callBack readCallBack;
    callBack writeCallBack;
    callBack errorCallBack;

};


#endif //WEBSERVER_CHANNEL_H
