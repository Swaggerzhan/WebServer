//
// Created by Swagger on 2021/8/11.
//

#ifndef WEBSERVER_CACHE_H
#define WEBSERVER_CACHE_H

#include <iostream>
#include <map>
#include "Mutex.h"

class Cache{
public:
    Cache();
    ~Cache();

    bool addCache(std::string key);

    char* getCache(std::string key, int* len);

    /* 返回errno */
    int hasFile(std::string& key);


private:

    struct Page{
        Page(size_t file_len){
            buf = new char[file_len + 1];
            len = file_len;
        }
        char* buf;
        size_t len;
        ~Page(){
            delete buf;
        }
    };

    typedef std::pair<std::string, Page*> KeyValue;

    std::map<std::string, Page*> map_;
    MutexLock mutex_;

};

#endif //WEBSERVER_CACHE_H
