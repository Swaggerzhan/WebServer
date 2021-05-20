//
// Created by swagger on 2021/5/19.
//

#include "ThreadPool.h"


ThreadPool::ThreadPool()
:   mutex_(),
    noEmpty_(mutex_)
{}

ThreadPool::~ThreadPool() {
    stop();
}

size_t ThreadPool::queueSize() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

ThreadPool::Task ThreadPool::take() {

    MutexLockGuard lock(mutex_);
    Task task;
    while ( queue_.empty() )
        noEmpty_.wait();

    if (!queue_.empty()){
        task = queue_.front();
        queue_.pop_front();
        // 发现任务比较多就帮忙叫醒其他线程
        if (queue_.size() >= 1)
            noEmpty_.notifyAll();
    }
    return task;
}


void ThreadPool::put(Task task) {
    MutexLockGuard lock(mutex_);
    queue_.push_back(std::move(task));
    noEmpty_.notify();
}


void ThreadPool::start(int thread_nums) {
    std::string name = "thread_";
    quit_ = false;
    thread_.reserve(thread_nums); // 提前准备大小
    for (int i=1; i<=thread_nums; i++){
        char buf[10];
        sprintf(buf, "%d", i);
        std::string thread_name = name + buf;
        thread_.emplace_back(new Thread(
                std::bind(&ThreadPool::runInThread, this),
                thread_name
        ));
        thread_[i]->start(); // 启动回调函数
    }
}

/* 线程池中线程运行在的主循环函数 */
void ThreadPool::runInThread() {
    Task task;
    while (!quit_){
        task = take();
        if (task)
            task();
    }
}


void ThreadPool::stop() {
    MutexLockGuard lock(mutex_);
    quit_ = true;
    noEmpty_.notifyAll(); // 叫醒所有线程
}










