//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H


#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <csignal>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <queue>
#include <fcntl.h>
#include <cstring>
#include "../utility.h"
#include "../HTTP/Request.h"


class ThreadPool;
class ThreadZ;


class ThreadPool {

private:

    /**
     * 线程池
     */
    ThreadPool(int);

public:

    /**
     * 获取线程池
     * 传入线程数量
     * @return
     */
    static ThreadPool* getPool(int);

    /**
     * 向队列中添加工作事项
     * @param request
     * @return
     */
    static bool append(Request* request);

    /**
     * 从工作队列中取出任务并执行
     */
    void run();

public:

    /* 控制线程的停止 */
    bool t_stop = false;

    /* 线程池总数量 */
    static int m_thread_num;

    /* 线程池数组 */
    static ThreadZ** m_list;

    /* 访问队列所需要的锁 */
    static pthread_mutex_t lock;

    /* 线程池实例 */
    static ThreadPool* instance;

    /* 请求队列 */
    static std::queue<Request*> requestLine;



};


class ThreadZ{

public:

    ThreadZ(int);


    static void *work(void* arg);



public:

    /* 线程索引 */
    int t_idx;

    /* 线程ID */
    pthread_t pthread_id;



};



/**
 * 将fd设置为非堵塞型套接字
 * @param fd
 * @return
 */
static int setNonBlock(int fd);


/**
 * 将fd加入epoll监听
 * @param epfd
 * @param sock
 */
static void addFd(int epfd, int sock);


/**
 * 将fd从epoll监听中移除
 * @param epfd
 * @param sock
 */
static void removeFd(int epfd, int sock);


#endif //WEBSERVER_THREADPOOL_H
