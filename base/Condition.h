//
// Created by swagger on 2021/5/17.
//

#ifndef WEBSERVER_CONDITION_H
#define WEBSERVER_CONDITION_H

#include "Mutex.h"

class Condition {
public:

    Condition(MutexLock& mutex)
    : mutex_(mutex)
    { pthread_cond_init(&cond_, nullptr);}

    /* 等待条件变量 */
    void wait(){
        pthread_cond_wait(&cond_, mutex_.getMutex());
    }
    /* 唤醒部分线程 */
    void notify(){
        pthread_cond_signal(&cond_);
    }

    void waitTime(int sec){
        struct timespec wait{};
        clock_gettime(CLOCK_REALTIME, &wait);
        wait.tv_sec += sec;
        pthread_cond_timedwait(&cond_, mutex_.getMutex(), &wait);
    }

    /* 叫醒全部线程 */
    void notifyAll(){
        pthread_cond_broadcast(&cond_);
    }


private:

    pthread_cond_t cond_; // 条件变量
    MutexLock &mutex_; // 互斥锁

};


#endif //WEBSERVER_CONDITION_H
