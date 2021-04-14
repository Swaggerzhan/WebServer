//
// Created by swagger on 2021/4/12.
//

#include "Request.h"


char* Request::index_buf;

Request::Request(int fd) {
    this->fd = fd;
    buf = new char[BUFSIZE];


}


void Request::bufInit() {
    index_buf = new char[BUFSIZE];
    int index_fd = open("../index/index.html", O_RDONLY);
    read(index_fd, index_buf, BUFSIZE);
    close(index_fd);
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

    //printf("sending back data!\n");
    int len = strlen(index_buf);
    if (len != send(fd, index_buf, len, 0)){
        printf("send() error!\n");
        printf("%s\n", strerror(errno));
        return -1;
    }
    return 99;

}
