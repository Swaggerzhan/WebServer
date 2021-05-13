//
// Created by swagger on 2021/5/10.
//

#include "TimerNode.h"



TimerNode::TimerNode(timeval expire, void *data, void(*call_bak)(void*)){
    this->expire = expire;
    /* 保存用户信息 */
    client_data = data;
    this->call_bak = call_bak;
}

