//
// Created by swagger on 2021/5/10.
//

#ifndef TIMER_TIMERHANDLER_H
#define TIMER_TIMERHANDLER_H

#include <sys/select.h>
#include <pthread.h>
#include "TimerHeap.h"
#include <unistd.h>


class TimerHandler {

public:
    /* 时间线程到io线程管道 */
    static int pipe2io_thread[2];
    /* io线程到时间线程管道 */
    static int pipe2timer_thread[2];

    static TimerHeap heap;

private:

    /* 时间线程 */
    static pthread_t timer_thread;
    /* 需要睡眠的时间 */
    static timeval sleep_time; // 虽然是临界区，但是可以通过代码逻辑让其不用加锁
    /* select监听的可读管道符 */
    static fd_set read_set;

    static fd_set write_set;


public:


    TimerHandler(){
        pipe(pipe2timer_thread);
        pipe(pipe2io_thread);
        fd_SET(pipe2timer_thread[0], &read_set);
        pthread_create(&timer_thread, nullptr, TimerHandler::timer_thread_entry, nullptr);
        pthread_detach(timer_thread);
    }

    
    /**
     *  @param request 需要挂载节点的客户端
     *  如果当前堆空，则直接发送信号
     *  如果当前堆顶时间比新挂载的节点更晚，那就重设节点
     *  @return true 为挂载成功
     */
    static bool insert(Request* request);

    /**
     * 处理超时节点
     * @return
     */
    static bool handler_time_out();

    /**
     * 更新节点，将某些存在的节点删除是采用延迟删除
     * @param request
     * @return
     */
    static bool update(Request* request);


private:


    /**
     * 时间线程入口 
     */ 
    static void* timer_thread_entry(void* arg);

    /**
     * @param node为传入节点
     * @param diff_time表示时间差
     * @return 返回true表示获取时间差成功，false表示时间节点已经超时
     */
    static bool cal_time_diff(TimerNode* node, timeval* diff_time);


    static int sleep_forever();


    static int sleep_some_time();


    /**
     * 一定是有节点才会去叫时间醒来
     * @param sig
     */
    static inline void set_timer_with_sleep_time(int sig){
        TimerNode* node = heap.top();
        cal_time_diff(node, &sleep_time);
        std::cout << "trying to call timer_thread wake..." << std::endl;
        if (::write(pipe2timer_thread[1], (char *)&sig, sizeof(int)) == -1)
            exit_error("set_timer_with_sleep_time()->write()", true, -1);
    }



};



#endif //TIMER_TIMERHANDLER_H
