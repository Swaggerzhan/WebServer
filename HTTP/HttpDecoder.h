//
// Created by swagger on 2021/4/14.
//

#ifndef WEBSERVER_HTTPDECODER_H
#define WEBSERVER_HTTPDECODER_H


#include <iostream>
#include <cstring>
#include "../utility.h"


class HttpDecoder {

private:

    /* 请求行数据，method url version */
    char *request_data[3];



public:

    /**
     * 构造函数
     */
    HttpDecoder();


    ~HttpDecoder();

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


#endif //WEBSERVER_HTTPDECODER_H
