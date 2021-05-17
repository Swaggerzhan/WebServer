//
// Created by swagger on 2021/5/17.
//

#include "CountDownLatch.h"


/* count > 0表示所有线程都还没就绪 */
void CountDownLatch::wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0){
        cond_.wait();
    }
}

/* count减少一，当count最终为0时表示都准备完毕，唤醒所有线程 */
void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0)
        cond_.notifyAll();
}


int CountDownLatch::getCount() const {
    MutexLockGuard lock(mutex_);
    return count_;
}