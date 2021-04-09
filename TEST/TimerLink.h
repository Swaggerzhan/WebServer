//
// Created by swagger on 2021/4/9.
//

#ifndef WEBSERVER_TIMERLINK_H
#define WEBSERVER_TIMERLINK_H

#include <signal.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <sys/unistd.h>
#include "../utility.h"
#include <iostream>


using std::cout;
using std::endl;


struct WorkData;



/**
 *  测试总函数 
 **/
void TimerTest();


/**
 * 封装后的信号注册
 **/
void addsig(int sig);


/**
 *  信号注册函数
 **/ 
void sig_handler(int sig);


/*
** 回调函数
*/
void call_back(WorkData *);


struct WorkData{
    int workTime;
    int workId;

    WorkData(int workId){
        workId = workId;
    };
};
class TimerLink;
class TimerNode;

class TimerNode{
public:

    TimerNode();

    time_t expire; /* 超时时间(绝对时间) */
    void (*call_back)(WorkData*); /* 超时时的回调函数 */
    WorkData* data;
    TimerNode* pre;
    TimerNode* next;

};



class TimerLink {

public:
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


static TimerLink timerLink;
static int testNumber = 97;

#endif //WEBSERVER_TIMERLINK_H
