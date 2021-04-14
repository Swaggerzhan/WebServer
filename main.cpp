#include <iostream>
#include "Thread/ThreadPool.h"
#include <map>


/* 进程调试部分 */
//int main() {
//
//    int demo = socket(AF_INET, SOCK_STREAM, 0);
//
//    struct sockaddr_in local_addr;
//    memset(&local_addr, 0, sizeof(local_addr));
//    local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
//    local_addr.sin_port = htons(7999);
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
//    printf("network init ok!\n");
//    ProcessPool* pool = ProcessPool::getPool(4, demo);
//    pool->RUN();
//
//
//
//}

int main_thread(){
    int demo = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    local_addr.sin_port = htons(7999);
    local_addr.sin_family = AF_INET;
    /* 设置重复使用地址便于测试 */
    int reuse = 1;
    setsockopt(demo, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    /* 将地址和套接字绑定 */
    if (bind(demo, (sockaddr*)&local_addr, sizeof(local_addr)) == -1){
        printf("bind() error!\n");
        exit(1);
    }
    /* 监听端口 */
    if ( listen(demo, 5) == -1){
        printf("listen() error!\n");
        exit(1);
    }
    printf("network init ok!\n");

    /* 获取线程池 */

    printf("pool ok\n");

    int epfd = epoll_create(5);
    epoll_event *eventArray;
    eventArray = new epoll_event[OPENMAX];

    addFd(epfd, demo);

    ThreadPool* pool = ThreadPool::getPool(epfd, 2);
    /* index.html初始化 */
    Request::bufInit();
    while( true ){
        int ret = epoll_wait(epfd, eventArray, OPENMAX, 0);
        //printf("main loop\n");
        if (ret < 0){
            printf("epoll_wait() error!\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
        /* 处理请求 */
        for (int i=0; i<ret; i++){
            int handler_fd = eventArray[i].data.fd;
            /* 处理新请求 */
            if ( (handler_fd == demo) && (eventArray[i].events & EPOLLIN )){
                struct sockaddr_in client_addr{};
                socklen_t client_addr_sz;
                int clientFd = accept(demo, (sockaddr*)&client_addr, &client_addr_sz);
                if (clientFd < 0){
                    printf("accept() error!\n");
                    printf("%s\n", strerror(errno));
                    continue;
                }
                //printf("new client connection!\n");
                /* 将新用户加入到epoll监听端口中去 */
                addFd(epfd, clientFd);
            } else {
                /* 用户发起请求，主线程只是将其加入到队列中去 */
                Request* request = new Request(handler_fd);
                ThreadPool::append(request);
                continue;
            }
        }
    }

}

std::map<int, Request*> fd_request; /* 建立fd和Request之间的关系 */
pthread_rwlock_t global_lock = PTHREAD_RWLOCK_INITIALIZER;



int main(){
    int demo = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    local_addr.sin_port = htons(7999);
    local_addr.sin_family = AF_INET;
    /* 设置重复使用地址便于测试 */
    int reuse = 1;
    setsockopt(demo, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    /* 将地址和套接字绑定 */
    if (bind(demo, (sockaddr*)&local_addr, sizeof(local_addr)) == -1){
        printf("bind() error!\n");
        exit(1);
    }
    /* 监听端口 */
    if ( listen(demo, 5) == -1){
        printf("listen() error!\n");
        exit(1);
    }
    addSig(SIGALRM); /* 添加信号量 */
    alarm(10);/* 10秒后的闹钟 */
    timer = new TimerHeap;/* 定时器初始化 */



    printf("init ok!\n");


    int epfd = epoll_create(5);
    epoll_event *eventArray;
    eventArray = new epoll_event[OPENMAX];
    addFd(epfd, demo);
    /* 获取线程池 */
    ThreadPool* pool = ThreadPool::getPool(epfd, 2);
    while ( true ){
        int ret = epoll_wait(epfd, eventArray, OPENMAX, 0);
        if ( ret < 0 ){
            printf("epoll_wait() error!\n");
            exit(1);
        }
        /* 处理链接 */
        for (int i=0 ;i<ret; ++i){
            int sockfd = eventArray[i].data.fd;
            /* 新链接请求 */
            if ( (sockfd == demo) && (eventArray[i].events & EPOLLIN )){
                struct sockaddr_in client_addr{};
                socklen_t client_addr_sz;
                int clientFd = accept(demo, (sockaddr*)&client_addr, &client_addr_sz);
                if (clientFd < 0){
                    printf("accept() error!\n");
                    printf("%s\n", strerror(errno));
                    continue;
                }
                /* 将新用户加入到epoll监听端口中去 */
                addFd(epfd, clientFd);
                Request* request = new Request(clientFd);
                pthread_rwlock_wrlock(&global_lock); // 写锁
                fd_request[clientFd] = request;
                pthread_rwlock_unlock(&global_lock); // 解锁
                /* 将超时时间设为10秒后 */
                TimerNode* node = new TimerNode(time(nullptr) + 10, request);
                timer->insert(node);//加入定时器
            }else {
                /* 其他链接请求 */
                /* 建立起fd和Request的关系 */
                pthread_rwlock_rdlock(&global_lock);// 读锁
                ThreadPool::append(fd_request.find(sockfd)->second);
                pthread_rwlock_unlock(&global_lock);
            }
        }
    }


}
