//
// Created by swagger on 2021/4/12.
//

#include "Request.h"
#include "../base/Log.h"
#include <cstring>
#include <unistd.h>
#include <malloc.h>
#include <sys/socket.h>
#include "../base/Cache.h"
#include <cassert>

const std::string Request::code_200_ = "HTTP/1.1 200 OK\r\n";
const std::string Request::code_403_ = "";
const std::string Request::code_404_ = "HTTP/1.1 404 NOT FOUND\r\n";
const std::string Request::code_500_ = "HTTP/1.1 500 INTERNAL ERROR\r\n";
const std::string Request::content_type_ = "Content-Type: ";
const std::string Request::server_ = "Server: MyWebServer/1.0.0 (Ubuntu)\r\n";
const std::string Request::content_length_ = "Content-Length: ";
const std::string Request::connection_ = "Connection: ";
const std::string Request::route_dir_ = "index_page/";
Mime Request::mime_;

const int Request::recv_buf_size = 4096;


Request::Request(Cache* cache)
:   channel_(this),
    cache_(cache)
{
    recv_buf = new char[recv_buf_size];
    //recv_buf = (char*)malloc(recv_buf_size);
}

void Request::init(int fd, int ev) {
    // for debug
    //assert( fd != 0);
    channel_.setfd(fd);
    channel_.setEvent(ev);
    channel_.setUsed(); // channel启用
    reSet(); //调用本地reSet()

}


void Request::close_conn() {
    /* 直接关闭链接 */
    //assert( channel_.getfd() != 0 );
//    if ( channel_.getfd() == 0 )
//        std::cout << "close fd: 0" << std::endl;
    ::close(channel_.getfd());
    //Log(L_DEBUG) << "close: " << channel_.getfd();
    channel_.reSet();

}


Request::~Request(){
    delete [] recv_buf;
}


bool Request::read(){
    int len = -1;
    /* 接收缓冲区满 */
    if (read_index >= recv_buf_size ){
        readStatus_ = ReadError;
        return false;
    }
    while ( true ){
        len = recv(channel_.getfd(), recv_buf, recv_buf_size, 0);
        /* 读取完毕 */
        if ( len == -1 ){
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                /* 只有这里是正常读取，此时应该是在EPOLL循环中，将自己加入到线程池中去 */
                readStatus_ = ReadOk;
               // std::cout << recv_buf << std::endl;
//                std::cout << "recv: " << std::endl;
//                std::cout << recv_buf << std::endl;
//                std::cout << "------end-------" << std::endl;
                return true;
            }
            /* 出错 */
            readStatus_ = ReadError;
            return false;
        }
        /* 对方直接关闭了连接，那就直接关闭即可 */
        if (len == 0){
            //Log(L_INFO) << "recv EOF from" << channel_.getfd();
            readStatus_ = ReadEof;
            return false;
        }
        /* 累计已经读取到的数据 */
        read_index += len;
    }
}


WriteProcess Request::write(){
//    std::cout << "server send header: " << std::endl;
//    std::cout << respond_header_ << std::endl;
//    std::cout << "-------end----------" << std::endl;
    if (!http_header_send_ok){
        int len = 0;
        while ( true ){
            len = ::send(channel_.getfd(), respond_header_.c_str()+send_index, respond_header_.size()-send_index, 0);
            if (len < 0){
                if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){
                    return WriteIncomplete;
                }else{
//                    std::cout << "Header WriteError: " << strerror(errno) << std::endl;
//                    std::cout << "fd: " << channel_.getfd() << std::endl;
                    if ( (channel_.getRetEvent() & EPOLLHUP)){
                        std::cout << "EPOLLHUP" << std::endl;
                    }
                    if ((channel_.getRetEvent() & EPOLLRDHUP)){
                        std::cout << "EPOLLRDHUP" << std::endl;
                    }

                    return WriteError;
                }
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
        assert( channel_.getIsUsed() );
        len = ::send(channel_.getfd(), cache_buf_, file_length-file_already_send_index, 0);
        file_already_send_index += len;
        if (len < 0){
            if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ){
                return WriteIncomplete;
            }else{
//                std::cout << "Context WriteError: " << strerror(errno) << std::endl;
//                std::cout << "fd: " << channel_.getfd() << std::endl;
                return WriteError;
            }
        }
        if (len == 0){
//            reSet();
//            close(file_fd);
//            //std::cout << "send len: " << file_already_send_index << std::endl;
//            return WriteOk;
            if (keep_alive){
                reSet(); // 重置
                //close(file_fd);
                return WriteOk;
                //TODO: keep-alive处理，在Request? 还是HttpServer？
            }else{
                //close(file_fd);
                return WriteOk;
            }
        }
    }

}


httpDecode Request::decode_route() {
    /* /和空路由直接返回index页面 */
    if ( (strcasecmp(url, "/") == 0) || (url == nullptr)){
        route_ += "index.html";
        return GET_REQUEST;
    }
    if (url[0] == '/')
        url++;

    /* 出现".."直接返回403 */
    if (check_dot(url)){
        return FORBIDDEN_REQUEST;
    }
    route_ += url;
    return GET_REQUEST;
}


void Request::load_content() {
    start:
    int* len = new int;
    char* buf = cache_->getCache(route_, len);
    if ( !buf ){ // 不存在缓存
        int retCode = 0;
        if ( ( retCode = cache_->hasFile(route_)) == 0 ){ // 加载缓存
            cache_->addCache(route_);
        }else { // 返回错误代码
            switch ( retCode ){
                case ENOENT:{
                    http_code = NOT_FOUND;
                    route_ = route_dir_ + "404.html";
                    break;
                }
                case EACCES:{
                    http_code = FORBIDDEN_REQUEST;
                    route_ = route_dir_ + "403.html";
                    break;
                }
                default:{
                    http_code = INTERNAL_ERROR;
                    route_ = route_dir_ + "500.html";
                    break;
                }
            }
        }
        goto start;
    }else { // 缓存已经存在
        accept_type_ = mime_.getAcceptType(route_);
        file_length = *len;
        cache_buf_ = buf;
    }

//    file_fd = open(route_.c_str(), O_RDONLY);
//    if (file_fd < 0){
//        switch (errno){
//            case ENOENT:{
//                http_code = NOT_FOUND;
//                route_ = route_dir_ + "404.html";
//                break;
//            }
//            case EACCES:{
//                http_code = FORBIDDEN_REQUEST;
//                route_ = route_dir_ + "403.html";
//                break;
//            }
//            default:{
//                http_code = INTERNAL_ERROR;
//                route_ = route_dir_ + "500.html";
//                break;
//            }
//        }
//        file_fd = open(route_.c_str(), O_RDONLY);
//    }
//    /* 解析返回类型 */
//    accept_type_ = mime_.getAcceptType(route_);
//    struct stat file_infor{};
//    fstat(file_fd, &file_infor);
//    file_length = file_infor.st_size;

}



void Request::pack_http_respond(int code) {
    add_respond_head(code);
    add_content_type();
    add_content_length();
    add_connection();
    add_server();
    respond_header_ += "\r\n";
    //std::cout << respond_header_ << std::endl;

}


void Request::add_connection() {

    if (keep_alive){
        respond_header_ += connection_;
        respond_header_ += "keep-alive\r\n";
        respond_header_ += "keep-alive: timeout=10, max=1000\r\n";
        respond_header_ += "Mother: dead\r\n";
        return;
    }else {
        respond_header_ += connection_;
        respond_header_ += "close\r\n";
    }

}


void Request::process_send(){

    load_content();
    switch (http_code){
        case GET_REQUEST:{
            //Log(L_INFO) <<channel_.getfd()<<"GET REQUEST"<<version<<url<<header_["Host:"];
            pack_http_respond(200); // http包头
            //write(); // 写入由IO线程接管
            break;
        }
        case POST_REQUEST: {
            //Log(L_INFO) <<channel_.getfd() <<"POST REQUEST"<< version<< url<<header_["Host:"];
            break;
        }
        case BAD_REQUEST: {
            //Log(L_ERROR) <<channel_.getfd()<< recv_buf;
            break;
        }
        case FORBIDDEN_REQUEST:{
            //Log(L_ERROR) <<channel_.getfd()<<"FORBIDDEN"<<version<<url<<header_["Host:"];
            pack_http_respond(403);
            //write();
            break;
        }
        case INTERNAL_ERROR:{
            //Log(L_ERROR) <<channel_.getfd()<<route_<<recv_buf;
            pack_http_respond(500);
            //write();
            break;
        }
        case CLOSED_CONNECTION:
            break;
        case INCOMPLETE_REQUEST:
            break;
        case NOT_FOUND:{
            //Log(L_ERROR) <<channel_.getfd()<<"404 NOTFOUND"<<version<<route_<<header_["Host:"];
            pack_http_respond(404);
            //write();
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


httpDecode Request::parse_request_line(char *buf) {
    /* 返回指向第一个空格或者\t的位子 */
    url = strpbrk(buf, " \t");
    if (!url)
        return BAD_REQUEST;
    *url = '\0'; // 将空格设置为结束符
    url ++; // 跳过结束符，指向真正的url

    method = buf; // 剩下的即方法

    version = strpbrk(url, " \t");
    if (!version)
        return BAD_REQUEST;
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


httpDecode Request::parse_header(char *buf) {
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
        if (strncasecmp(version, "HTTP/1.1", 8) == 0){
            keep_alive = true;
            //std::cout << "HTTP/1.1 default keep-alive" << std::endl;
        }else{
            //std::cout << "HTTP/1.1 error!" << std::endl;
        }

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


httpDecode Request::parse_all() {

    lineStatus = LINE_OK;
    httpDecode retCode;
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


void Request::reSet() {
    checkStatus = CHECK_REQUEST_LINE;
    lineStatus = LINE_OK;
    http_code = GET_REQUEST;
    read_index = 0;
    checked_index = 0;
    start_line = 0;
    http_recv_ok = false;

    send_index = 0;
    file_length = 0;
    http_header_send_ok = false;
    file_already_send_index = 0;
    cache_buf_ = nullptr;
    //route_.clear();
    route_ = route_dir_;
    memset(recv_buf, '\0', recv_buf_size);
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
