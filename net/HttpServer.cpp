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


int HttpServer::listenfd_ = -1;
int HttpServer::port_ = -1;
int HttpServer::kRequestCount_ = 10000;
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
        Log(L_FATAL) << "bind() error!";
    }
    if (::listen(listenfd_, 5) < 0){
        Log(L_FATAL) << "listen() error!";
    }
    Log(L_DEBUG)<< "netWork init OK";
}


HttpServer::HttpServer(EpollPoll *poller, int Count=10000)
:   poller_(poller),
    listenChannel_(new Channel)
{
    listenChannel_->setfd(listenfd_);
    listenChannel_->setEvent(EPOLLIN );
    listenChannel_->setReadCallBack(
            std::bind(&HttpServer::AcceptClient, this)
            ); // listenfd的读取回调函数就是accept事件
    poller_->update(EPOLL_CTL_ADD, listenChannel_); // 添加监听套接字
    //启动线程？
    kRequestCount_ = Count;
    for (int i=0; i<kRequestCount_; i++){
        queue_.push_back(new Request);
    }
    start();

}


HttpServer::~HttpServer() {

}


void HttpServer::start() {

    pool_ = new ThreadPool;

    pool_->start(2);
    Log(L_DEBUG) << "thread pool init OK";
    //在这里启动线程
    // 之后进入主循环
    Log(L_DEBUG) << "enter loop";
    poller_->loop();
}


void HttpServer::AcceptClient() {
    sockaddr_in remote_addr{};
    memset(&remote_addr, 0, sizeof remote_addr);
    socklen_t remote_addr_sz = sizeof remote_addr;
    int sockfd = ::accept(listenfd_, (sockaddr*)&remote_addr, &remote_addr_sz);
    Log(L_DEBUG) << "accept new client" << sockfd;
    if (sockfd < 0)
        Log(L_ERROR) << "accept fd error";
    else{
        if (queue_.empty()){
            expand();
            std::cout << "TODO:expand()" << std::endl;
        }
        auto request = queue_.front();
        queue_.pop_front();
        assert(!request->channel_.getIsUsed()); // channel是未使用状态
        request->init(sockfd, EPOLLIN | EPOLLET | EPOLLONESHOT);
        /* 设置回调函数 */
        request->channel_.setReadCallBack(
                std::bind(&HttpServer::readEvent, this, &request->channel_));
        request->channel_.setWriteCallBack(
                std::bind(&HttpServer::writeEvent, this, &request->channel_));
        request->channel_.setErrorCallBack(
                std::bind(&HttpServer::errorEvent, this, &request->channel_));
        poller_->update(EPOLL_CTL_ADD, &request->channel_); // 添加到epoll中去
        auto iter = map_.find(&request->channel_);
        assert(iter == map_.end()); // for debug
        mapNode node(&request->channel_, request);
        map_.insert(node);
    }
}


void HttpServer::readEvent(Channel* channel) {
    auto iter = map_.find(channel);
    assert(iter != map_.end());
    if (iter->second->read()) {
        pool_->put(
                std::bind(&HttpServer::process, this, iter->second)
        ); // 解析HTTP
    }else{
        /* close */
        iter->second->close_conn();
        poller_->update(EPOLL_CTL_DEL, iter->first); // 从epoll中删除
        map_.erase(iter); // 清空map_
        queue_.push_back(&*iter->second); // 重新加入到队列中去
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

}

void HttpServer::errorEvent(Channel *channel) {
    auto iter = map_.find(channel);
    assert(iter != map_.end() );
    iter->second->close_conn();
    poller_->update(EPOLL_CTL_DEL, iter->first);
    map_.erase(iter);
    queue_.push_back(&*iter->second);

}


void HttpServer::writeEvent(Channel* channel) {
    auto iter = map_.find(channel);
    assert(iter != map_.end());
    WriteProcess ret = iter->second->write();
    if ( ret == WriteIncomplete){
        poller_->update(EPOLL_CTL_MOD, channel);
        return;
    }
    if ( ret == WriteError ){
        /* 出错，直接进行删除操作 */
        iter->second->close_conn();
        poller_->update(EPOLL_CTL_DEL, channel);
        map_.erase(iter);
        queue_.push_back(&*iter->second);
        return;
    }
    if ( ret == WriteOk ){
        /* keep-alive状态和正常关闭?，设置定时器？ */
        return;
    }
}


void HttpServer::expand() {}
void HttpServer::reduce() {}




