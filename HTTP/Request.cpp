//
// Created by swagger on 2021/4/12.
//

#include "Request.h"





Request::Request() {
    recv_buf = new char[RECVBUF];
    send_buf = new char[SENDBUF];
    send_file_buf = new char[FILEBUF];

}

void Request::init(int sock) {
    this->fd = sock;
    addfd(epfd, fd, true);
    clear();
}



Request::~Request(){
    delete [] recv_buf;
    delete [] send_buf;
    delete [] send_file_buf;
}


bool Request::process() {

     /* 尝试解析数据，数据不全则重新加入epoll中等待数据来临 */
     http_code = parse_all();
     if ( http_code == INCOMPLETE_REQUEST ){
         modfd(epfd, fd, EPOLLIN);
         return false;
     }
     h_status = H_RESPOND;
     return true;
     /* 之后就要跳转到Respond类中进行发送 */
}


bool Request::read(){
    int len = -1;
    /* 接收缓冲区满 */
    if (read_index >= RECVBUF )
        return false;
    while ( true ){
        len = recv(fd, recv_buf, RECVBUF, 0);
        /* 读取完毕 */
        if ( len == -1 ){
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                return true;
            }
            /* 出错 */
            return false;
        }
        /* 对方直接关闭了连接，那就直接关闭即可 */
        if (len == 0){
            return false;
        }
        /* 累计已经读取到的数据 */
        read_index += len;
    }
}





HTTP_CODE Request::decode_route() {
    /* /和空路由直接返回index页面 */
    if ( (strcasecmp(url, "/") == 0) || (url == nullptr)){
        sprintf(route, "%s", "index/index.html");
        return GET_REQUEST;
    }
    /* 出现".."直接返回403 */
    //TODO:安全检测
    sprintf(route, "%s", "index");
    sprintf(route+5, "%s", url);
    return GET_REQUEST;
}



LINE_STATUS Request::parse_line() {

    for (; checked_index <= read_index; ++checked_index ){
        if (recv_buf[checked_index] == '\r'){
            /* 差一个\n，行还未完整 */
            if (checked_index + 1 > read_index )
                return LINE_INCOMPLETE;
            /* 找到一个完整的行！ */
            if (recv_buf[checked_index + 1] == '\n' ){
                /* 这里不使用简写更清晰 */
                recv_buf[checked_index] = '\0';
                ++ checked_index;
                recv_buf[checked_index] = '\0';
                /* 将其指向下一个新数据 */
                ++ checked_index;
                /* 返回行正常 */
                return LINE_OK;
            }
            /* 如果\r后不接\n表示出错！ */
            return LINE_BAD;
        }else if ( recv_buf[checked_index] == '\n'){
            /* 处理上次\r之后数据未完整的情况 */
            if (recv_buf[checked_index - 1] == '\r'){
                recv_buf[checked_index-1] = '\0';
                recv_buf[checked_index] = '\0';
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


HTTP_CODE Request::parse_request_line(char *buf) {
    /* 返回指向第一个空格或者\t的位子 */
    url = strpbrk(buf, " \t");
    *url = '\0'; // 将空格设置为结束符
    url ++; // 跳过结束符，指向真正的url

    method = buf; // 剩下的即方法

    version = strpbrk(url, " \t");
    *version = '\0';
    version ++;

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
        host = tmp;
    }else if ( strncasecmp(tmp, "User-Agent:", 11) == 0){
        tmp += 11;
        tmp += strspn(tmp, " \t");
        user_agent = tmp;
    }else if ( strncasecmp(tmp, "Connection:", 11) == 0){
        tmp += 11;
        tmp += strspn(tmp, " \t");
        keep_alive = strncasecmp(tmp, "keep-alive", 10) == 0;
    }else if ( strncasecmp(tmp, "Accept:", 7) == 0){
        tmp += 7;
        tmp += strspn(tmp, " \t");
    } else if ( strncasecmp(tmp, "Accept-Encoding", 15) == 0){
        tmp += 15;
        tmp += strspn(tmp, " \t");
    }
    return INCOMPLETE_REQUEST;

}


HTTP_CODE Request::parse_all() {

    lineStatus = LINE_OK;
    HTTP_CODE retCode;
    /* 行正确就一只解析下去 */
    while ( (lineStatus = parse_line()) == LINE_OK ){
        char *tmp = recv_buf + start_line;
        start_line = checked_index;
        switch ( checkStatus ){
            /* 解析请求头 */
            case CHECK_REQUEST_LINE: {
                retCode = parse_request_line(tmp);
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


void Request::clear(){
    checkStatus = CHECK_REQUEST_LINE;
    lineStatus = LINE_OK;
    read_index = 0;
    checked_index = 0;
    start_line = 0;
    http_recv_ok = false;
    memset(route, '\0', ROUTE_LENGTH); /* 重置路由为空 */
    memset(send_file_buf, '\0', FILEBUF);
    memset(recv_buf, '\0', RECVBUF);
    memset(send_buf, '\0', SENDBUF);

}





