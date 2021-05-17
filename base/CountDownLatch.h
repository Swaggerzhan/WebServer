//
// Created by swagger on 2021/5/17.
//

#ifndef WEBSERVER_COUNTDOWNLATCH_H
#define WEBSERVER_COUNTDOWNLATCH_H

#include "Condition.h"

class CountDownLatch {
public:
    CountDownLatch(int count)
    :   count_(count),
        mutex_(),
        cond_(mutex_)
    {}

    void wait();

    void countDown();

    int getCount() const ;


private:
    mutable MutexLock mutex_; // 互斥锁
    Condition cond_; // 条件变量
    int count_;

};


#endif //WEBSERVER_COUNTDOWNLATCH_H
