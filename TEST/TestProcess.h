//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_TESTPROCESS_H
#define WEBSERVER_TESTPROCESS_H

#include <csignal>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "../utility.h"


class Process;
class TestProcess;


void addFd(int, int);



class TestProcess {

public:

    TestProcess(int pool_max);

    /**
     * 获取进程池
     * @return
     */
    static TestProcess* getPool(int);


    /* 进程池启动方法 */
    void RUN();

public:

    /* 区分在数组中的进程 */
    static int m_idx;

    /* 进程数组 */
    static Process** m_list;

    /* 进程池总数 */
    static int m_max_process;

    /* 进程池实例 */
    static TestProcess* instance;

    /* epoll监听 */
    static int epfd;

    /* epoll数组 */
    static epoll_event *epollArray;

    static bool m_stop;


private:

    /* 父进程启动方法 */
    void run_father();

    /* 子进程启动方法 */
    void run_child();
};


class Process{

public:

    pid_t m_pid;

    /* 通讯管道 */
    int pipe[2];
};


#endif //WEBSERVER_TESTPROCESS_H
