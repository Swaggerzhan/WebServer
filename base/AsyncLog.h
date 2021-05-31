//
// Created by swagger on 2021/5/29.
//

#ifndef ASYNCLOG_ASYNCLOG_H
#define ASYNCLOG_ASYNCLOG_H

#include <string>
#include "Mutex.h"
#include "Condition.h"

class Buffer;


class AsyncLog {

public:

    AsyncLog();

    ~AsyncLog();

    /* 添加日志 */
    void append(std::string& msg);
    /* 日志后台守护线程 */
    void start();
    /* 将buffer持久化到磁盘中 */
    void persist();
    void demo();
    void stop();


private:

    /* 改变生产者线buffer
     * no thread safe */
    void changeBuffer();

    /* 尝试删除一些buffer */
    void reduceBuffer();

private:

    int fd; /* 写入文件管道符 */

    Buffer* head_; // 头部节点
    Buffer* tail_; // 尾部节点
    Buffer* logPtr; // 后端指针
    Buffer* productPtr; // 前端消费者线程
    pthread_t logThread_; // 后端线程id
    const static int kRingLen; // 默认环长度

    const static std::string dir_; // 日志存储位置

    MutexLock productMutex_; // 前端锁
    MutexLock logMutex_; // 后端锁
    Condition productCond_; // 前端条件
    Condition logCond_; // 后端条件

    int count_; // buffer总数
    bool quit_;


};


#endif //ASYNCLOG_ASYNCLOG_H
