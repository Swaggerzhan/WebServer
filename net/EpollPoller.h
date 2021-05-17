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

    /* 更新channel，中调用update来改变epoll中的情况 */
    void updateChannel(Channel *channel);
    /* 删除channel，除了删除epoll中的channel，还需要从channel_中删除 */
    void removeChannel(Channel *channel);

    /* 更新epoll状态，真正操作EPOLL的函数 */
    void update(int option, Channel* channel);

    /* epoll_wait的封装 */
    void poll(std::vector<Channel*> *activeChannels);

    /* 获取到活跃的channel */
    void getActChannel(int nums, std::vector<Channel*> *activeChannels);

    bool hasChannel(Channel* channel);



private:

    int epfd_; // epoll接口
    std::vector<Channel*> listen_list_;
    std::map<int, Channel*> channel_; // 通过fd索引channel
    std::vector<struct epoll_event> events_; // 用来保存返回的EPOLL结构体

    static const int kNew; // channel对于EPOLL是新的，对于channel_也是新的
    static const int kAdded; // channel已经存在于EPOLL和channel_中了
    static const int kDelete; // channel存在于channel_中，不存在于EPOLL中

};


#endif //WEBSERVER_EPOLLPOLLER_H
