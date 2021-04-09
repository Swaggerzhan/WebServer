#include <iostream>
#include <sys/poll.h>

#include "TEST/TimerLink.h"



int main() {

    // threadPoolMain();
    TimerTest();
    std::cout << "TimerTest end..." << std::endl;
    while(1)
        poll(nullptr, 0, 5);

}
