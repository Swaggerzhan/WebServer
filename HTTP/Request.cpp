//
// Created by swagger on 2021/4/12.
//

#include "Request.h"


Request::Request(int fd) {
    this->fd = fd;
    buf = new char[BUFSIZE];
}


Request::~Request() {
    delete [] buf;
}


int Request::process() {
    int ret = -1;
    /* ET模式下，一次保证将所有数据全部收起 */
//    while((ret = recv(fd, buf+ret, BUFSIZE-ret, 0)) > 0){
//        /* 期间有限状态机就可以顺序解析http请求了 */
//    }
    memset(buf, 0, BUFSIZE);
    ret = recv(fd, buf, BUFSIZE, 0);

    if (ret == 0){
        /* 客户端关闭了链接 */
        //printf("client close\n");
        return 0;
    }


    /* 以下是数据模拟 */
    char retData[] = "hello client\n";
    //printf("sending back data!\n");
    int len = strlen(retData);
    if (len != send(fd, retData, len, 0)){
        printf("send() error!\n");
        printf("%s\n", strerror(errno));
        return -1;
    }
    return 99;

}
