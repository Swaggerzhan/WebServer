//
// Created by swagger on 2021/4/12.
//

#include "Request.h"


char* Request::index_buf;
int Request::epfd;

const char* code_200 = "HTTP/1.1 200 OK";
const char* code_301;
const char* code_302;
const char* code_404 = "HTTP/1.1 404 NOT FOUND";
const char* code_500 = "HTTP/1.1 500 INTERNAL ERROR";
const char* content_type = "Content-Type: text/html";
const char* server = "Server: MyWebServer/1.0.0 (Ubuntu)";


Request::Request() {


}

void Request::init(int sock) {
    this->fd = sock;
    recv_buf = new char[RECVBUF];
    send_buf = new char[SENDBUF];
    addfd(epfd, fd, true);
    init();
}


void Request::close_conn() {
    delete [] recv_buf;
    delete [] send_buf;
    removefd(epfd, fd);
}


void Request::bufInit() {
    index_buf = new char[BUFSIZE];
    int index_fd = open("../index/index.html", O_RDONLY);
    ::read(index_fd, index_buf, BUFSIZE);
    close(index_fd);
}


Request::~Request(){

}


void Request::process() {

    /* 尝试解析数据，数据不全则重新加入epoll中等待数据来临 */
     http_code = process_recv();
     if ( http_code == INCOMPLETE_REQUEST ){
         modfd(epfd, fd, EPOLLIN);
         return;
     }

     /* 尝试去发送数据，如果发送缓冲区已满那就将其加入到epoll中监听写事件 */
     bool send_code = process_send();
     if ( !send_code ){
         /* 处理异常 */
         close_conn();
     }

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


bool Request::write(){
    int len = 0;
    while ( true ){
        /* 循环写入直到写入堵塞或者成功 */
        len = send(fd, send_buf+send_index, write_index-send_index, 0);

        /* 写缓冲区已经堵塞 */
        if ( len == -1 ){
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* 发送还未完成，监听写事件 */
                modfd(epfd, fd, EPOLLOUT);
                return true;
            }
            /* 异常退出 */
            return false;

        }
        if ( (len == 0) && (send_index == write_index)){
            /* 响应成功，清空所有接收数据准备下次请求 */
            init();
            /* 重新添加到epoll中 */
            modfd(epfd, fd, EPOLLIN);
            return true;
        }
        /* 累计已写 */
        send_index += len;
    }
}


HTTP_CODE Request::process_recv() {

    /* 尝试解析HTTP请求 */
    HTTP_CODE ret = parse_all();
    return ret;

}


bool Request::process_send(){
    //TODO:所有页面的请求方式
    // 目前只实现单个index页面的方法
    if ( http_code == BAD_REQUEST ){
        add_respond_head(500);
        return write();
    }
    add_respond_head(200);
    add_content_type();
    add_server();
    add_blank();
    /* 载入内容，当前只是index.html */
    load_context();
    /* 写入失败将有主线程接管 */
    return write();

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
    checkStatus = CHECK_REQUEST_LINE;
    lineStatus = LINE_OK;
    read_index = 0;
    checked_index = 0;
    start_line = 0;
    http_recv_ok = false;

    write_index = 0;
    send_index = 0;
    memset(recv_buf, '\0', RECVBUF);
    memset(send_buf, '\0', SENDBUF);

}

void Request::add_respond_head(int code) {
    if (code == 200){
        strcpy(send_buf, code_200);
        write_index += strlen(code_200);
        add_blank();
        return;
    }
    strcpy(send_buf, code_500);
    write_index += strlen(code_500);
    add_blank();

}


void Request::add_blank() {
    strcpy(send_buf+write_index, "\r\n");
    write_index += 2;
}

void Request::add_content_type() {
    strcpy(send_buf+write_index, content_type);
    write_index += strlen(content_type);
    add_blank();
}


void Request::add_server(){
    strcpy(send_buf+write_index, server);
    write_index += strlen(server);
    add_blank();
}


void Request::load_context() {
    strcpy(send_buf+write_index, index_buf);
    write_index += strlen(index_buf);
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