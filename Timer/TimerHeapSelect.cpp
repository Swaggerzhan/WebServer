//
// Created by swagger on 2021/4/10.
//

#include "TimerHeap.h"



TimerNode::TimerNode(time_t expire, Request* request) {
    this->expire = expire;
    /* 保存用户信息 */
    client_data = request;
    call_bak = Request::time_out;

}


TimerHeap::TimerHeap() {
    this->count = 0;
    /* 申请空间 */
    //this->heap = new TimerNode[OPENMAX];
    this->maxOpen = OPENMAX + 1;
}


TimerHeap::~TimerHeap() {
    //delete [] heap;
    for (int i=0; i<count; ++i)
        delete heap[i];
}


bool TimerHeap::insert(TimerNode* target) {
    if (target == nullptr || count >= maxOpen)
        return false;
    heap[count + 1] = target;
    /* 将节点移动到所在的地方 */
    shiftUp(count);
    count ++;
    return true;

}


void TimerHeap::shiftUp(int index) {
    while (index > 1){
        if (heap[index]->expire < heap[index / 2]->expire)
            swap(&heap[index], &heap[index/2]);
        index /= 2;
    }
}


void TimerHeap::swap(TimerNode **left, TimerNode **right) {
    TimerNode* tmp = *left;
    *left = *right;
    *right = tmp;
}


TimerNode* TimerHeap::pop() {
    if ( count <= 0 )
        return nullptr;
    TimerNode* retNode = heap[1];
    swap(&heap[1], &heap[count]);
    count --; // 维护count数量
    /* 将堆最后面的节点移动上来后再将其shiftDown移动到该到的地方去 */
    shiftDown(1);
    return retNode;
}


void TimerHeap::shiftDown(int index) {

    int child = index * 2;
    while (child <= count){
        /* 从孩子中找出最小的时间值，跟最小的时间值更换 */
        if (child + 1 <= count )
            if (heap[child + 1]->expire < heap[child]->expire)
                child ++;
        /* 如果index的时间比较大，就往下移动 */
        if (heap[index]->expire > heap[child]->expire)
            swap(&heap[index], &heap[child]);

        /* 继续递归往下寻找 */
        index = child;
        child = index * 2;
    }
}


void TimerHeap::tick() {
    printf("tick tick!..\n");
    /* 处理超时的时间节点 */
    TimerNode* timerNode;
    while( isTimeOut() ){
        timerNode = pop();
        /* 调用超时回调函数，并将参数传入 */
        timerNode->call_bak(timerNode->client_data);
        delete timerNode;
        /* timerNode->data也是指针 */
    }
    printf("time out node handle finished\n");

}


bool TimerHeap::isTimeOut() {
    if (count <= 0)
        return false;

    TimerNode* timerNode = heap[1];
    /* 小于当前时间，超时处理 */
    if (timerNode->expire < time(nullptr))
        return true;
    return false;
}


int TimerHeap::size() {
    return count;
}



//这里才是定义
TimerHeap *timer;

