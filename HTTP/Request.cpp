//
// Created by swagger on 2021/4/12.
//

#include "Request.h"


char* Request::index_buf;
int Request::epfd;


Request::Request() {


}

void Request::init(int sock) {
    checkStatus = CHECK_REQUEST_LINE; /* 有限状态机 */
    lineStatus = LINE_OK; /* 行状态 */
    read_index = 0; /* 已经读取到的数据 */
    checked_index = 0;/* 行检测到的地方 */
    start_line = 0;/* 行开始地方 */
    this->fd = sock;
    recv_buf = new char[RECVBUF];
}


void Request::close_conn() {
    delete [] recv_buf;
}


void Request::bufInit() {
    index_buf = new char[BUFSIZE];
    int index_fd = open("../index/index.html", O_RDONLY);
    read(index_fd, index_buf, BUFSIZE);
    close(index_fd);
}


Request::~Request(){

}


void Request::process() {

    /* 尝试去读数据，如果数据不够则重新加入到epoll中监听读事件 */
     HTTP_CODE recv_code = process_recv();
     if (recv_code == INCOMPLETE_REQUEST){
         modfd(epfd, fd, EPOLLIN);
         return;
     }

     /* 尝试去发送数据，如果发送缓冲区已满那就将其加入到epoll中监听写事件 */
     bool send_code = process_send();
     if ( !send_code ){

     }
    modfd(epfd, fd, EPOLLOUT);

}


bool Request::read(){
    int len = -1;
    /* 接收缓冲区满 */
    if (read_index >= RECVBUF )
        return false;
    while ( true ){
        len = recv(fd, recv_buf, RECVBUF, 0);
        /* 读取完毕 */
        if ((len == -1) && ( (errno == EAGAIN) || (errno == EWOULDBLOCK)))
            return true;
        /* 对方直接关闭了连接，那就直接关闭即可 */
        if (len == 0){
            return false;
        }
        /* 累计已经读取到的数据 */
        read_index += len;
    }
}


HTTP_CODE Request::process_recv() {
    /* 此函数后续可以添加新功能 */
    /* 尝试解析HTTP请求 */
    HTTP_CODE ret = parse_all();
    return ret;

}


HTTP_CODE Request::process_send(){

}


void Request::time_out(void *arg) {
    auto* request = (Request*)arg;
    delete request;
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
    char *tmp = buf;/* 指向数据头部 */
    int cur = 0; /* 解析索引 */
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
    }else {

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


void Request::init(){
    checkStatus = CHECK_HEADER;
    checked_index = 0;
    read_index = 0;
    start_line = 0;
}


void addfd(int epfd, int fd, bool oneShot){
    epoll_event event{};
    event.data.fd = fd;
    event.events = EPOLLET | EPOLLIN | EPOLLRDHUP;
    if (oneShot)
        event.events |= EPOLLOUT;
    /* 将套接字设置为非阻塞 */
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setNonBlock(fd);
}


void modfd(int epfd, int fd, int ev){
    epoll_event event{};
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}


void removefd(int epfd, int fd){
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
}



int setNonBlock(int fd){
    int old_option = fcntl(fd, F_GETFD);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFD, new_option);
    return old_option;
}