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


void ThreadPool::run(Task task) {
    MutexLockGuard lock(mutex_);
    queue_.push_back(std::move(task));
    noEmpty_.notify();
}










