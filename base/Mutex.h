//
// Created by swagger on 2021/5/17.
//

#ifndef WEBSERVER_MUTEX_H
#define WEBSERVER_MUTEX_H

#include <pthread.h>


class MutexLock{

public:

    MutexLock()
    :   holder_(0)
    { pthread_mutex_init(&mutex_, nullptr);}

    ~MutexLock(){
        pthread_mutex_destroy(&mutex_);
    }

    void lock(){
        pthread_mutex_lock(&mutex_);
        holder_ = pthread_self(); // 当前拿到锁的ID
    }

    void unlock(){
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    bool isLockedByThisThread(){
        return pthread_self() == holder_;
    }

    pthread_t getLockedThread(){
        return holder_;
    }

    pthread_mutex_t* getMutex(){
        return &mutex_;
    }

private:
    mutable pthread_mutex_t mutex_; // 互斥锁
    pthread_t holder_; // 锁的线程拥有者
};


class MutexLockGuard{
public:
    MutexLockGuard(MutexLock& mutex)
    :   mutex_(mutex)
    { mutex_.lock(); }

    ~MutexLockGuard(){
        mutex_.unlock();
    }

private:

    MutexLock &mutex_;
};




#endif //WEBSERVER_MUTEX_H
