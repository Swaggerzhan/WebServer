//
// Created by swagger on 2021/4/24.
//

#ifndef WEBSERVER_WORK_H
#define WEBSERVER_WORK_H

#include "../utility.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>


/**
 * HTTP请求的基类
 */
class Work {


public:


    PROCESS h_status;

    static int epfd;/* epoll描述符 */

    int fd; /* 当前request处理的客户端描述符 */

    HTTP_CODE http_code; /* 解析成功后的HTTP状态 */

    char *recv_buf; /* 接收缓冲区 */

    char *send_buf; /* 发送缓冲区 */

    char *send_file_buf; /* 发送文件缓冲区 */


    // 以下是HTTP请求解析后的数据
    bool http_recv_ok; /* http请求解析完成 */

    char *method; /* 请求方法 */

    char *url; /* 请求url */

    char *user_agent; /* 客户端使用的user_agent */

    char *host; /* 请求HOST */

    char *version; /* HTTP版本 */

    bool keep_alive; /* 头字段keep_alive */

    char route[ROUTE_LENGTH] = {}; /* 请求路由 */


public:

    /**
     * 请求处理方法的入口
     */
    virtual bool process() = 0;


    /**
     * 初始化
     */
    void init(int sock);


    /**
     * 关闭链接，清空数据
     */
    void close_conn();


    bool write();


    bool read();

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



#endif //WEBSERVER_WORK_H
