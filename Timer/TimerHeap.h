//
// Created by swagger on 2021/5/10.
//

#ifndef TIMER_TIMERHEAP_H
#define TIMER_TIMERHEAP_H

#include "TimerNode.h"
#include <iostream>
#include "../utility.h"
#include "../HTTP/Request.h"


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
    inline int size(){return count;}

    /**
     * 时间节点的添加
     * @return
     */
    bool insert(Request *request);


    /**
     * 延迟删除 
     * 
     */
    inline void delNode(Request* request){
        if (!request)
            return;
        request->node->call_bak = nullptr;
    }


    /**
     * 将堆最顶端的节点推出
     * 推出的节点可能是延迟删除的产物，也就是call_bak = nullptr
     * @return
     *
     */
    TimerNode* pop();


    /**
     *  返回一个堆定元素，这个元素是可用的
     *  top函数可能将堆顶的一些延迟删除节点彻底删除掉
     * @return TimerNode*节点表示正常，nullptr表示堆空
     */
    inline TimerNode* top(){
        while (!isEmpty()){
            if (!heap[1]->call_bak){
                TimerNode *node = pop();
                delete node;
                continue;
            }
            return heap[1];
        }
        return nullptr;
    }


    inline bool isEmpty(){ return count == 0; }





    

private:

    /**
     * 交换值
     * @param left
     * @param right
     */
    static void swap(TimerNode** left, TimerNode** right);


    /**
     * 将对应的index下节点向上移动到该到的地方
     */
    void shiftUp(int index);


    /**
     * 将对应的index的节点向下移动到该到的地方
     * @param index
     */
    void shiftDown(int index);


    

};



#endif //TIMER_TIMERHEAP_H
