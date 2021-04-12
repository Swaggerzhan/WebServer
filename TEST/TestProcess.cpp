//
// Created by swagger on 2021/4/12.
//

#include "TestProcess.h"




int TestProcess::m_idx;
Process** TestProcess::m_list;
TestProcess* TestProcess::instance;
int TestProcess::epfd;
epoll_event *TestProcess::epollArray;
bool TestProcess::m_stop = false;
int TestProcess::m_max_process;


TestProcess* TestProcess::getPool(int pool_max) {
    if (instance)
        return instance;
    instance = new TestProcess(pool_max);
}



TestProcess::TestProcess(int pool_max) {
    m_max_process = pool_max;
    m_list = new Process*[m_max_process];
    for (int i=0; i<pool_max; i++){
        m_list[i] = new Process;
        int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, m_list[i]->pipe);
        if (ret < 0){
            printf("pipe error! ret code %d\n", ret);
            printf("%s\n", strerror(errno));
            exit(1);
        }
        m_list[i]->m_pid = fork();
        if (m_list[i]->m_pid > 0){
            /* 父进程 */
            close(m_list[i]->pipe[0]); // 关闭0，不需要读取
            m_idx = -1;
            continue;
        }else {
            /* 子进程 */
            close(m_list[i]->pipe[1]); // 关闭1，不需要写
            m_idx = i;
            break;
        }
    }



}


void TestProcess::RUN() {
    if (m_idx == -1){
        run_father();
        return;
    }
    run_child();
}


void TestProcess::run_father() {

    printf("father process starting...\n");
    char buf[10] = "hello\n";
    for (int i=0; i<10; i++){
        printf("send to idx %d pid is %d\n", m_idx, m_list[0]->m_pid);
        write(m_list[0]->pipe[1], buf, strlen(buf));
        sleep(2);
        printf("debug\n");
    }


}


void TestProcess::run_child() {

    printf("child process starting...\n");
    epfd = epoll_create(5);
    int pipefd = m_list[m_idx]->pipe[0];
    if (epfd < 0){
        printf("epoll_create() error!\n");
        exit(1);
    }
    epollArray = new epoll_event[10];
    addFd(epfd, pipefd);// 添加到epoll中
    while ( !m_stop ){
        int ret = epoll_wait(epfd, epollArray, 10, 0);
        if (ret < 0){
            printf("epoll_wait() error!\n");
            exit(1);
        }
        for (int i=0; i<ret; i++){
            if (epollArray[i].data.fd == pipefd){
                /* 收到父进程收到的信息 */
                char buf[1024];
                int len = recv(pipefd, buf, 1024, 0);
                if (len != 6)
                    continue;
                printf("got father data %s\n", buf);
            }
        }
    }

}


void addFd(int epfd, int fd){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}




