//
// Created by swagger on 2021/5/10.
//

#include "TimerHandler.h"


static int pipe2io_thread[2];
static int pipe2timer_thread[2];
static pthread_t timer_thread;
static fd_set read_set;
static fd_set write_set;
static TimerHeap heap;


void* TimerHandler::tick(void *arg) {

    struct timeval select_time{}; /* 需要等待的最短时间 */
    struct timeval cur_time{}; /* 当前时间 */
    int ret;
    int sig;

    while ( true ){

        TimerNode* time_node = heap.top();
        gettimeofday(&cur_time, nullptr);
        /* 计算当前时间差 */
        if (!time_node){ // 没有节点就默认一只等待下去
            ret = select(1, &read_set, nullptr, nullptr, nullptr);
        }else{
            select_time.tv_sec = time_node->expire.tv_sec - cur_time.tv_sec;
            select_time.tv_usec = time_node->expire.tv_usec - cur_time.tv_usec;
            ret = select(1, &read_set, nullptr, nullptr, &select_time);
        }
        if (ret == 0){
            /* 时间到，通知主线程！ */
            ::write(pipe2io_thread[1], (char*)&TIME_OUT, sizeof(int));
        }else if (ret == -1){
            exit_error("select error!\n", true, 1);
        }else{ // 定时更短的需求到了
            ::read(pipe2timer_thread[0], (char*)&sig, sizeof(int));
            if (sig == RESET_TIMER)
                continue;
        }
        /* 此处一只堵塞等待，直到IO线程处理完所有时间节点在叫醒时间线程！*/
        ::read(pipe2timer_thread[0], (char*)&sig, sizeof(int));
        if (sig == RESET_TIMER){
            continue;
        }else{
            break;
        }
    }
    exit_error("IO thread send Wrong thing!\n", true, 1);

    return nullptr;
}