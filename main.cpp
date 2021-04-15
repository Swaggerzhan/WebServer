#include <iostream>
#include "Thread/ThreadPool.h"




int main(){

    int demo = network_init();/* 网络初始化 */
    addSig(SIGALRM); /* 添加信号量 */
    alarm(10);/* 10秒后的闹钟 */
    timer = new TimerHeap;/* 定时器初始化 */
    int epfd = epoll_create(5);
    epoll_event *eventArray;
    eventArray = new epoll_event[OPENMAX];
    addFd(epfd, demo);
    /* 获取线程池 */
    ThreadPool* pool = ThreadPool::getPool(epfd, 2);
    /* 客户端初始化，20000个上限 */
    Request *user = new Request[OPENMAX];
    Request::bufInit(); // 将主页提前载入内存中
    Request::epfd = epfd;
    printf("init ok!\n");


    while ( true ){
        int ret = epoll_wait(epfd, eventArray, OPENMAX, 0);
        if ( ret < 0 )
            exit_error("epoll_wait()", true,1);
        /* 处理链接 */
        for (int i=0 ;i<ret; ++i){
            int sockfd = eventArray[i].data.fd;
            /* 新链接请求 */
            if ( (sockfd == demo) && (eventArray[i].events & EPOLLIN )){
                struct sockaddr_in client_addr{};
                socklen_t client_addr_sz;
                int clientFd = accept(demo, (sockaddr*)&client_addr, &client_addr_sz);
                if (clientFd < 0){
                    exit_error("accept()", false);
                    continue;
                }
                /* 将新用户加入到epoll监听端口中去 */
                addFd(epfd, clientFd);
                user[clientFd].init(clientFd);
            }else {
                /* 已经链接用户的请求，直接丢入到进程池中等待其他线程操作 */
                ThreadPool::append(user + sockfd);
                //TODO:处理Request的请求还需细分

            }
        }
    }


}
