#include <iostream>
#include "Thread/ThreadPool.h"




int main(){

    int demo = network_init();/* 网络初始化 */

    int epfd = epoll_create(5);
    epoll_event *eventArray;
    eventArray = new epoll_event[OPENMAX];
    addfd(epfd, demo, false);
    /* 获取线程池 */
    ThreadPool* pool = ThreadPool::getPool(epfd, 2);
    /* 客户端初始化，20000个上限 */
    auto *user = new Request[OPENMAX];
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
                /* 初始化Request类 */
                user[clientFd].init(clientFd);
            }else if ( eventArray[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)){
                /* 出错关闭 */
                user[sockfd].close_conn();
            } else if ( eventArray[i].events & EPOLLIN){
                /* 先尝试去读取数据，如果读取数据失败就直接关闭连接 */
                if ( user[sockfd].read()){
                    /* 读取成功就将数据扔到池中由其他线程池接管 */
                    ThreadPool::append( user + sockfd );
                }else{
                    user[sockfd].close_conn();
                }
            }else if ( eventArray[i].events & EPOLLOUT ){
                /* 处理上次没有写完的数据 */
                if (!user[sockfd].write()){
                    /* 出错关闭 */
                    user[sockfd].close_conn();
                }
            }
        }
    }


}
