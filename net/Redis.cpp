//
// Created by Swagger on 2021/8/11.
//

#include "Redis.h"
#include <cstring>
extern "C" {
#include "hiredis.h"
}

// for debug
using std::endl;
using std::cout;
using std::string;
using std::cerr;

Redis::Redis(string ip, int port)
:   ip_(move(ip)),
    port_(port),
    redisContext_(nullptr),
    hashName_("test")
{

    redisContext_ = redisConnect(ip_.c_str(), port_);
    if ( !redisContext_ ){
        cerr << "redis Connect Error!" << endl;
        exit(1);
    }

}

Redis::~Redis() {
    if (redisContext_)
        redisFree(redisContext_);
}


size_t Redis::getPage(string key, char *value) {
    restart: // 重新读取
    void* ret = redisCommand(redisContext_, "get %s", key.c_str());
    redisReply* reply = static_cast<redisReply*>(ret);
    /* 不是字符串类型，估计出错！ */
    if ( reply->type != REDIS_REPLY_STRING){
        freeReplyObject(ret);
        if (setPage(key))
            goto restart;
        else
            return -1;
    }
    void* ret2 = redisCommand(redisContext_, "strlen %s", key.c_str());
    redisReply* reply2 = static_cast<redisReply*>(ret);
    if ( reply2->type != REDIS_REPLY_INTEGER ){
        cerr << "strlen Error!" << endl;
        return -1;
    }
    size_t len = reply2->integer;
    freeReplyObject(reply2);
    memcpy(value, reply->str, len);
    return len;
}


bool Redis::setPage(string key) {


    return true;

}














