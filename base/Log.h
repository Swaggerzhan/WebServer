//
// Created by swagger on 2021/5/29.
//

#ifndef ASYNCLOG_LOG_H
#define ASYNCLOG_LOG_H

#include <string>
#include "TimeStamp.h"

class AsyncLog;

enum LOGLEVEL{
    L_BACKTRACE,
    L_INFO,
    L_DEBUG,
    L_ERROR,
    L_FATAL,
};


class Log {
public:

    Log(LOGLEVEL loglevel);

    Log& operator << (const std::string& msg);
    Log& operator << (short);
    Log& operator << (int);
    Log& operator << (long);
    Log& operator << (long long);

    ~Log();

public:

    static AsyncLog* asyncLog;

private:

    TimeStamp time_;
    std::string log_;

    const static std::string kINFO;
    const static std::string kDEBUG;
    const static std::string kERROR;
    const static std::string kFATAL;

};




#endif //ASYNCLOG_LOG_H
