//
// Created by swagger on 2021/6/1.
//

#ifndef WEBSERVER_HTTPSERVER_H
#define WEBSERVER_HTTPSERVER_H

#include <functional>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <deque>
#include <map>


class EpollPoll;
class Channel;
class Request;
class ThreadPool;

class HttpServer {
public:

    typedef std::pair<Channel*, Request*> mapNode;
    typedef std::map<Channel*, Request*>::iterator mapIter;

    HttpServer(EpollPoll* poller);

    ~HttpServer();

    static void netWorkInit(int port);

    void AcceptClient();

    void start();

    void readEvent(Channel* channel);

    void writeEvent(Channel* channel);

    void errorEvent(Channel* channel);


private:

    void expand(); // 扩充Channel

    void reduce(); // 减小Channel

private:

    std::deque<Channel*> queue_; // 处于queue_中的Channel都是未使用的，对象池
    std::map<Channel*, Request*> map_; // 通过Channel索引Channel

    static int listenFd_;
    static int port_;
    static sockaddr_in local_addr_;

    EpollPoll* poller_;
    Channel* listenChannel_;
    ThreadPool* pool_; // 线程池

};


#endif //WEBSERVER_HTTPSERVER_H
