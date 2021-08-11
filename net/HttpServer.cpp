//
// Created by swagger on 2021/6/1.
//

#include "HttpServer.h"
#include "EpollPoll.h"
#include "Channel.h"
#include "Request.h"
#include "../base/Log.h"
#include <cstring>
#include "../base/ThreadPool.h"
#include "../base/Cache.h"


int HttpServer::listenfd_ = -1;
int HttpServer::port_ = -1;
int HttpServer::kRequestCount_ = 1000;
sockaddr_in HttpServer::local_addr_;


void HttpServer::netWorkInit(int port) {
    port_ = port;
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    memset(&local_addr_, 0, sizeof(local_addr_));
    local_addr_.sin_addr.s_addr = inet_addr("0.0.0.0");
    local_addr_.sin_port = htons(port);
    local_addr_.sin_family = AF_INET;
    int reuse = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    if (::bind(listenfd_, (sockaddr*)&local_addr_, sizeof local_addr_) < 0){
       // Log(L_FATAL) << "bind() error!";
    }
    if (::listen(listenfd_, 5) < 0){
        //Log(L_FATAL) << "listen() error!";
    }
    //Log(L_DEBUG)<< "netWork init OK";
}


HttpServer::HttpServer(EpollPoll *poller)
:   poller_(poller),
    listenChannel_(new Channel),
    quit_(false),
    queue_(10000),
    cache_(new Cache())
{
    listenChannel_->setfd(listenfd_);
    listenChannel_->setEvent(EPOLLIN );
    listenChannel_->setReadCallBack(
            std::bind(&HttpServer::AcceptClient, this)
            ); // listenfd的读取回调函数就是accept事件
    poller_->update(EPOLL_CTL_ADD, listenChannel_); // 添加监听套接字
    //启动线程？
    kRequestCount_ = 1000; /* 暂定 */
    for (int i=0; i<kRequestCount_; i++){
        //queue_.push_back(new Request);
        queue_.append(new Request(cache_));
    }
    start();
    request_count = kRequestCount_;

}


HttpServer::~HttpServer() {
    assert(quit_);
    /* 清空队列 */
    while (!queue_.isEmpty()){
        delete queue_.pop();
    }
}


void HttpServer::start() {

    pool_ = new ThreadPool;

    pool_->start(6);
    //Log(L_DEBUG) << "thread pool init OK";
    //在这里启动线程
    // 之后进入主循环
    //Log(L_DEBUG) << "enter loop";
    poller_->loop();
    quit_ = true;
}


void HttpServer::AcceptClient() {
    sockaddr_in remote_addr{};
    memset(&remote_addr, 0, sizeof remote_addr);
    socklen_t remote_addr_sz = sizeof remote_addr;
    int sockfd = ::accept(listenfd_, (sockaddr*)&remote_addr, &remote_addr_sz);
    //Log(L_DEBUG) << "accept new client" << sockfd;
    if (sockfd < 0) {
        //Log(L_ERROR) << "accept fd error";
    }
    else{
        if (queue_.isEmpty()){
            expand();
            std::cout << "TODO:expand()" << std::endl;
        }

//        while (queue_.front() == nullptr){
//            std::cout << "queue empty" << std::endl;
//            std::cout << queue_.size() << std::endl;
//            std::cout << "count:" << request_count << std::endl;
//            sleep(1);
//        }
        auto request = queue_.pop();
        request_count --;
        assert(!request->channel_.getIsUsed()); // channel是未使用状态
        request->init(sockfd, EPOLLIN | EPOLLET | EPOLLONESHOT);
        /* 设置回调函数 */
        request->channel_.setReadCallBack(
                std::bind(&HttpServer::readEvent, this, request));
        request->channel_.setWriteCallBack(
                std::bind(&HttpServer::writeEvent, this, request));
        request->channel_.setErrorCallBack(
                std::bind(&HttpServer::errorEvent, this, request));
        poller_->update(EPOLL_CTL_ADD, &request->channel_); // 添加到epoll中去

//        auto iter = map_.find(&request->channel_);
//        assert(iter == map_.end()); // for debug
//        mapNode node(&request->channel_, request);
//        map_.insert(node);
    }
}


void HttpServer::readEvent(Request* request) {
    //std::cout << "readEvent" << std::endl;
    Channel* channel = &request->channel_;
    bool ret = request->read();
    if ( ret ) {
        pool_->put(
                std::bind(&HttpServer::process, this, request)
        ); // 解析HTTP
    }else{
        /* close */
        request->close_conn();
        poller_->update(EPOLL_CTL_DEL, channel); // 从epoll中删除
        queue_.append(request); // 重新加入到队列中去
    }

}

/* 线程池将执行此函数?，接管Request中原先存在的process */
void HttpServer::process(Request* request) {
    httpDecode ret = request->parse_all();
    if (ret == INCOMPLETE_REQUEST) {
        poller_->update(EPOLL_CTL_MOD, &request->channel_);
        return;
    }
    /* 发送信息 */
    request->process_send();
    writeEvent(request);

}

void HttpServer::errorEvent(Request* request) {
    Channel* channel = &request->channel_;
    request->close_conn();
    poller_->update(EPOLL_CTL_DEL, channel);
    queue_.append(request);
}

/* write Event可能由IO线程调用，也有可能由子线程处理 */
void HttpServer::writeEvent(Request* request) {
    Channel* channel = &request->channel_;
    WriteProcess ret = request->write();
    if ( ret == WriteIncomplete){
        channel->setEvent(EPOLLOUT); // 添加写事件
        //std::cout << "try to update fd" << std::endl;
        poller_->update(EPOLL_CTL_MOD, channel);
        return;
    }
    if ( ret == WriteError ){
        /* 出错，直接进行删除操作 */
        request->close_conn();
        poller_->update(EPOLL_CTL_DEL, channel);
        queue_.append(request);
        request_count ++;
        return;
    }
    if ( ret == WriteOk ){
        /* keep-alive状态和正常关闭?，设置定时器？ */
        /* 暂时直接关闭 */
//        request->close_conn();
//        queue_.append(request);
        if (request->keep_alive){
            poller_->update(EPOLL_CTL_MOD, channel);
        }else {
            request->close_conn();
            poller_->update(EPOLL_CTL_DEL, channel);
            request_count ++;
            queue_.append(request);
        }

    }
}


void HttpServer::expand() {}
void HttpServer::reduce() {}




