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



ThreadPool * ThreadPool::getPool(int epoll_fd, int thread_number) {
    if (instance)
        return instance;
    instance = new ThreadPool(epoll_fd, thread_number);
    return instance;
}


ThreadPool::ThreadPool(int epoll_fd, int thread_number) {
    m_thread_num = thread_number;
    lock = PTHREAD_MUTEX_INITIALIZER;
    epfd = epoll_fd;
    /* 声明指针数组所需要的空间 */
    m_list = new ThreadZ*[m_thread_num];
    /* 开始创建线程，并且加入线程池 */
    for (int i=0; i<m_thread_num; ++i){
        m_list[i] = new ThreadZ(i);
        /* instance可能为空？线程池还未初始化! */
        /* 传入this可以解决这个问题，this总是指向事例的首地址！ */
        int ret = pthread_create(&m_list[i]->pthread_id, nullptr, ThreadZ::work, (void*)this);
        if (ret < 0){
            printf("pthread_create() error!\n");
            printf("%s\n", strerror(errno));
            exit(1);
        }
        /* 分离线程 */
        pthread_detach(m_list[i]->pthread_id);
    }

}


bool ThreadPool::append(Request *request) {
    if (pthread_mutex_trylock(&lock) != 0 )
        return false;
    requestLine.push(request);
    pthread_mutex_unlock(&lock);
    return true;
}


void ThreadPool::run() {
    while( !t_stop ){
        /* 尝试上锁 */
        if (pthread_mutex_trylock(&lock) != 0)
            continue;
        Request* w = requestLine.front();
        /*  如果其中是空的，则直接释放锁，不做任何事情 */
        if (w == nullptr){
            pthread_mutex_unlock(&lock);
            continue;
        }
        /* 队列不空，弹出队头，处理 */
        requestLine.pop();
        pthread_mutex_unlock(&lock);
        int code = w->process();
        if (code == 0){
            removeFd(epfd, w->fd);
            delete w;
        }
    }
}



void* ThreadZ::work(void *arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    pool->run();
    return pool;
}


ThreadZ::ThreadZ(int idx) {
    this->t_idx = idx;
}

static int setNonBlock(int fd){
    int old_option = fcntl(fd, F_GETFD);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFD, new_option);
    return old_option;
}


void addFd(int epfd, int sock){
    epoll_event event{};
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
    setNonBlock(sock);
}


static void removeFd(int epfd, int sock){
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, 0);
    close(sock);
}
