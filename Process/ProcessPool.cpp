//
// Created by swagger on 2021/4/11.
//

#include "ProcessPool.h"






ProcessPool* ProcessPool::getPool(int max_process, int demo) {
    if ( instance )
        return instance;
    return (instance = new ProcessPool(max_process, demo));
}


ProcessPool::ProcessPool(int max_process, int demo_socket) {
    m_process_max = max_process;
    m_process_list = new Process*[m_process_max];
    demo = demo_socket;

    for (int i=0; i<m_process_max; i++){
        m_process_list[i] = new Process();
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, m_process_list[i]->pipe) == -1) {
            printf("socketpair error!\n");
            exit(1);
        }
        /* 进行fork操作 */
        m_process_list[i]->c_proc_id = fork();
        if (m_process_list[i]->c_proc_id == 0){
            /* 子进程中 */
            close(m_process_list[i]->pipe[0]);
            m_idx = i;
            break; // 子进程中不需要循环，直接结束即可
        }else {
            /* 父进程中 */
            close(m_process_list[i]->pipe[1]);
            continue; // 继续创建进程
        }
    }

}


void ProcessPool::RUN(int idx) {
    if (m_idx == -1){
        /* 父进程中 */
        run_father();
        return;
    }
    run_child();
}


void ProcessPool::run_father() {

    //注册信号和开启epoll
    setup_sig_pipe();

    /* 父进程貌似可以什么都不做，直接通知子进程去accept即可 */
    epollArray = new epoll_event[10];
    epoll_event event;
    event.data.fd = demo;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, demo, &event);
    int new_conn = 1; /* 随机传送一个数据让子进程知道有新客户链接 */
    while ( !m_stop ){
        int code = epoll_wait(epfd, epollArray, 10, 0);
        if (code < 0) {
            printf("epoll_wait failure\n");
            break;
        }
        /* 循环处理每个请求 */
        for (int i=0; i<code; i++){
            int handler_fd = epollArray[i].data.fd;
            /* 收到新链接请求，将其告诉子进程 */
            if (handler_fd == demo){
                /* 这里先采用随机算法，随机分配任务给子进程，后续是需要进行负载均衡处理 */
                int idx = 1 + rand() % (m_process_max - 1);
                /* 随便往子进程的管道中写点东西即可 */
                send(m_process_list[idx]->pipe[0], (char*)&new_conn, sizeof(int), 0);
                printf("request send to child: %d\n", m_process_list[idx]->c_proc_id);
                continue;
            }
            /* 处理信号 */
            if ( (handler_fd == sig_pipe[0]) && (epollArray[i].events & EPOLLIN )){
                int sig;
                char signal[1024];
                int ret = recv(handler_fd, signal, sizeof(signal), 0);
                if ( ret <= 0 )
                    continue;
                for (int idx=0; idx<ret; idx++){
                    /* 处理各个信号 */
                    switch ( signal[idx] ){
                        /* 子进程死亡 */
                        case SIGCHLD: {
                            pid_t pid;
                            int stat;
                            while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0){
                                /* 找出其中已经退出的子进程，将其c_proc_id设为-1表示已经退出 */
                                for (int j=0; j<m_process_max; j++){
                                    if (m_process_list[j]->c_proc_id == pid ){
                                        close(m_process_list[i]->pipe[0]);
                                        m_process_list[i]->c_proc_id = -1;
                                    }
                                }
                            }
                            /* 如果所有的子进程都退出了，那父进程也退出 */
                            m_stop = true;
                            for (int j=0; j<m_process_max; j++){
                                if (m_process_list[i] != -1)
                                    m_stop = false;
                            }
                            break;
                        }
                        case SIGTERM:
                        /* 父进程收到杀死信号，将子进程都杀死 */
                        case SIGINT:{
                            printf("kill all child process\n");
                            for (int j=0; j<m_process_max; ++j){
                                if (m_process_list[j]->c_proc_id != -1)
                                    kill(m_process_list[i]->c_proc_id, SIGTERM);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                continue;
            }
        }
    }
    // close(demo); /* 创建着来关闭这个套接字 */
    close(epfd);

}


void ProcessPool::run_child() {

    //注册信号和开启epoll
    setup_sig_pipe();

    /* 子进程从这个fd中收到来自父进程的数据 */
    int pipefd = m_process_list[m_idx]->pipe[1];
    epollArray = new epoll_event[maxOpen];
    /* 将父进程fd添加到epoll中 */
    addFd(epfd, pipefd);

    while ( !m_stop ){
        int code = epoll_wait(epfd, epollArray, maxOpen, 0);
        if ( (code < 0) && (errno == EINTR) ){
            printf("epoll_wait() error\n");
            break;
        }
        /* 处理所有请求 */
        for (int i=0; i<code; ++i){
            int handler_fd = epollArray[i].data.fd;
            /* 父进程发来请求，表示有新链接的到来 */
            if ( (handler_fd == pipefd) && (epollArray[i].events & EPOLLIN )){
                int new_conn;
                int ret = recv(handler_fd, (char*)&new_conn, sizeof(int), 0);
                if ( ret < 0 ){
                    printf("father process data error!\n");
                    continue;
                }
                struct sockaddr_in client_addr;
                socklen_t client_addr_sz;
                int clientFd = accept(demo, (sockaddr*)&client_addr, &client_addr_sz);
                if ( clientFd < 0 ){
                    printf("client socket error!\n");
                    strerror(errno);
                    continue;
                }
                /* 添加到epoll循环中去 */
                addFd(epfd, clientFd);
                continue;
            }
            /* 收到信号 */
            if ( (handler_fd == sig_pipe[0]) && ( epollArray[i].events & EPOLLIN ) ){
                int sig;
                char signal[1024];
                int ret = recv(handler_fd, signal, sizeof(signal), 0);
                if ( ret < 0 )
                    continue;
                for (int idx=0; idx<ret; ++idx){
                    switch( signal[idx] ){
                        case SIGCHLD:

                        case SIGTERM:

                        /* 杀死子进程的命令 */
                        case SIGINT: {
                            m_stop = true;
                            break;
                        }
                        default:
                            break;
                    }
                }
                continue;
            }
            /* 剩下的就是客户端发来的请求了 */
            char *buf = "hello there\n";
            send(handler_fd, buf, strlen(buf), 0);
        }

    }
    close(epfd);
    close(pipefd);
}


void ProcessPool::setup_sig_pipe() {
    epfd = epoll_create(5);
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sig_pipe);
    if ( ret < 0 ){
        printf("setup_sig_pipe() error!\n");
        exit(1);
    }
    setNonBlock(sig_pipe[1]);
    addFd(epfd, sig_pipe[0]);
    /* 添加信号 */
    addSignal(SIGCHLD, signal_handler);
    addSignal(SIGTERM, signal_handler);
    addSignal(SIGINT, signal_handler);
    addSignal(SIGPIPE, SIG_IGN);
}


static int setNonBlock(int fd){
    int old_option = fcntl(fd, F_GETFD);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFD, new_option);
    return old_option;
}


static void addFd(int epfd, int sock){
    epoll_event event;
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
    setNonBlock(sock);
}


static void removeFd(int epfd, int sock){
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, 0);
    close(sock);
}


static void signal_handler(int sig){
    int old_err = errno;
    int signal = sig;
    send(sig_pipe[1], (char*)&signal, sizeof(signal), 0);
    errno = old_err;
}


static void addSignal(int sig, void (handler)(int), bool restart = true){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
        sa.sa_flags |= SA_RESTART;

    sigfillset( &sa.sa_mask );
    sigaction(sig, &sa, nullptr);
}






