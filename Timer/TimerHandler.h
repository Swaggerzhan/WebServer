//
// Created by swagger on 2021/5/10.
//

#ifndef WEBSERVER_TIMERHANDLER_H
#define WEBSERVER_TIMERHANDLER_H

#include "TimerHeapSelect.h"
#include <sys/select.h>




class TimerHandler {

public:
    static int pipe2io_thread[2];

    static int pipe2timer_thread[2];

    static TimerHeap heap;

private:

    static pthread_t timer_thread;

    static fd_set read_set;

    static fd_set write_set;




public:


    TimerHandler(){

        FD_SET(pipe2io_thread[1], &write_set);
        FD_SET(pipe2timer_thread[0], &read_set);
        pthread_create(&timer_thread, nullptr, TimerHandler::tick, nullptr);
        pthread_detach(timer_thread);
    }

    static void* tick(void* arg);


};


#endif //WEBSERVER_TIMERHANDLER_H
