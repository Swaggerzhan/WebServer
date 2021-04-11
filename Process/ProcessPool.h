//
// Created by swagger on 2021/4/11.
//

#ifndef WEBSERVER_PROCESSPOOL_H
#define WEBSERVER_PROCESSPOOL_H


#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <wait.h>
#include <sys/epoll.h>
#include "../utility.h"


class ProcessPool;
class Process;

/* 信号管道 */
static int sig_pipe[2];

class ProcessPool {

private:

    /**
     *
     * @param max_process 进程总数
     * @param demo_socket 监听套接字
     */
    ProcessPool(int max_process, int demo_socket);

    /**
     * 父进程主循环
     */
    void run_father();

    /**
     * 子进程主循环
     */
    void run_child();

public:

    /**
     * 进程池的获取方法
     * @return
     */
    static ProcessPool* getPool(int max_process, int demo);


    static void setup_sig_pipe();


    /**
     * 进程池启动函数
     */
    void RUN(int idx);


public:

    /* 进程池实例 */
    static ProcessPool* instance;

    /* 进程池中线程总数量 */
    static int m_process_max;

    /* 进程数组 */
    static Process** m_process_list;

    /* 监听socket，提前给定 */
    static int demo;

    /* 每个进程中都有的epoll fd */
    static int epfd;

    /* epoll最多监听数量 */
    static int maxOpen;

    /* epoll监听数组 */
    static epoll_event* epollArray;

    /* 循环停止 */
    static bool m_stop;

    /* 进程在数组中的索引 */
    static int m_idx;

};


class Process{


public:

    Process();


public:

    /* 进程id */
    pid_t c_proc_id;

    /* 进程间通讯管道 */
    int pipe[2];

    /* 进程在进程池中的索引 */
    int m_idx; /* 值为-1表示在父进程中 */


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

/**
 * 信号处理函数
 * 采用统一事件源的形式
 * @param sig
 */
static void signal_handler(int sig);

/**
 * 注册信号
 * @param sig
 * @param handler
 * @param restart
 */
static void addSignal(int sig, void (handler)(int), bool restart = true);


#endif //WEBSERVER_PROCESSPOOL_H
