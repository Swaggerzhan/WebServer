//
// Created by swagger on 2021/4/9.
//

#include "TimerLink.h"





void TimerTest(){

    /* 注册信号函数 */
    addsig(SIGALRM);

//    srand(time(nullptr));
//    TimerNode* timerNode;
//    cout << 1 << endl;
//    for (int i=0; i<10; i++){
//        timerNode = new TimerNode;
//        cout << "new TimerNode" << endl;
//        timerNode->call_back = call_back;
//        time_t cur = time(nullptr);
//        timerNode->expire = cur + (TIMESLOT * rand() % 10);
//        timerNode->data = new WorkData(i);
//        /* 添加节点 */
//        timerLink.addTimer(timerNode);
//    }
//    cout << "end loop" << endl;
//    alarm(TIMESLOT);
}



void addsig(int sig){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset( &sa.sa_mask );
    assert( sigaction(sig, &sa, nullptr) != -1 );
}



void sig_handler(int sig){
    cout << "testNumber: " << testNumber << endl;
    cout << "sa_handler...." << endl;
    cout << &timerLink << endl;
    cout << "timer head endl" << endl;
    timerLink.tick();
    alarm(TIMESLOT);
}




void call_back(WorkData* workData){
    //cout << "workId: "<< workData->workId << "call_back called..." << endl;
    cout << "call_back end..." << endl;
}



TimerLink::TimerLink() {
    head = nullptr;
    tail = nullptr;
}

TimerNode::TimerNode() {
    data = nullptr;
    pre = next = nullptr;
}


bool TimerLink::addTimer(TimerNode* target) {
    if (head  == nullptr) {
        head = tail = target;
        return true;
    }
    TimerNode* preNode;
    TimerNode* tmpNode = head;
//    while (tmpNode != nullptr && target->expire > tmpNode->expire){
//        preNode = tmpNode;
//        tmpNode = tmpNode->next;
//    }
//
//    /* 需要将其加入到尾部 */
//    if (tmpNode == nullptr){
//        preNode->next = target;
//        target->pre = preNode;
//        tail = target;
//    }else {/* 加入到preNode和tmpNode之间 */
//        target->next = tmpNode;
//        tmpNode->pre = target;
//        preNode->next = target;
//        target->pre = preNode;
//    }

    while ( tmpNode != nullptr ){
        if ( target->expire < tmpNode->expire){
            preNode->next = target;
            target->next = tmpNode;
            tmpNode->pre = target;
            target->pre = preNode;
            break;
        }
        preNode = tmpNode;
        tmpNode = tmpNode->next;
    }

    if ( tmpNode == nullptr){
        preNode->next = target;
        target->pre = preNode;
        target->next = nullptr;
        tail = target;
    }


    return true;
}


void TimerLink::tick() {
    cout << "tick函数调用不可访问head" << endl;
//    if (!head)
//        return;
    cout << "tick!!!!!!!!" << endl;
    time_t curTime = time(nullptr);
    cout << "curTime" << endl;
    TimerNode* tmpNode = head;
    cout << tmpNode << endl;
    cout << "got head" << endl;
    while (tmpNode != nullptr){
        /* 如果时间还未到，直接返回 */
        cout << "while" << endl;
        if (tmpNode->expire > curTime )
            return;
        /* 超时，调回调函数处理 */
        cout << "回调函数调用。。。。" << endl;
        tmpNode->call_back(tmpNode->data);
        cout << "going call delTimer..." << endl;
        delTimer(tmpNode); // delete node
        cout << "delete node" << endl;
        tmpNode = tmpNode->next;
    }
}


void TimerLink::delTimer(TimerNode *target) {
    /* 头节点的话直接清空链表 */
    if (target == head){
        delete target;
        head = tail = nullptr;
        return;
    }

    TimerNode* preNode = target->pre;
    /* target是尾节点情况 */
    if (target->next == nullptr ){
        preNode->next = nullptr;
        tail = preNode;
        delete target;
        return;
    }

    preNode->next = target->next;
    target->next->pre = preNode;
    delete target;

}
