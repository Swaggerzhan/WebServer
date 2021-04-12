#include <iostream>
#include "Timer/TimerHeap.h"
#include "Process/ProcessPool.h"




int main() {

    int demo = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    local_addr.sin_port = htons(7999);
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
    printf("network init ok!\n");
    ProcessPool* pool = ProcessPool::getPool(1, demo);
    pool->RUN();
//    struct sockaddr_in client_addr;
//    socklen_t client_addr_sz;
//    int client_sock = accept(demo, (sockaddr*)&client_addr, &client_addr_sz);
//    std::string data = "hello client..\n";
//    const char *buf1 = data.data();
//    send(client_sock, buf1, strlen(buf1), 0);
//    close(client_sock);
//    close(demo);


}
