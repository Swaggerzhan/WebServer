//
// Created by swagger on 2021/5/19.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <deque>
#include <functional>
#include <vector>
#include <memory>
#include "Mutex.h"
#include "Thread.h"
#include "Condition.h"


class ThreadPool {

public:

    typedef std::function<void()> Task;

    ThreadPool();

    ~ThreadPool();

    void start(int thread_nums); // 线程池启动函数

    void runInThread(); // 线程池中线程一只运行在的循环函数所在

    void put(Task task); // 往队列中放置任务

    Task take(); // 从队列中取出任务

    size_t queueSize() const;

    void stop(); // 暂停线程池并且通知所有线程

private:

    std::deque<Task> queue_; // 任务队列

    mutable MutexLock mutex_; // 互斥锁
    Condition noEmpty_; // 队列非空条件变量

    int thread_nums_;
    bool quit_;
    std::vector<std::unique_ptr<Thread>> thread_; // 线程数组，使用智能指针管理
};


#endif //WEBSERVER_THREADPOOL_H
