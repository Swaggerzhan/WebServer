//
// Created by swagger on 2021/5/19.
//

#include "Thread.h"



namespace CurrentThread{

    __thread int t_cacheId = 0;
    __thread char t_threadName[32] = "unknown\0"; // 线程名称
    __thread int t_threadNameLength = 7;

};


Thread::Thread(Thread::Functor cb, std::string name)
:   func_(std::move(cb)),
    name_(std::move(name)),
    isStart_(false),
    isJoin_(false)
{

}

std::string Thread::ThreadName() const {
    return name_;
}


void Thread::start() {

    isStart_ = true; // 启动线程
    auto data = new CurrentThread::ThreadData(
            std::move(func_),
            name_,
            &tid_);
    /* 启动线程 */
    pthread_create(&threadId_, nullptr, &CurrentThread::entry, data);


}


