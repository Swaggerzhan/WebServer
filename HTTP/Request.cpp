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


Request::~Request(){
    delete [] buf;
    if (!decoder)
        return;
    delete decoder;
}


CODE Request::process() {
    if (!decoder)
        decoder = new HttpDecoder;

    HTTP_CODE retCode;
    CHECK_STATUS checkStatus = CHECK_REQUEST_LINE;
    int checked_index = 0;
    int read_index = 0;
    int start_line = 0;

    while ( true ){

        int data_len = recv(fd, buf+read_index, BUFSIZE-read_index, 0);
        if (data_len == -1){
            printf("recv() error!\n");
            return ERROR;
        }
        if (data_len == 0){
            printf("client closed\n");
            return CLOSE;
        }
        read_index += data_len;
        retCode = decoder->parse_all(buf, checkStatus, checked_index, read_index, start_line);
        if ( retCode == INCOMPLETE_REQUEST )
            continue;
        if ( retCode == BAD_REQUEST )
            return ERROR;
        if ( retCode == GET_REQUEST ){
            /* 提供GET请求服务 */
            break;
        }


    }
    return KEEP;

}


void Request::time_out(void *arg) {
    Request* request = (Request*)arg;
    delete request;
}
