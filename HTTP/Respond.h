//
// Created by swagger on 2021/4/24.
//

#ifndef WEBSERVER_RESPOND_H
#define WEBSERVER_RESPOND_H

#include "Work.h"



class Respond :public Work{


public:

    // 以下是HTTP发送
    bool http_send_ok; /* http发送完成 */
    int write_index; /* 当前写入缓冲区的字节 */
    int send_index; /* 当前已发送的字节 */

    size_t file_length;


public:

    /**
     * 将当前send_buf中的数据发送到对端
     * 正常返回true，异常返回false
     * @return
     */
    bool write();


    /**
     * 发送respond入口
     * 主要是将所需内容载入到缓冲区中，并且尝试发送
     * 如若发送失败则将加入EPOLLOUT事件，由主线程接管。
     * @return
     */
    bool process_send();


private:

    void add_respond_head(int code);
    void add_blank();
    void add_content_type();
    void add_server();


    /**
     * 将请求内容载入内存
     */
    HTTP_CODE load_content();



    /**
     * 添加内容
     */
    void add_content();

    /**
     * 需要提前调用！
     */
    void add_content_length();


};


#endif //WEBSERVER_RESPOND_H
