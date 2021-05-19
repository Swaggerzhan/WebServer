//
// Created by swagger on 2021/5/19.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <deque>
#include <functional>
#include "Mutex.h"
#include "Thread.h"
#include "Condition.h"


class ThreadPool {

public:

    typedef std::function<void()> Task;

    ThreadPool();

    ~ThreadPool();

    void start(int thread_nums);

    void run(Task task);

    Task take();

    size_t queueSize() const ;

    void stop(){ quit_ = true; }

private:

    std::deque<Task> queue_; // 任务队列

    mutable MutexLock mutex_; // 互斥锁
    Condition noEmpty_; // 队列非空条件变量

    int thread_nums_;
    bool quit_;
};


#endif //WEBSERVER_THREADPOOL_H
