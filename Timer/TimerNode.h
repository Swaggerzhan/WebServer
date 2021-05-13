//
// Created by swagger on 2021/5/10.
//

#ifndef TIMER_TIMERNODE_H
#define TIMER_TIMERNODE_H

#include <sys/time.h>




class TimerNode{
public:

    //time_t expire; /* 超时时间 */

    timeval expire;

    void (*call_bak)(void*); /* 回调函数方法 */

    void *client_data;/* 客户信息 */

public:

    bool operator < (const TimerNode &rightNode){
        if (this->expire.tv_sec == rightNode.expire.tv_sec)
            return this->expire.tv_usec < rightNode.expire.tv_usec;
        return this->expire.tv_sec < rightNode.expire.tv_sec;
    }

    bool operator < (const timeval &rightTime){
        if (this->expire.tv_sec == rightTime.tv_sec)
            return this->expire.tv_usec < rightTime.tv_usec;
        return this->expire.tv_sec < rightTime.tv_sec;
    }

    bool operator > (const TimerNode &rightNode){

        if (this->expire.tv_sec == rightNode.expire.tv_sec)
            return this->expire.tv_usec > rightNode.expire.tv_usec;
        return this->expire.tv_sec > rightNode.expire.tv_sec;
    }


    TimerNode(timeval expire, void *data, void(*call_bak)(void*));
};


#endif //TIMER_TIMERNODE_H
