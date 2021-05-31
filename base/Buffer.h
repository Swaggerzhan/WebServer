//
// Created by swagger on 2021/5/29.
//

#ifndef ASYNCLOG_BUFFER_H
#define ASYNCLOG_BUFFER_H
#include <iostream>
#include <cstring>

/* no thread safe */
class Buffer{
public:

    Buffer* next_;

    Buffer* pre_;

public:

    Buffer();

    ~Buffer();

    /* 尝试往buffer里添加数据，如果长度不够则更换buffer */
    bool add(std::string& msg);

    inline void setFull(){ isFull_ = true; }

    inline size_t peekCap() const { return reMain_; }

    inline bool isFull() const { return isFull_; }

    size_t length() const { return cur_ - buffer_; }

    void reset();

    size_t write(int fd);

private:

    size_t reMain_; // 剩余长度
    const static size_t kLength_; // 总长度
    const static int kWriteCount_; //超过这个写入次数也会强制写入
    char* buffer_; // 缓冲区
    char* cur_; // 当前缓冲区
    bool isFull_; // 是否已满
    bool writing_; // 正在持久化

};


#endif //ASYNCLOG_BUFFER_H
