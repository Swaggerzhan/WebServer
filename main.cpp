#include <iostream>
#include "base/AsyncLog.h"
#include "base/Log.h"
#include "net/HttpServer.h"
#include "net/EpollPoll.h"


int network_init();


//int main(){
//    AsyncLog log;
//    Log::asyncLog = &log; /* 日志初始化 */
//    int demo = network_init();/* 网络初始化 */
//    //TimerHandler timer;
//    int epfd = epoll_create(5);
//    epoll_event *eventArray;
//    eventArray = new epoll_event[OPENMAX];
//    //addfd(epfd, demo, false);
//    epoll_event event{};
//    event.events = EPOLLIN;
//    event.data.fd = demo;
//    epoll_ctl(epfd, EPOLL_CTL_ADD, demo, &event);
//    //event.data.fd = TimerHandler::pipe2io_thread[0];
//    //epoll_ctl(epfd, EPOLL_CTL_ADD, TimerHandler::pipe2io_thread[0], &event);
//    /* 获取线程池 */
//    ThreadPool* pool = ThreadPool::getPool(epfd, 2);
//    /* 客户端初始化，20000个上限 */
//    auto *user = new Request[OPENMAX];
//    Request::bufInit(); // 将主页提前载入内存中
//    Request::epfd = epfd;
//    Log(L_INFO) << "server init OK!";
//    bool quit = false;
//
//    while ( !quit ){
//        int ret = epoll_wait(epfd, eventArray, OPENMAX, 10);
//        if ( ret < 0 ){
//            /* 调试系统中断 */
//            if ( errno == EINTR )
//                continue;
//            exit_error("epoll_wait()", true,1);
//        }
//        /* 处理链接 */
//        for (int i=0 ;i<ret; ++i){
//            int sockfd = eventArray[i].data.fd;
//            /* 新链接请求 */
//            if ( (sockfd == demo) && (eventArray[i].events & EPOLLIN )){
//                struct sockaddr_in client_addr{};
//                socklen_t client_addr_sz;
//                int clientfd = accept(demo, (sockaddr*)&client_addr, &client_addr_sz);
//                if (clientfd < 0){
//                    exit_error("accept()", false);
//                    continue;
//                }
//                /* 初始化Request类 */
//                user[clientfd].init(clientfd);
//                continue;
//            } else if ( eventArray[i].events & EPOLLIN) {
//                /* 先尝试去读取数据，如果读取数据失败就直接关闭连接 */
//                if (user[sockfd].read()) {
//                    /* 读取成功就将数据扔到池中由其他线程池接管 */
//                    ThreadPool::append(user + sockfd);
//                    //TimerHandler::update(user + sockfd);
//                } else {
//                    //TimerHandler::heap.delNode(user + sockfd);
//                    user[sockfd].close_conn();
//                }
//                continue;
//            } else if ( eventArray[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)){
//                /* 出错关闭 */
//                printf("EPOLLERR | EPOLLHUP | EPOLLRDHUP\n");
//                /* 删除定时器 */
//                //TimerHandler::heap.delNode(user + sockfd);
//                user[sockfd].close_conn();
//                continue;
//            }else if ( eventArray[i].events & EPOLLOUT ){
//                /* 处理上次没有写完的数据 */
//                if (!user[sockfd].write()){
//                    /* 出错关闭 */
//                }
//
//                continue;
//            }
//        }
//    }
//
//
//}


int main(){
//    AsyncLog log;
//    Log::asyncLog = &log;
    HttpServer::netWorkInit(80);
    // channel count
    EpollPoll poller(10000);
    HttpServer httpServer(&poller);
    httpServer.start();
}


/**
 * 网络初始化，返回监听套接字
 * @return
 */
//int network_init(){
//    int demo = socket(AF_INET, SOCK_STREAM, 0);
//
//    struct sockaddr_in local_addr;
//    memset(&local_addr, 0, sizeof(local_addr));
//    local_addr.sin_addr.s_addr = inet_addr(HOST);
//    local_addr.sin_port = htons(PORT);
//    local_addr.sin_family = AF_INET;
//    /* 设置重复使用地址便于测试 */
//    int reuse = 1;
//    setsockopt(demo, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
//    /* 将地址和套接字绑定 */
//    if (bind(demo, (sockaddr*)&local_addr, sizeof(local_addr)) == -1){
//        printf("bind() error!\n");
//        exit(1);
//    }
//    /* 监听端口 */
//    if ( listen(demo, 5) == -1){
//        printf("listen() error!\n");
//        exit(1);
//    }
//    return demo;
//}

