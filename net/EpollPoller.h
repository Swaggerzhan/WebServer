//
// Created by swagger on 2021/5/15.
//

#ifndef WEBSERVER_EPOLLPOLLER_H
#define WEBSERVER_EPOLLPOLLER_H
#include <sys/epoll.h>
#include <vector>
#include <map>

class Channel;

class EpollPoller {

public:

    EpollPoller(){
        epfd_ = epoll_create(5);
    }


    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    /* 更新epoll状态 */
    void update(int option, Channel* channel);

    void poll(std::vector<Channel*> *activeChannels);


    /* 获取到活跃的channel */
    void getActChannel(int nums, std::vector<Channel*> *activeChannels);


private:

    int epfd_; // epoll接口
    std::vector<Channel*> listen_list_;
    std::map<int, Channel*> channel_; // 通过fd索引channel
    std::vector<struct epoll_event> events_; // 用来保存返回的EPOLL结构体

    static const int kNew;
    static const int kAdded;
    static const int kDelete;

};


#endif //WEBSERVER_EPOLLPOLLER_H
