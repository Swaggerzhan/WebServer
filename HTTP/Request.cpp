//
// Created by swagger on 2021/4/12.
//

#include "Request.h"


char* Request::index_buf;
int Request::epfd;

const std::string Request::code_200_ = "HTTP/1.1 200 OK\r\n";
const std::string Request::code_403_ = "";
const std::string Request::code_404_ = "HTTP/1.1 404 NOT FOUND\r\n";
const std::string Request::code_500_ = "HTTP/1.1 500 INTERNAL ERROR\r\n";
const std::string Request::content_type_ = "Content-Type: ";
const std::string Request::server_ = "Server: MyWebServer/1.0.0 (Ubuntu)\r\n";
const std::string Request::content_length_ = "Content-Length: ";
const std::string Request::connection_ = "Connection: ";
Mime Request::mime_;


const char* index_html = "index.html";

Request::Request() {
    recv_buf = new char[RECVBUF];

}

void Request::init(int sock) {
    this->fd = sock;
    addfd(epfd, fd, true);
    init();
}


void Request::close_conn() {
    printf("close %d\n", fd);
    removefd(epfd, fd);

}


void Request::bufInit() {
    index_buf = new char[BUFSIZE];
    int index_fd = open("../index/index.html", O_RDONLY);
    ::read(index_fd, index_buf, BUFSIZE);
    close(index_fd);
}


Request::~Request(){
    delete [] recv_buf;
}


void Request::process() {

    /* 尝试解析数据，数据不全则重新加入epoll中等待数据来临 */
     http_code = parse_all();

     if ( http_code == INCOMPLETE_REQUEST ){
         modfd(epfd, fd, EPOLLIN);
         return;
     }

     /* 尝试去发送数据，如果发送缓冲区已满那就将其加入到epoll中监听写事件 */
     process_send();


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
    if (!http_header_send_ok){
        int len = 0;
        while ( true ){
            len = send(fd, respond_header_.c_str()+send_index, respond_header_.size()-send_index, 0);
            if (len < 0){
                if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){
                    modfd(epfd, fd, EPOLLOUT);
                    return true;
                }
                close_conn();
                return false;
            }
            if (len == 0){
                http_header_send_ok = true;
                break;
            }
            send_index += len;
        }
    }
    int len = 0;
    while ( true ){
        len = sendfile(fd, file_fd, &file_already_send_index, file_length-file_already_send_index);
        if (len < 0){
            if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){
                modfd(epfd, fd, EPOLLOUT);
                return true;
            }
            close_conn();
            return false;
        }
        if (len == 0){
            if (keep_alive){
                std::cout << "fd: " << fd  << " is keep-alive" << std::endl;
                init();
                modfd(epfd, fd, EPOLLIN);
                return true;
            }else{
                std::cout << "fd: " << fd  << " close connection..." << std::endl;
                init();
                close_conn();
                return false;
            }
        }
    }

}


HTTP_CODE Request::decode_route() {
    /* /和空路由直接返回index页面 */
    if ( (strcasecmp(url, "/") == 0) || (url == nullptr)){
        route_ = "index.html";
        return GET_REQUEST;
    }
    if (url[0] == '/')
        url++;

    /* 出现".."直接返回403 */
    if (check_dot(url)){
        return FORBIDDEN_REQUEST;
    }
    route_ = url;
    return GET_REQUEST;
}


void Request::load_content() {

    file_fd = open(route_.c_str(), O_RDONLY);
    if (file_fd < 0){
        switch (errno){
            case ENONET:{
                http_code = NOT_FOUND;
                route_ = "404.html";
                break;
            }
            case EACCES:{
                http_code = FORBIDDEN_REQUEST;
                route_ = "403.html";
                break;
            }
            default:{
                http_code = INTERNAL_ERROR;
                route_ = "500.html";
                break;
            }
        }
        file_fd = open(route_.c_str(), O_RDONLY);
    }
    /* 解析返回类型 */
    accept_type_ = mime_.getAcceptType(route_);
    struct stat file_infor{};
    fstat(file_fd, &file_infor);
    file_length = file_infor.st_size;

}



void Request::pack_http_respond(int code) {
    add_respond_head(code);
    add_content_type();
    add_content_length();
    add_connection();
    add_server();
    respond_header_ += "\r\n";
}


void Request::add_connection() {
    respond_header_ += connection_;
    if (keep_alive){
        respond_header_ += "keep-alive\r\n";
        return;
    }
    respond_header_ += "close\r\n";

}


void Request::process_send(){

    load_content();
    switch (http_code){
        case GET_REQUEST:{
            pack_http_respond(200); // http包头
            write(); // 写入由IO线程接管
            break;
        }
        case POST_REQUEST:
            break;
        case BAD_REQUEST:
            break;
        case FORBIDDEN_REQUEST:{
            pack_http_respond(403);
            write();
            break;
        }
        case INTERNAL_ERROR:{
            pack_http_respond(500);
            write();
            break;
        }
        case CLOSED_CONNECTION:
            break;
        case INCOMPLETE_REQUEST:
            break;
        case NOT_FOUND:{
            pack_http_respond(404);
            write();
            break;
        }
        default:
            break;
    }

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
    /* 返回指向第一个空格或者\t的位子 */
    url = strpbrk(buf, " \t");
    *url = '\0'; // 将空格设置为结束符
    url ++; // 跳过结束符，指向真正的url

    method = buf; // 剩下的即方法

    version = strpbrk(url, " \t");
    *version = '\0';
    version ++;
    /* HTTP/1.1默认开启keep-alive */
    keep_alive = strncasecmp(version, "HTTP/1.1", 8) == 0;


    if (decode_route() == FORBIDDEN_REQUEST)
        return FORBIDDEN_REQUEST;

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
        header_["Host"] = tmp;
    }else if ( strncasecmp(tmp, "User-Agent:", 11) == 0){
        tmp += 11;
        tmp += strspn(tmp, " \t");
        header_["User-Agent"] = tmp;
    }else if ( strncasecmp(tmp, "Connection:", 11) == 0){
        tmp += 11;
        tmp += strspn(tmp, " \t");
        header_["Connection"] = tmp;
        keep_alive = strncasecmp(tmp, "keep-alive", 10) == 0 ||
                     strncasecmp(tmp, "Keep-Alive", 10) == 0;
    }else if ( strncasecmp(tmp, "Accept:", 7) == 0){
        tmp += 7;
        tmp += strspn(tmp, " \t");
        header_["Accept"] = tmp;
    } else if ( strncasecmp(tmp, "Accept-Encoding", 15) == 0){
        tmp += 15;
        tmp += strspn(tmp, " \t");
        header_["Accept-Encoding"] = tmp;
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
                if (retCode == FORBIDDEN_REQUEST)
                    return FORBIDDEN_REQUEST;
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

    send_index = 0;
    file_length = 0;
    http_header_send_ok = false;
    file_already_send_index = 0;
    route_.clear();
    memset(recv_buf, '\0', RECVBUF);
    respond_header_.clear();

}





void Request::add_respond_head(int code) {
    switch (code){
        case 200: {
            respond_header_ += code_200_;
            break;
        }
        case 404: {
            respond_header_ += code_404_;
            break;
        }
        default:{
            respond_header_ += code_500_;
            break;
        }
    }
}



void Request::add_content_type() {
    respond_header_ += content_type_;
    respond_header_ += accept_type_; // 添加响应类型
    respond_header_ += "\r\n";
}


void Request::add_server(){
    respond_header_ += server_;
}




bool Request::check_dot(char *msg){
    int len = strlen(msg);
    for (int i=0; i<len-1; i++){
        if (msg[i] == '.' && msg[i+1] == '.')
            return true;
    }
    return false;
}


void Request::add_content_length() {
    respond_header_ += content_length_;
    char buf[40];
    sprintf(buf, "%ld", file_length);
    respond_header_ += std::string(buf);
    respond_header_ += "\r\n";
}


void addfd(int epfd, int fd, bool oneShot){
    epoll_event event{};
    event.data.fd = fd;
    event.events = EPOLLET | EPOLLIN | EPOLLRDHUP;
    if (oneShot)
        event.events |= EPOLLONESHOT;
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
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void call_back(void* arg){
    int fd = ((Request*)arg)->fd;
    int epfd = Request::epfd;
    printf("close %d\n", fd);
    removefd(epfd, fd);
}