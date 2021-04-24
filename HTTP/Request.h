//
// Created by swagger on 2021/4/12.
//

#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H


#include "Work.h"


class Request : public Work{


public:


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
     * 请求初始化，分配地址等，配合close方法
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
     * 数据初始化准备，每次HTTP请求结束都应该重新加载此次操作
     * 主要是将Request中checked_index等一些维护HTTP解析数据重制
     */
    void clear();

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


    /**
     * 解析路由
     */
    HTTP_CODE decode_route();


};


#endif //WEBSERVER_REQUEST_H
