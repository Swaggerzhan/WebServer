//
// Created by swagger on 2021/5/19.
//

#include "Thread.h"



namespace CurrentThread{

    __thread int t_cacheId = 0;
    __thread char t_threadName[32] = "unknown\0"; // 线程名称
    __thread int t_threadNameLength = 7;

    pid_t getTid(){
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    void cacheTid(){
        if (t_cacheId == 0)
            t_cacheId = getTid();
    }

    /* 获取tid */
    inline pid_t tid(){
        if (t_cacheId == 0)
            t_cacheId = getTid();
        return t_cacheId;
    }

    /* 线程真实启动函数，将线程数据传入，之后启动回调函数 */
    void* entry(void* arg){
        auto data = static_cast<ThreadData*>(arg);
        *data->tid_ = tid(); // 获取当前线程tid
        strcpy(t_threadName, data->name_.c_str()); // 命名线程名字
        t_threadNameLength = data->name_.length();
        data->cb_(); // 启动回调函数
        strcpy(t_threadName, "finish\0"); // 线程终结
        t_threadNameLength = 6;
        delete data; // 清除内存
        return nullptr;
    }

};


Thread::Thread(Thread::Functor cb, std::string name)
:   func_(cb),
    name_(name),
    isStart_(false),
    isJoin_(false)
{

}

Thread::~Thread() {}

std::string Thread::ThreadName() const {
    return name_;
}


void Thread::start() {

    isStart_ = true; // 启动线程


    auto data = new CurrentThread::ThreadData(
            func_,
            " ",
            &tid_);
    /* 启动线程 */
    pthread_create(&threadId_, nullptr, &CurrentThread::entry, data);


}


