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
sem_t ThreadPool::sem_lock;



ThreadPool * ThreadPool::getPool(int epoll_fd, int thread_number) {
    if (instance)
        return instance;
    instance = new ThreadPool(epoll_fd, thread_number);
    return instance;
}


ThreadPool::ThreadPool(int epoll_fd, int thread_number) {
    m_thread_num = thread_number;
    lock = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&sem_lock, 0, 0);
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

/* 互斥锁版本 */
//bool ThreadPool::append(Request *request) {
//    if (pthread_mutex_trylock(&lock) != 0 )
//        return false;
//    requestLine.push(request);
//    pthread_mutex_unlock(&lock);
//    return true;
//}

bool ThreadPool::append(Request *request) {
    if ( pthread_mutex_lock(&lock) != 0){
        printf("pthread_mutex_lock() error!\n");
        exit(1);
    }
    requestLine.push(request);
    pthread_mutex_unlock(&lock);
    sem_post(&sem_lock);
    return true;
}

/* 互斥锁版本 */
//void ThreadPool::run() {
//    while( !t_stop ){
//        /* 尝试上锁 */
//        if (pthread_mutex_trylock(&lock) != 0)
//            continue;
//        Request* w = requestLine.front();
//        /*  如果其中是空的，则直接释放锁，不做任何事情 */
//        if (w == nullptr){
//            pthread_mutex_unlock(&lock);
//            continue;
//        }
//        /* 队列不空，弹出队头，处理 */
//        requestLine.pop();
//        pthread_mutex_unlock(&lock);
//        int code = w->process();
//        if (code == 0){
//            removeFd(epfd, w->fd);
//            delete w;
//        }
//    }
//}
void ThreadPool::run(){
    while ( !t_stop ){
        /* -1操作 */
        if (sem_wait(&sem_lock) != 0){
            printf("sem_wait() error!\n");
            exit(1);
        }
        printf("stack was doing nothing!\n");
        /* 上锁操作 */
        pthread_mutex_lock(&lock);
        Request* w = requestLine.front();
        requestLine.pop();
        pthread_mutex_unlock(&lock);
        /* 解锁 */
        int code = w->process();
//        if ( code == 0 ){
//            removeFd(epfd, w->fd);
//            delete w;
//        }
        removeFd(epfd, w->fd);
        delete w;

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
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
    setNonBlock(sock);
}


static void removeFd(int epfd, int sock){
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, 0);
    close(sock);
}
