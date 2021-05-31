//
// Created by swagger on 2021/5/29.
//

#include "Log.h"
#include "AsyncLog.h"
#include <sys/time.h>

const std::string Log::kINFO = " [ INFO ] ";
const std::string Log::kDEBUG = " [ DEBUG ] ";
const std::string Log::kERROR = " [ ERROR ] ";
const std::string Log::kFATAL = " [ FATAL ] ";
AsyncLog* Log::asyncLog = nullptr;


Log::Log(LOGLEVEL loglevel)
:   time_()
{
    switch (loglevel){
        case L_INFO:{
            log_ += kINFO;
            break;
        }
        case L_DEBUG:{
            log_ += kDEBUG;
            break;
        }
        case L_ERROR:{
            log_ += kERROR;
            break;
        }
        case L_FATAL:{
            log_ += kFATAL;
            break;
        }
        default:{
            break;
        }
    }
    gettimeofday(&time_, nullptr);
    time2str();
    log_ += time_str_;

}

Log& Log::operator << (const std::string &msg) {
    log_ += msg;
    return *this;
}

Log& Log::operator<<(const int fd) {
    char buf[8];
    sprintf(buf, "%d", fd);
    log_ += buf;
    return *this;
}


Log::~Log() {
    if (asyncLog){
        asyncLog->append(log_);
    }
}

void Log::time2str() {
    char buf1[24];
    char buf2[24];
    sprintf(buf1, "%ld", time_.tv_sec);
    sprintf(buf2, "%ld", time_.tv_usec);
    time_str_ = "(" + std::string(buf1) + ":" + buf2 + ")";
}