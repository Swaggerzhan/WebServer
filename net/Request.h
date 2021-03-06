//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H


#include <sys/epoll.h>
#include <string>
#include <sys/types.h>
#include <sys/sendfile.h>
#include "Mime.h"
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include "Channel.h"

class Cache;


enum CHECK_STATUS{
    CHECK_REQUEST_LINE,
    CHECK_HEADER,
};

enum ReadProcess{
    ReadError,
    ReadOk,
    ReadEof,
};


enum WriteProcess{
    WriteOk,
    WriteIncomplete,
    WriteError,
};


enum LINE_STATUS{
    LINE_OK,
    LINE_BAD,
    LINE_INCOMPLETE,
};

enum httpDecode{
    GET_REQUEST,
    POST_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION,
    INCOMPLETE_REQUEST,
    NOT_FOUND,
};




class Request {


public:


    static const std::string code_200_;
    static const std::string code_403_;
    static const std::string code_404_;
    static const std::string code_500_;
    static const std::string content_type_;
    static const std::string server_;
    static const std::string content_length_;
    static const std::string connection_;
    static const std::string route_dir_;

    Channel channel_; // Request中所保存的频道


    static Mime mime_; /* 返回类型解析器 */

    const static int recv_buf_size; /* 接收缓冲区大小 */
    char *recv_buf; /* 接收缓冲区 */
    std::string respond_header_; /* 响应头缓冲区 */

    // 以下是HTTP请求解析后的数据
    bool http_recv_ok; /* http请求解析完成 */
    char *method; /* 请求方法 */
    char *url; /* 请求url */
    char *version; /* HTTP版本 */
    bool keep_alive; /* 头字段keep_alive */
    std::string accept_type_; /* 发送文件类型 */

    // 以下是解析HTTP请求的状态
    httpDecode http_code; /* 解析成功后的HTTP状态 */
    CHECK_STATUS checkStatus; /* 有限状态机 */
    LINE_STATUS lineStatus; /* 行状态 */
    ReadProcess readStatus_; // read状态
    int read_index; /* 已经读取到的数据 */
    int checked_index;/* 行检测到的地方 */
    int start_line;/* 行开始地方 */

    std::map<std::string, std::string> header_; /* HTTP头 */

    // 以下是HTTP发送
    bool http_header_send_ok; /* http头发送完成 */
    int send_index; /* 当前已发送的字节 */
    std::string route_;
    size_t file_length;
    char* cache_buf_;               // 缓存的返回缓冲区
    off_t file_already_send_index;

    int file_fd; // 请求目标文件fd

    Cache* cache_; // 缓存

public:

    /**
     * 请求方法
     */
    Request(Cache* cache);

    ~Request();

    /**
     * 请求初始化，通过fd和event初始化Request
     * @param sock
     */
    void init(int, int);

    /**
     * 当前读取状态
     */
    bool read();

    /**
     * 将当前send_buf中的数据发送到对端
     * 正常返回true，异常返回false
     * @return
     */
    WriteProcess write();


    /**
     * 关闭链接，清空数据
     */
    void close_conn();

    /**
     * 发送respond入口
     * 主要是将所需内容载入到缓冲区中，并且尝试发送
     * 如若发送失败则将加入EPOLLOUT事件，由主线程接管。
     * @return
     */
    void process_send();

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
    httpDecode parse_all();


private:


    /**
     * 数据初始化准备，每次HTTP请求结束都应该重新加载此次操作
     * 主要是将Request中checked_index等一些维护HTTP解析数据重制
     */
    void reSet();

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
    httpDecode parse_request_line(char* buf);

    /**
     * 解析 HTTP 请求头字段
     * @param buf
     */
    httpDecode parse_header(char* buf);


    void pack_http_respond(int code);
    void add_respond_head(int code);
    void add_connection();
    void add_content_type();
    void add_server();


    /**
     * 将请求内容载入内存
     */
    void load_content();


    /**
     * 解析路由
     * 解析会进行http_code状态转移
     */
    httpDecode decode_route();


    /**
     * 需要提前调用！
     */
    void add_content_length();


    /**
     * 检测是否存在 `..` 字符串
     * @param msg
     * @return
     */
    bool check_dot(char *msg);


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


void call_back(void* arg);


#endif //WEBSERVER_REQUEST_H
