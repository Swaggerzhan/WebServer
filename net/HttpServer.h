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
#include "../base/BlockQueue.h"

class EpollPoll;
class Channel;
class Request;
class ThreadPool;
class Cache;


class HttpServer {
public:

    typedef std::pair<Channel*, Request*> mapNode;
    typedef std::map<Channel*, Request*>::iterator mapIter;

    HttpServer(EpollPoll* poller);

    ~HttpServer();

    static void netWorkInit(int port);

    void AcceptClient();

    void start();

    void readEvent(Request* request);

    void writeEvent(Request* request);

    void errorEvent(Request* request);

    void process(Request* channel);


private:

    void expand(); // 扩充Channel

    void reduce(); // 减小Channel

private:

    bool quit_; // 是否退出

    //std::deque<Request*> queue_; // 处于queue_中的所有对象都是处于空闲状态
    BlockQueue<Request*> queue_;

    std::map<Channel*, Request*> map_; // 通过Channel 索引 Request

    static int listenfd_;
    static int port_;
    static sockaddr_in local_addr_;
    static int kRequestCount_;
    int request_count;

    EpollPoll* poller_;
    Channel* listenChannel_;
    ThreadPool* pool_; // 线程池
    Cache* cache_;      // 缓存

};


#endif //WEBSERVER_HTTPSERVER_H
