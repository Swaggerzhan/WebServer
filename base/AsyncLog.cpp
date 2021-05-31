//
// Created by swagger on 2021/5/29.
//

#include "AsyncLog.h"
#include "Buffer.h"
#include <cassert>
#include <fcntl.h>
#include <functional>
#include <unistd.h>


const int AsyncLog::kRingLen = 8; // 默认环长度为8
const std::string AsyncLog::dir_ = "/log/WebServer/";

struct threadData{
    threadData(std::function<void()> cb)
    :   cb_(std::move(cb))
    {}
    std::function<void()> cb_;
};


void* thread_entry(void* arg){
    auto data = (threadData*)arg;
    data->cb_(); // 启动任务
    delete data;
    return nullptr;
}


AsyncLog::AsyncLog()
:   productMutex_(),
    logMutex_(),
    logCond_(logMutex_),
    productCond_(productMutex_)
{
    std::string file = dir_ + "info.log";
    fd = open(file.c_str(), O_CREAT | O_RDWR);
    assert(fd > 0);

    auto dummyHead = new Buffer;
    auto preNode = dummyHead;
    Buffer* curNode = nullptr;

    for (int i=0; i<kRingLen; i++){
        curNode = new Buffer;
        curNode->pre_ = preNode;
        preNode->next_ = curNode;
        preNode = curNode;
        curNode = curNode->next_;
    }
    tail_ = preNode;
    head_ = dummyHead->next_;
    delete dummyHead;
    head_->pre_ = tail_;
    tail_->next_ = head_;
    logPtr = head_;
    productPtr = head_;
    count_ = kRingLen;
    start();

}


AsyncLog::~AsyncLog() {
    stop();
    close(fd);
    Buffer* cur = productPtr;
    for (int i=0; i<count_; i++){
        Buffer* next = cur->next_;
        delete cur;
        cur = next;
    }
}

void AsyncLog::append(std::string &msg) {
    MutexLockGuard lock(productMutex_);
    /* 添加消息，失败为false，则改变后尝试 */
    while (!productPtr->add(msg)){
        changeBuffer();
        logCond_.notify(); // 叫醒后端线程持久化
    }

}
/* 改变生产者线buffer
 * no thread safe */
void AsyncLog::changeBuffer() {
    if (productPtr->next_ == logPtr){ // 生产者线程追上消费者
        /* 分配新的空间 */
        auto newBuffer = new Buffer;
        newBuffer->next_ = productPtr->next_;
        productPtr->next_ = newBuffer;
        newBuffer->next_->pre_ = newBuffer;
        newBuffer->pre_ = productPtr;
        ++ count_; // buffer总数+1
    }
    productPtr = productPtr->next_; // 指向下一个buffer
}
/* thread safe
 * 持久化buffer到磁盘中 */
void AsyncLog::persist(){
    MutexLockGuard lock(logMutex_);
    /* 如果生产者和消费者写相同buffer，就等待 */
    bool reduce = false;
    while (productPtr == logPtr){
        logCond_.waitTime(3);
        // 尝试等待3秒，如果还没叫醒就强制更换生产者指针
        MutexLockGuard lock2(productMutex_);
        changeBuffer();
        if (count_ > kRingLen)
            reduce = true;
    }
    logPtr->write(fd); // 持久化buffer
    /* 当前块已经写完，指向下一块 */
    if (reduce){
        auto delNode = logPtr;
        auto preNode = logPtr->pre_;
        auto nxtNode = logPtr->next_;
        preNode->next_ = nxtNode;
        nxtNode->pre_ = preNode;
        delete delNode;
        logPtr = nxtNode;
        -- count_;
    }else {
        logPtr = logPtr->next_;
    }

}


void AsyncLog::reduceBuffer() {

}

/* 线程启动入口 */
void AsyncLog::start() {
    auto data = new threadData(std::bind(&AsyncLog::demo, this));
    quit_ = false;
    pthread_create(&logThread_, nullptr, thread_entry, data);
}


void AsyncLog::demo() {

    while ( !quit_ ){
        persist();
    }

}


void AsyncLog::stop() {
    MutexLockGuard lock1(productMutex_);
    MutexLockGuard lock2(logMutex_);
    quit_ = true;
}


