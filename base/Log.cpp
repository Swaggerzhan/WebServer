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
:   time_(TimeStamp::now())
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
    log_ += time_.getString();
}


Log& Log::operator<<(const short target) {
    char buf[3];
    sprintf(buf, "%d", target);
    log_ += buf;
    return *this;
}


Log& Log::operator<<(const int target) {
    char buf[5];
    sprintf(buf, "%d", target);
    log_ += buf;
    return *this;
}

Log& Log::operator<<(const long target) {
    char buf[9];
    sprintf(buf, "%ld", target);
    log_ += buf;
    return *this;
}

Log& Log::operator<<(const long long target) {
    char buf[17];
    sprintf(buf, "%lld", target);
    log_ += buf;
    return *this;
}


Log& Log::operator << (const std::string &msg) {
    log_ += " " + msg;
    return *this;
}


Log::~Log() {
    if (asyncLog){
        asyncLog->append(log_);
    }
    else
        std::cout << "log error!" << std::endl;
}
