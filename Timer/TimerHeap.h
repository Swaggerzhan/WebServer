//
// Created by swagger on 2021/4/10.
//

#ifndef WEBSERVER_TIMERHEAP_H
#define WEBSERVER_TIMERHEAP_H

#include <csignal>
#include <sys/poll.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <ctime>
#include "../utility.h"
#include "../HTTP/Request.h"



//TODO:缺少更新定时器的方法

/*
 * 信号注册函数
 * */
void addSig(int sig);



void sig_handler(int sig);

class TimerNode;

/**
 * 定时器
 * 使用最小堆实现
 */
class TimerHeap {

private:

    TimerNode* heap[OPENMAX]; // 堆数据
    int count; // 当前堆总数
    int maxOpen; /* 堆最大容量 */

public:
    /**
     * 构造方法
     * 这里的最大堆实现采用 index从 1 开始
     */
    TimerHeap();

    ~TimerHeap();

    /* 返回堆大小 */
    int size();

    /**
     * 时间节点的添加
     * @return
     */
    bool insert(TimerNode*);


    /**
     * 将堆最顶端的节点推出
     * 这里并没有处理的删除问题，需要调用者自行删除
     * @return
     *
     */
    TimerNode* pop();



    /**
     * 心跳
     */
    void tick();


    /**
     * 交换值
     * @param left
     * @param right
     */
    static void swap(TimerNode** left, TimerNode** right);

private:

    /**
     * 将对应的index下节点向上移动到该到的地方
     */
    void shiftUp(int index);


    /**
     * 将对应的index的节点向下移动到该到的地方
     * @param index
     */
    void shiftDown(int index);


    /**
     * 检测堆最上层是否已经超时了
     * 如果此时堆为空返回false
     * @return
     */
    bool isTimeOut();

};


class TimerNode{
public:

    time_t expire; /* 超时时间 */
    void (*call_bak)(void*); /* 回调函数方法 */
    Request* client_data;/* 客户信息 */

public:
    TimerNode(time_t expire, Request* request);
};


extern TimerHeap* timer;


#endif //WEBSERVER_TIMERHEAP_H
