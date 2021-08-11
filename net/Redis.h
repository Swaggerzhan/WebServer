//
// Created by Swagger on 2021/8/11.
//

#ifndef WEBSERVER_REDIS_H
#define WEBSERVER_REDIS_H

#include <iostream>

struct redisContext;



class Redis{
public:

    Redis(std::string ip, int port);
    ~Redis();

    size_t getPage(std::string key, char* value);

    bool setPage(std::string key);


private:

    std::string ip_;
    int port_;
    redisContext* redisContext_;

    std::string hashName_;

};


#endif //WEBSERVER_REDIS_H
