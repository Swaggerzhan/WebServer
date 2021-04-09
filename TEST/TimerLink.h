//
// Created by swagger on 2021/4/9.
//

#ifndef WEBSERVER_TIMERLINK_H
#define WEBSERVER_TIMERLINK_H

#include <signal.h>
#include <ctime>
#include <sys/unistd.h>


void TimerTest();


struct WorkData{
    int workTime;
};
class TimerLink;
class TimerNode;

class TimerNode{
public:

    TimerNode();

    time_t expire; /* 超时时间(绝对时间) */
    void *(call_back)(void*); /* 超时时的回调函数 */
    WorkData* data;
    TimerNode* pre;
    TimerNode* next;

};



class TimerLink {

private:
    TimerNode* head;
    TimerNode* tail;


public:

    TimerLink();

    /**
     * 心跳，每次调用tick都检查链表中超时的时间节点，并调用其回调函数
     * 之后将其从链表中删除出去
     */
    void tick();

    /**
     * 添加时间节点
     * @param target
     * @return
     */
    bool addTimer(TimerNode* target);


    /**
     * 删除时间节点
     * @param target
     */
    void delTimer(TimerNode* target);


};


#endif //WEBSERVER_TIMERLINK_H
