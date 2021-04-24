//
// Created by swagger on 2021/4/24.
//

#include "Respond.h"



const char* code_200 = "HTTP/1.1 200 OK";
const char* code_301;
const char* code_302;
const char* code_404 = "HTTP/1.1 404 NOT FOUND";
const char* code_500 = "HTTP/1.1 500 INTERNAL ERROR";
const char* content_type = "Content-Type: text/html";
const char* server = "Server: MyWebServer/1.0.0 (Ubuntu)";
const char* content_length = "Content-Length: ";
const char* index_html = "index.html";




bool Respond::write(){
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
            close_conn();
            return false;

        }
        if ( (len == 0) && (send_index == write_index)){
            /* 响应成功，清空所有接收数据准备下次请求 */
            if (keep_alive){
                init();
                /* 重新添加到epoll中 */
                modfd(epfd, fd, EPOLLIN);
                return true;
            }else{
                init();
                //printf("not keep-alive\n");
                close_conn();
                return false;
            }

        }
        /* 累计已写 */
        send_index += len;
    }
}



HTTP_CODE Respond::load_content() {

    int file_fd = open(route, O_RDONLY);
    if (file_fd < 0){
        /* 文件不存在，切换成读取404文件 */
        if (errno == ENOENT){
            http_code = NOT_FOUND;
            file_fd = open("index/404.html", O_RDONLY);
        }else{
            exit_error("load_content() open() ", false);
            return INTERNAL_ERROR;
        }
    }
    int len = 0;
    while ( (len = ::read(file_fd, send_file_buf, FILEBUF)) > 0 ){
        if (len == -1){
            exit_error("load_content()", false);
            break;
        }
        file_length += len;
    }
    close(file_fd);

}




bool Respond::process_send(){
    //TODO:所有页面的请求方式
    if ( http_code == BAD_REQUEST ){
        add_respond_head(500);
        return write();
    }
    decode_route();

    load_content(); /* 将需要请求的页面载入send_file_buf */
    /* 文件不存在 */
    if (http_code == NOT_FOUND){
        add_respond_head(404);
        add_content_type();
        add_content_length();
        add_server();
        add_blank();
        add_content();
        return write();
    }
    add_respond_head(200); /* 添加相应头 */
    add_content_type();
    add_content_length(); /* 添加长度 */
    add_server();
    add_blank();
    /* 添加主要内容 */
    add_content();
    /* 写入失败将有主线程接管 */
    return write();

}



void Respond::add_respond_head(int code) {
    if (code == 200){
        strcpy(send_buf, code_200);
        write_index += strlen(code_200);
        add_blank();
        return;
    }
    if (code == 404){
        strcpy(send_buf, code_404);
        write_index += strlen(code_404);
        add_blank();
        return;
    }
    strcpy(send_buf, code_500);
    write_index += strlen(code_500);
    add_blank();

}


void Respond::add_blank() {
    strcpy(send_buf+write_index, "\r\n");
    write_index += 2;
}

void Respond::add_content_type() {
    strcpy(send_buf+write_index, content_type);
    write_index += strlen(content_type);
    add_blank();
}


void Respond::add_server(){
    strcpy(send_buf+write_index, server);
    write_index += strlen(server);
    add_blank();
}


void Respond::add_content() {
    strcpy(send_buf+write_index, send_file_buf);
    write_index += file_length;
}


void Respond::add_content_length() {

    sprintf(send_buf+write_index, "%s", content_length);
    write_index += strlen(content_length);
    sprintf(send_buf+write_index, "%d", (int)file_length);
    write_index = strlen(send_buf);
    add_blank();
}

