//
// Created by swagger on 2021/5/19.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H


#include <pthread.h>
#include <functional>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstring>


namespace CurrentThread{

    class Thread;

    extern __thread int t_cacheId; // 缓存线程ID
    extern __thread char t_threadName[32]; // 线程名称
    extern __thread int t_threadNameLength; // 线程名称长度

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


    struct ThreadData{
        std::function<void()> cb_;
        std::string name_;
        pid_t* tid_;

        ThreadData(std::function<void()> cb, std::string name, pid_t* tid)
        :   cb_(std::move(cb)),
            tid_(tid),
            name_(std::move(name))
        {}
    };

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


class Thread {
public:

    typedef std::function<void()> Functor;
    Thread(Functor cb, std::string name);
    ~Thread();
    void start(); // 线程启动
    std::string ThreadName() const;

    //void join();

private:

    bool isStart_; // 线程是否启动
    bool isJoin_;

    std::string name_; // 线程名称

    Functor func_; // 线程启动后的回调函数
    pthread_t threadId_; // 线程ID
    pid_t tid_; // linux内核真正ID



};


#endif //WEBSERVER_THREAD_H
