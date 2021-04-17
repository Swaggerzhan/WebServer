//
// Created by swagger on 2021/4/12.
//

#include "ThreadPool.h"



/* 静态变量定义 */

int ThreadPool::m_thread_num;
ThreadZ** ThreadPool::m_list;
pthread_mutex_t ThreadPool::lock;
ThreadPool* ThreadPool::instance;
std::queue<Request*> ThreadPool::requestLine;
int ThreadPool::epfd;
pthread_cond_t ThreadPool::line_ready;



ThreadPool * ThreadPool::getPool(int epoll_fd, int thread_number) {
    if (instance)
        return instance;
    instance = new ThreadPool(epoll_fd, thread_number);
    return instance;
}


ThreadPool::ThreadPool(int epoll_fd, int thread_number) {
    m_thread_num = thread_number;
    lock = PTHREAD_MUTEX_INITIALIZER;
    line_ready = PTHREAD_COND_INITIALIZER;
    epfd = epoll_fd;
    /* 声明指针数组所需要的空间 */
    m_list = new ThreadZ*[m_thread_num];
    /* 开始创建线程，并且加入线程池 */
    for (int i=0; i<m_thread_num; ++i){
        m_list[i] = new ThreadZ(i);
        /* instance可能为空？线程池还未初始化! */
        /* 传入this可以解决这个问题，this总是指向事例的首地址！ */
        int ret = pthread_create(&m_list[i]->pthread_id, nullptr, ThreadZ::work, (void*)this);
        if (ret < 0)
            exit_error("pthread_create()", true, 1);
        /* 分离线程 */
        pthread_detach(m_list[i]->pthread_id);
    }

}


bool ThreadPool::append(Request *request) {
    if ( pthread_mutex_lock(&lock) != 0)
        exit_error("pthread_mutex_lock()", true, 1);
    requestLine.push(request);
    pthread_mutex_unlock(&lock);
    /* 叫醒睡眠的线程 */
    pthread_cond_signal(&line_ready);
    return true;
}

void ThreadPool::run(){
    while ( !t_stop ){
        /* -1操作 */
        if ( pthread_mutex_lock(&lock) != 0)
            exit_error("pthread_mutex_lock()", true, 1);
        /* 里面的东西为空则随眠等待队列就绪 */
        if ( requestLine.front() == nullptr )
            pthread_cond_wait(&line_ready, &lock);
        Request* w = requestLine.front();
        requestLine.pop();
        pthread_mutex_unlock(&lock);
        w->process();

    }
}


void* ThreadZ::work(void *arg) {
    auto* pool = (ThreadPool*)arg;
    pool->run();
    return pool;
}


ThreadZ::ThreadZ(int idx) {
    this->t_idx = idx;
}
