//
// Created by swagger on 2021/5/29.
//

#include "Buffer.h"
#include <unistd.h>
#include <cassert>

const int Buffer::kWriteCount_ = 10;
const size_t Buffer::kLength_ = 1024 * 8;

Buffer::Buffer()
:   next_(nullptr),
    pre_(nullptr)
{
    buffer_ = new char[kLength_];
    cur_ = buffer_; // 游动指针
    reMain_ = kLength_; // 剩余长度
    isFull_ = false;
}

Buffer::~Buffer(){
    //std::cout << "delete buffer" << std::endl;
    delete buffer_;
}


bool Buffer::add(std::string &msg) {
    if (isFull())
        return false;
    if (msg.length()+1 > reMain_){
        setFull();
        return false;
    }
    assert(!writing_);
    msg[msg.length()] = '\n'; // 将\0去除，改成\n
    size_t len = msg.length() + 1;
    memcpy(cur_, msg.c_str(), len);
    cur_ += len;
    reMain_ -= len;
    return true;
}


void Buffer::reset() {
    isFull_ = false;
    cur_ = buffer_;
    reMain_ = kLength_;
}

size_t Buffer::write(int fd){
    writing_ = true;
    size_t len = ::write(fd, buffer_, length());
    assert(len == length());
    writing_ = false;
    reset();
    return len;
}
