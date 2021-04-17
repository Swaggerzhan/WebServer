//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include "../utility.h"
#include <sys/socket.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../HTTP/HttpDecoder.h"
#include <fcntl.h>
#include <iostream>


class Request {


public:

    static int epfd;/* epoll描述符 */

    static char* index_buf; /* 主页信息 */

    int fd;/* 当前request处理的客户端描述符 */

    char *recv_buf; /* 收到的信息 */

    // 以下是HTTP请求解析后的数据
    char *method; /* 请求方法 */
    char *url; /* 请求url */
    char *user_agent; /* 客户端使用的user_agent */
    char *host; /* 请求HOST */
    char *version; /* HTTP版本 */
    bool keep_alive; /* 头字段keep_alive */

    // 以下是解析HTTP请求的状态
    CHECK_STATUS checkStatus; /* 有限状态机 */
    LINE_STATUS lineStatus; /* 行状态 */
    int read_index; /* 已经读取到的数据 */
    int checked_index;/* 行检测到的地方 */
    int start_line;/* 行开始地方 */




public:

    /**
     * 请求方法
     */
    Request();

    ~Request();

    /**
     * 请求初始化
     * @param sock
     */
    void init(int sock);

    /**
     * 尝试将数据读取到缓冲区
     *  主线程调用成功则加入线程池进行解析，否则失败
     * @return
     */
    bool read();

    /**
     * 请求处理方法的入口
     */
    void process();

    /**
     * 关闭链接，清空数据
     */
    void close_conn();

    /**
     * 解析接收到的请求入口
     * @return
     */
    HTTP_CODE process_recv();

    /**
     * 发送respond入口
     * @return
     */
    HTTP_CODE process_send();

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
    HTTP_CODE parse_all();


private:

    /**
     * 数据初始化准备，和上init不同
     */
    void init();

    /**
     * 行解析函数
     * @param buf
     * @param checked_index
     * @param read_index
     * @return 数据量不够返回 LINE_INCOMPLETE
     * 数据错误返回 LINE_BAD
     * 正常解析整个行返回LINE_OK
     */
    LINE_STATUS parse_line();

    /**
     * 解析 HTTP的请求头，将其提取出来
     * @param buf
     * @param checkStatus
     * @return
     */
    HTTP_CODE parse_request_line(char* buf);

    /**
     * 解析 HTTP 请求头字段
     * @param buf
     */
    HTTP_CODE parse_header(char* buf);

};

/**
 * 添加套接字到epoll中
 * @param epfd
 * @param fd
 * @param oneShot
 */
void addfd(int epfd, int fd, bool oneShot);

/**
 * 修改监听套接字在epoll中的状态
 * @param epfd
 * @param fd
 * @param ev
 */
void modfd(int epfd, int fd, int ev);

/**
 * 删除epoll中的监听套接字并且关闭它
 * @param epfd
 * @param fd
 */
void removefd(int epfd, int fd);


int setNonBlock(int fd);


#endif //WEBSERVER_REQUEST_H
