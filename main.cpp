#include <iostream>
#include "Thread/ThreadPool.h"



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
    printf("network init ok!\n");

    /* 获取线程池 */

    printf("pool ok\n");

    int epfd = epoll_create(5);
    epoll_event *eventArray;
    eventArray = new epoll_event[OPENMAX];

    addFd(epfd, demo);
    ThreadPool* pool = ThreadPool::getPool(epfd, 2);
    while( true ){
        int ret = epoll_wait(epfd, eventArray, OPENMAX, 0);
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
                while (!ThreadPool::append(request)){}
                continue;
            }
        }
    }

}
