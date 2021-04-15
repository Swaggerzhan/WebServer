//
// Created by swagger on 2021/4/12.
//

#include "Request.h"


char* Request::index_buf;
int Request::epfd;


Request::Request() {


}


void Request::bufInit() {
    index_buf = new char[BUFSIZE];
    int index_fd = open("../index/index.html", O_RDONLY);
    read(index_fd, index_buf, BUFSIZE);
    close(index_fd);
}


Request::~Request(){
    delete [] buf;
}


CODE Request::process() {

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
        retCode = parse_all(buf, checkStatus, checked_index, read_index, start_line);
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

LINE_STATUS Request::parse_line(char *buf, int &checked_index, int &read_index) {

    for (; checked_index <= read_index; ++checked_index ){
        if (buf[checked_index] == '\r'){
            /* 差一个\n，行还未完整 */
            if (checked_index + 1 > read_index )
                return LINE_INCOMPLETE;
            /* 找到一个完整的行！ */
            if (buf[checked_index + 1] == '\n' ){
                /* 这里不使用简写更清晰 */
                buf[checked_index] = '\0';
                ++ checked_index;
                buf[checked_index] = '\0';
                /* 将其指向下一个新数据 */
                ++ checked_index;
                /* 返回行正常 */
                return LINE_OK;
            }
            /* 如果\r后不接\n表示出错！ */
            return LINE_BAD;
        }else if ( buf[checked_index] == '\n'){
            /* 处理上次\r之后数据未完整的情况 */
            if (buf[checked_index - 1] == '\r'){
                buf[checked_index-1] = '\0';
                buf[checked_index] = '\0';
                ++ checked_index;
                return LINE_OK;
            }
            /* \n之前不以\r结尾则发生行错误 */
            return LINE_BAD;
        }
    }
    /* 整个循环结束还没有访问\r\n表示行不完整，需要继续接收数据 */
    return LINE_INCOMPLETE;
}


HTTP_CODE Request::parse_request_line(char *buf, CHECK_STATUS &checkStatus) {
    char *tmp = buf;/* 指向数据头部 */
    int cur = 0; /* 解析索引 */
    char* method;/* 请求方法 */
    char* url; /* 请求url */
    char* version; /* http版本 */
    int count = 1; /* 按顺序解析 method url version */
    while (tmp[cur] != '\0'){
        /* 找到间隔地方 */
        if (tmp[cur] == ' ' || tmp[cur] == '\t'){
            tmp[cur] = '\0';
            if (count == 1){
                method = tmp;
                ++ count;
                /* 将其指向下一个数据地址 */
                tmp += (cur + 1);
            }else if (count == 2){
                url = tmp;
                ++ count;
                tmp += (cur + 1);
            }else {
                version = tmp;
            }
        }
        ++ cur;
    }
    int l;
    l = strlen(method); request_data[0] = new char[l+1]; request_data[0][l] = '\0';
    l = strlen(url); request_data[1] = new char[l+1]; request_data[1][l] = '\0';
    l = strlen(version); request_data[2] = new char[l+1]; request_data[2][l] = '\0';
    strcpy(request_data[0], method);
    strcpy(request_data[1], url);
    strcpy(request_data[2], version);
    /* 更改有限状态机 */
    checkStatus = CHECK_HEADER;
    if (strcasecmp(method, "GET") == 0)
        return GET_REQUEST;
    if (strcasecmp(method, "POST") == 0)
        return POST_REQUEST;
    return BAD_REQUEST;

}


HTTP_CODE Request::parse_header(char *buf) {
    char *tmp = buf;
    /* 这一行是最后一行 */
    if ( tmp[0] == '\0' )
        return GET_REQUEST;
    else if ( strncasecmp(tmp, "Host:", 5) == 0){
        tmp += 5;
        tmp += strspn(tmp, " \t");
        printf("Host is %s\n", tmp);
    }else if ( strncasecmp(tmp, "User-Agent:", 11) == 0){
        tmp += 11;
        tmp += strspn(tmp, " \t");
        printf("User-Agent is %s\n", tmp);
    }else{

    }
    return INCOMPLETE_REQUEST;

}


HTTP_CODE Request::parse_all(char *buf, CHECK_STATUS &checkStatus, int &checked_index,
                                 int &read_index, int &start_line) {
    LINE_STATUS lineStatus = LINE_OK;
    HTTP_CODE retCode;
    /* 行正确就一只解析下去 */
    while ( (lineStatus = parse_line(buf, checked_index, read_index)) == LINE_OK ){
        char *tmp = buf + start_line;
        start_line = checked_index;
        switch ( checkStatus ){
            /* 解析请求头 */
            case CHECK_REQUEST_LINE: {
                retCode = parse_request_line(tmp, checkStatus);
                /* 解析出错，直接返回 */
                if (retCode == BAD_REQUEST)
                    return BAD_REQUEST;
                break;
            }
                /* 解析出头字段 */
            case CHECK_HEADER: {
                retCode = parse_header( tmp );
                if ( retCode == BAD_REQUEST )
                    return BAD_REQUEST;
                /* 这里就算解析成功了 */
                if (retCode == GET_REQUEST )
                    return GET_REQUEST;
                break;
            }
            default:{return INTERNAL_ERROR;}
        }
    }
    /* 数据不够 */
    if (lineStatus == LINE_INCOMPLETE)
        return INCOMPLETE_REQUEST;
    return BAD_REQUEST;
}
