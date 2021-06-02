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

    pid_t getTid();

    void cacheTid();

    /* 获取tid */
    inline pid_t tid();


    struct ThreadData{
        std::function<void()> cb_;
        std::string name_;
        pid_t* tid_;

        ThreadData(std::function<void()> cb, std::string name, pid_t* tid)
        :   cb_(cb),
            tid_(tid),
            name_(name)
        {}
    };

    /* 线程真实启动函数，将线程数据传入，之后启动回调函数 */
    void* entry(void* arg);


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
