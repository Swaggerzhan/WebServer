//
// Created by swagger on 2021/5/10.
//

#include "TimerHandler.h"


int TimerHandler::pipe2io_thread[2];
int TimerHandler::pipe2timer_thread[2];
pthread_t TimerHandler::timer_thread;
fd_set TimerHandler::read_set;
fd_set TimerHandler::write_set;
TimerHeap TimerHandler::heap;
timeval TimerHandler::sleep_time;


bool TimerHandler::insert(Request *request) {
    std::cout << "TimerHandler::insert()" << std::endl;
    std::cout << "insert node fd: " << request->fd << std::endl;
    if (heap.isEmpty()) {
        heap.insert(request);
        set_timer_with_sleep_time(ADD_TIMER); // 添加新节点
        return true;
    }
    heap.insert(request);
    TimerNode* heap_top_node = heap.top();
    /* 对于新来的节点比堆顶超时时间还早的情况就重新设置 */
    if (request->node->expire.tv_sec == heap_top_node->expire.tv_sec) {
        if (request->node->expire.tv_usec < heap_top_node->expire.tv_usec) {
            std::cout << "send RESET_TIMER sig to timer pthread" << std::endl;
            set_timer_with_sleep_time(RESET_TIMER); // 重置节点
            return true;
        }
    }
    if (request->node->expire.tv_sec < heap_top_node->expire.tv_sec){
        std::cout << "send RESET_TIMER sig to timer pthread" << std::endl;
        set_timer_with_sleep_time(RESET_TIMER);
        return true;
    }
    return true;
}


bool TimerHandler::update(Request *request) {
    std::cout << "updating node fd: " << request->fd << std::endl;
    request->node->call_bak = nullptr;
    TimerNode* heap_top_node = heap.top();
    return insert(request);
}


bool TimerHandler::handler_time_out(){
    TimerNode* node;
    timeval *diff_time = new timeval;
    while ((node = heap.top()) != nullptr){
        if ( !cal_time_diff(node, diff_time) ){ // 超时节点
            heap.pop(); // 弹出超时节点
            node->call_bak(node->client_data); // 处理超时节点
            delete node; // 删除已经处理完的节点
            continue;
        }
        break;
    }
    /* 时间堆空情况 */
    if (!node){
        /* 时间堆是空的，那就永久睡眠，直到io线程来临新任务 */
        ::write(pipe2timer_thread[1], (char*)&SLEEP_FOREVER, sizeof(int));
        std::cout << "handler_timer_out()->sleep_forever()" << std::endl;
        return false;
    }
    /* 接下来对于还没超时的节点，通过获取到的时间差，重新设置时间，等待时间到达
     通知时间线程该睡眠多长时间，注：此时节点还存在与堆顶 */
    sleep_time = *diff_time; // 更新
    std::cout << "sleep_time->tv_sec: " << sleep_time.tv_sec << std::endl;
    std::cout << "sleep_time->tv_usec: " << sleep_time.tv_usec << std::endl;
    delete diff_time;
    set_timer_with_sleep_time(RESET_TIMER);
    return true;
}


/**
 * @param node为传入节点
 * @param diff_time表示时间差
 * @return 返回true表示获取时间差成功，false表示时间节点已经超时
 */
bool TimerHandler::cal_time_diff(TimerNode* node, timeval* diff_time){
    timeval cur_time{};
    gettimeofday(&cur_time, nullptr);
    if (cur_time.tv_sec == node->expire.tv_sec){
        if (cur_time.tv_usec >= node->expire.tv_usec) // 节点超时！
            return false;
    
    }
    if (cur_time.tv_sec > node->expire.tv_sec) // 节点超时！
        return false;
    /* 节点还没超时的情况，那就返回true，并将时间差写入diff_time */
    long sec = node->expire.tv_sec - cur_time.tv_sec;
    long usec = node->expire.tv_usec - cur_time.tv_usec;
    sec *= 1000000;
    usec += sec;
    diff_time->tv_sec = usec / 1000000;
    diff_time->tv_usec = usec % 1000000;
    return true;
        
}


void* TimerHandler::timer_thread_entry(void *arg) {

    int ret;
    int sig;
    int loop = 0;
    ret = select(pipe2timer_thread[0]+1, &read_set, nullptr, nullptr, nullptr);
    if (ret == -1)
        exit_error("timer_thread_entry()->select()", true, -1);
    if (::read(pipe2timer_thread[0], (char*)&sig, sizeof(int)) == -1)
        exit_error("timer_thread_entry()->read()", true, -1);
    while ( true ){
        printf("loop: %d\n", loop);
        fd_ZERO(&read_set);
        fd_SET(pipe2timer_thread[0], &read_set);
        switch( sig ){
            case SLEEP_FOREVER:{
                sig = sleep_forever();
                break;
            }
            case RESET_TIMER:{
                sig = sleep_some_time();
                break;
            }
            case ADD_TIMER:{ // 提前叫醒的情况
                sig = sleep_some_time();
                break;
            }
            default: exit_error("timer_thread_entry()->switch", true, -1);
        }
        loop ++;
    }

    return nullptr;
        
}

/**
 * 永久睡眠，等待IO线程叫醒，并返回IO线程信号
 * @return
 */
int TimerHandler::sleep_forever(){
    int ret;
    int sig;
    std::cout << "going sleep forever" << std::endl;
    ret = select(pipe2timer_thread[0]+1, &read_set, nullptr, nullptr, nullptr);
    if (ret == -1)
        exit_error("sleep_forever()->select()", true, -1);
    ret = ::read(pipe2timer_thread[0], (char*)&sig, sizeof(int));
    if (ret == -1 )
        exit_error("sleep_forever()->::read()", true, -1);
    return sig;
}

/**
 * 随眠一段时间，等待IO线程叫醒或者超时醒来
 * 超时醒来会向IO线程发送超时信号，本身再次进入永久睡眠
 * IO线程提前叫醒则根据叫醒信号处理
 * @return
 */
int TimerHandler::sleep_some_time(){
    int ret;
    int sig;
    /* 睡眠固定时间 */
    std::cout << "going sleep..." << std::endl;
    std::cout << "sleep_time->tv_sec: " << sleep_time.tv_sec << std::endl;
    std::cout << "sleep_time->tv_usec: " << sleep_time.tv_usec << std::endl;
    ret = select(pipe2timer_thread[0]+1, &read_set, nullptr, nullptr, &sleep_time);
    std::cout << "wake" << std::endl;
    if (ret == -1 )
        exit_error("sleep_some_time()->select()", true, -1);
    if (ret == 0){ // 随眠时间到，叫醒io线程处理！
        if (::write(pipe2io_thread[1], (char*)&TIME_OUT, sizeof(int)) == -1)
            exit_error("sleep_some_time()->write()", true, -1);
        sig = SLEEP_FOREVER;
        return sig;
    }
    /* 提前叫醒情况，可能是有新节点添加进来，那就重新计算随眠时间 */
    if (::read(pipe2timer_thread[0], (char*)&sig, sizeof(int)) == -1)
        exit_error("sleep_some_time()->read()", true, -1);
    /*  */
    return sig;
}




