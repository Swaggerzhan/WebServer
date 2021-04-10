//
// Created by swagger on 2021/4/9.
//

#include "TimerLink.h"





void TimerTest(){

    cout << "mainValue: " << mainValue << endl;
    timerLink = new TimerLink();
    /* 注册信号函数 */
    addsig(SIGALRM);
    //timerLink = new TimerLink();

    srand(time(nullptr));
    TimerNode* timerNode;
    cout << 1 << endl;
    for (int i=0; i<10; i++){
        timerNode = new TimerNode;
        timerNode->call_back = call_back;
        time_t cur = time(nullptr);
        timerNode->expire = cur + (TIMESLOT * rand() % 10);
        timerNode->data = new WorkData(i);
        /* 添加节点 */
        timerLink->addTimer(timerNode);
    }
    timerLink->checkLink();
    alarm(TIMESLOT);
    std::cout << "TimerTest end..." << std::endl;
    while(1)
        poll(nullptr, 0, 5);
}



void addsig(int sig){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    /* 开启信号传参 */
    sa.sa_flags |= SA_SIGINFO;
    sigfillset( &sa.sa_mask );
    assert( sigaction(sig, &sa, nullptr) != -1 );
}



void sig_handler(int sig){
    //void* test = (void*)((*info).si_ptr);
    timerLink->tick();
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
    TimerNode* preNode = nullptr;
    TimerNode* tmpNode = head;
    while (tmpNode != nullptr && target->expire > tmpNode->expire){
        preNode = tmpNode;
        tmpNode = tmpNode->next;
    }

    /* 需要加入头部 */
    if (tmpNode == head){
        head = target;
        target->next = tmpNode;
        tmpNode->pre = target;
    }
    /* 需要将其加入到尾部 */
    else if (tmpNode == nullptr){
        preNode->next = target;
        target->pre = preNode;
        tail = target;
    }else {/* 加入到preNode和tmpNode之间 */
        target->next = tmpNode;
        tmpNode->pre = target;
        preNode->next = target;
        target->pre = preNode;
    }

//    while ( tmpNode != nullptr ){
//        if ( target->expire < tmpNode->expire){
//            preNode->next = target;
//            target->next = tmpNode;
//            tmpNode->pre = target;
//            target->pre = preNode;
//            break;
//        }
//        preNode = tmpNode;
//        tmpNode = tmpNode->next;
//    }
//
//    if ( tmpNode == nullptr){
//        preNode->next = target;
//        target->pre = preNode;
//        target->next = nullptr;
//        tail = target;
//    }


    return true;
}


void TimerLink::tick() {
    printf("tick..\n");
    time_t curTime = time(nullptr);
    /* 尝试去获取头节点 */
    TimerNode* tmpNode = head;
    while (tmpNode != nullptr){
        /* 如果时间还未到，直接返回 */
        if (tmpNode->expire > curTime ) {
            return;
        }
        /* 超时，调回调函数处理 */
        tmpNode->call_back(tmpNode->data);
//        /* sb操作，麻了 */
//        delTimer(tmpNode); // delete node
//        tmpNode = tmpNode->next;
        head = tmpNode->next;
        delete tmpNode;
        tmpNode = head;
        checkLink();
    }
}


void TimerLink::delTimer(TimerNode *target) {
    checkLink();
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


void TimerLink::checkLink() {
    TimerNode* tmpNode = head;
    int count = 0;
    while(tmpNode != nullptr){
        tmpNode = tmpNode->next;
        count++;
    }
    cout << "total Node: " << count << endl;
}

TimerLink *timerLink;
int mainValue = 95;