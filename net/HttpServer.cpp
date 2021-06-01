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


void HttpServer::netWorkInit(int port) {
    port_ = port;
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    memset(&local_addr_, 0, sizeof(local_addr_));
    local_addr_.sin_addr.s_addr = inet_addr("0.0.0.0");
    local_addr_.sin_port = htons(port);
    local_addr_.sin_family = AF_INET;
    if (::bind(listenFd_, (sockaddr*)&local_addr_, sizeof local_addr_) < 0){
        Log(L_FATAL) << "bind() error!";
    }
    if (::listen(listenFd_, 5) < 0){
        Log(L_FATAL) << "listen() error!";
    }
}


HttpServer::HttpServer(EpollPoll *poller)
:   poller_(poller),
    listenChannel_(new Channel)
{
    listenChannel_->setFd(listenFd_);
    listenChannel_->setEvent(EPOLLIN | EPOLLET );
    listenChannel_->setReadCallBack(
            std::bind(&HttpServer::AcceptClient, this)
            ); // listenFd的读取回调函数就是accept事件
    poller_->update(EPOLL_CTL_ADD, listenChannel_); // 添加监听套接字
    //启动线程？
    start();

}


HttpServer::~HttpServer() {

}


void HttpServer::start() {

    pool_ = new ThreadPool;
    pool_->start(2);//
    //在这里启动线程
    // 之后进入主循环
    poller_->loop();
}


void HttpServer::AcceptClient() {
    sockaddr_in remote_addr{};
    memset(&remote_addr, 0, sizeof remote_addr);
    socklen_t remote_addr_sz = sizeof remote_addr;
    int sockfd = ::accept(listenFd_, (sockaddr*)&remote_addr, &remote_addr_sz);
    if (sockfd < 0)
        Log(L_ERROR) << "accept fd error";
    else{
        if (queue_.empty()){
            expand();
        }
        auto channel = queue_.front();
        queue_.pop_front();
        assert(!channel->getIsUsed()); // channel是未使用状态
        channel->setUsed();
        channel->setFd(sockfd);
        channel->setEvent(EPOLLIN | EPOLLET | EPOLLONESHOT);
        /* map中没有对应的Request就进行创建，如果已有，则直接初始化使用即可 */
        mapIter iter;
        if ((iter = map_.find(channel)) == map_.end()) {
            auto request = new Request;
            map_.insert(mapNode(channel, request));
            request->init(channel->getFd());
        }else{
            iter->second->init(channel->getFd());
        }
        /* 设置各种回调函数 */
        channel->setReadCallBack(std::bind(&HttpServer::readEvent, this, channel));
        channel->setWriteCallBack( std::bind(&HttpServer::writeEvent, this, channel));
        channel->setErrorCallBack(std::bind(&HttpServer::errorEvent, this, channel));
        poller_->update(EPOLL_CTL_ADD, channel); // 添加到epoll中去
    }
}


void HttpServer::readEvent(Channel* channel) {
    auto iter = map_.find(channel);
    assert(iter != map_.end());
    if (iter->second->read()) {
        pool_->put(
                std::bind(&Request::process, iter->second)
        ); // 解析HTTP
    }else{
        /* close */
        iter->second->close_conn();
        poller_->update(EPOLL_CTL_DEL, iter->first); // 从epoll中删除
        map_.erase(iter); // 清空map_
    }

}


void HttpServer::errorEvent(Channel *channel) {
    auto iter = map_.find(channel);
    assert(iter != map_.end() );
    iter->second->close_conn();
    poller_->update(EPOLL_CTL_DEL, iter->first);
    map_.erase(iter);

}


void HttpServer::writeEvent(Channel* channel) {
    auto iter = map_.find(channel);
    assert(iter != map_.end());
    if ( !iter->second->write() ){
        /* 出错？ */
    }
}


void HttpServer::expand() {}
void HttpServer::reduce() {}




