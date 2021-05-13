//
// Created by swagger on 2021/5/10.
//

#include "TimerHeap.h"



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


bool TimerHeap::insert(Request* request) {
    timeval cur_time{};
    gettimeofday(&cur_time, nullptr);
    cur_time.tv_sec += TIMESLOT;
    TimerNode* target = new TimerNode(cur_time, request, call_back);
    request->node = target;
    if ( count >= maxOpen )
        return false;
    heap[count + 1] = target;
    /* 将节点移动到所在的地方 */
    shiftUp(count);
    count ++;
    std::cout << "adding node, expire time: " << request->node->expire.tv_sec << std::endl;
    return true;
}


void TimerHeap::shiftUp(int index) {
    while (index > 1){
        if (heap[index] < heap[index / 2])
            swap(&heap[index], &heap[index/2]);
        index /= 2;
    }
}


void TimerHeap::swap(TimerNode **left, TimerNode **right) {
    TimerNode* tmp = *left;
    *left = *right;
    *right = tmp;
}


/**
 * 将堆顶元素返回，不考虑节点是否被删除的情况
 * @return
 */
TimerNode* TimerHeap::pop() {
    while ( !isEmpty() ){
        TimerNode* retNode = heap[1];
        swap(&heap[1], &heap[count]);
        count --; // 维护count数量
        /* 将堆最后面的节点移动上来后再将其shiftDown移动到该到的地方去 */
        shiftDown(1);
        return retNode;
    }
    return nullptr;

}


void TimerHeap::shiftDown(int index) {

    int child = index * 2;
    while (child <= count){
        /* 从孩子中找出最小的时间值，跟最小的时间值更换 */
        if (child + 1 <= count )
            if (heap[child + 1] < heap[child])
                child ++;
        /* 如果index的时间比较大，就往下移动 */
        if (heap[index] > heap[child])
            swap(&heap[index], &heap[child]);

        /* 继续递归往下寻找 */
        index = child;
        child = index * 2;
    }
}

