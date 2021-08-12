//
// Created by swagger on 2021/6/5.
//

#ifndef WEBSERVER_BLOCKQUEUE_H
#define WEBSERVER_BLOCKQUEUE_H

#include <queue>
#include "Mutex.h"
#include "Condition.h"
#include <iostream>

template <typename T>
class BlockQueue {
public:

    BlockQueue(int capacity):   mutex_(),
                                cond_(mutex_),
                                count_(0),
                                capacity_(capacity)
    {

    }

    ~BlockQueue(){}

    void append(T target){
        MutexLockGuard lock(mutex_);
        queue_.push_back(std::move(target));
        ++ count_;
    }

    T pop(){
        MutexLockGuard lock(mutex_);
        T target = queue_.front();
        if (target == nullptr)
            std::cout << "nullptr" << std::endl;
        queue_.pop_front();
        -- count_;
        return target;
    }

    bool isEmpty() const { return count_ == 0; }

    int size() const { return count_; }


private:

    std::deque<T> queue_;
    MutexLock mutex_;
    Condition cond_;
    int count_;
    int capacity_;

};


#endif //WEBSERVER_BLOCKQUEUE_H
