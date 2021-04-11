#include <iostream>
#include <sys/poll.h>
#include "Timer/TimerHeap.h"
#include "Process/ProcessPool.h"




int main() {

    int demo = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_addr.s_addr = inet_addr(HOST);
    local_addr.sin_port = PORT;
    local_addr.sin_family = AF_INET;
    /* 设置重复使用地址便于测试 */
    int reuse = 1;
    setsockopt(demo, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    /* 将地址和套接字绑定 */
    if (bind(demo, (sockaddr*)&local_addr, sizeof(local_addr)) == -1){
        printf("bind() error!\n");
        exit(1);
    }
    /* 监听端口 */
    if ( listen(demo, 5) == -1){
        printf("listen() error!\n");
        exit(1);
    }
    ProcessPool* pool = ProcessPool::getPool(8, demo);
    pool->RUN();


}
