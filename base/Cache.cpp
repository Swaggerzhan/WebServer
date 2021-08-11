//
// Created by Swagger on 2021/8/11.
//

#include "Cache.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

using std::string;

Cache::Cache()
:   map_(),
    mutex_()
{

}

Cache::~Cache() {
    for (auto & iter : map_){
        delete iter.second;
    }
}

bool Cache::addCache(string key) {
    MutexLockGuard lock(mutex_);
    /* 如果发现已经存在，直接返回 */
    if ( map_.find(key) != map_.end() )
        return true;

    int fd = open(key.c_str(), O_RDONLY);
    if ( fd < 0 )
        return false;
    struct stat file_infor{};
    fstat(fd, &file_infor);
    Page* page = new Page(file_infor.st_size);
    while ( read(fd, page->buf, page->len) ){
        // 持续读
    }
    map_.insert(KeyValue(key, page));
    return true;
}


char* Cache::getCache(string key, int *len) {
    MutexLockGuard lock(mutex_);
    auto iter = map_.find(key);
    if ( iter == map_.end() )
        return nullptr;
    *len = iter->second->len;
    return iter->second->buf;
}












