//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include "../utility.h"
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../HTTP/HttpDecoder.h"
#include <fcntl.h>
#include <iostream>


class Request {


public:

    static int epfd;/* epoll描述符 */

    int fd;/* 当前request处理的客户端描述符 */

    char *buf;

    static char* index_buf;

    /* 请求行数据，method url version */
    char *request_data[3];


public:

    /**
     * 请求方法
     */
    Request();

    ~Request();

    /**
     * 请求初始化
     * @param fd
     */
    void init(int fd);

    /**
     * 请求处理方法
     */
    CODE process();

    /**
     * 将index.html载入内存
     */
    static void bufInit();

    /**
     * 超时处理函数，删除一切数据
     */
    static void time_out(void *arg);


    /**
     * 解析 HTTP请求入口函数
     * @param buf
     * @param checkStatus
     * @param checked_index
     * @param read_index
     * @param start_line
     * @return
     */
    HTTP_CODE parse_all(char* buf, CHECK_STATUS& checkStatus,
                        int &checked_index, int &read_index, int &start_line);


private:

    /**
     * 行解析函数
     * @param buf
     * @param checked_index
     * @param read_index
     * @return 数据量不够返回 LINE_INCOMPLETE
     * 数据错误返回 LINE_BAD
     * 正常解析整个行返回LINE_OK
     */
    LINE_STATUS parse_line(char* buf, int &checked_index, int &read_index);

    /**
     * 解析 HTTP的请求头，将其提取出来
     * @param buf
     * @param checkStatus
     * @return
     */
    HTTP_CODE parse_request_line(char* buf, CHECK_STATUS& checkStatus);

    /**
     * 解析 HTTP 请求头字段
     * @param buf
     */
    HTTP_CODE parse_header(char* buf);

};


#endif //WEBSERVER_REQUEST_H
