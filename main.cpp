#include <iostream>
#include <sys/poll.h>

#include "Timer/TimerHeap.h"




int main() {

    /*函数注册*/
    addSig(SIGALRM);

    int n = 20;
    timer = new TimerHeap;
    srand(time(nullptr));
    TimerNode* tmpNode;
    for (int i=0; i<n; i++){
        time_t expire_time = time(nullptr)+(TIMESLOT*(rand()%20));
        tmpNode = new TimerNode(expire_time, i);
        tmpNode->call_bak = call_back;
        /* 时间节点加入 */
        timer->insert(tmpNode);
    }
    printf("节点添加完成");

    alarm(TIMESLOT);

    while(1)
        poll(nullptr, 0, 5);



}
