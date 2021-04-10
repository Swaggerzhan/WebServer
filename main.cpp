#include <iostream>
#include <sys/poll.h>

#include "TEST/TimerLink.h"




int main() {

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
