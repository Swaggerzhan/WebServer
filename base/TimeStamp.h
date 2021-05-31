//
// Created by swagger on 2021/5/31.
//

#ifndef ASYNCLOG_TIMESTAMP_H
#define ASYNCLOG_TIMESTAMP_H

#include <sys/time.h>
#include <string>

class TimeStamp {

public:

    TimeStamp(int64_t sec, int64_t usec);

    ~TimeStamp();

    /* 获取一个时间 */
    static TimeStamp now();

    std::string getString();

    std::string getTimeStamp();

    int64_t getSec() const { return sec_; }
    int64_t getUsec() const {return usec_; }

private:

    void to_string();

private:

    const static int kSec2Usec;

    int64_t sec_;
    int64_t usec_;

    std::string str_time_;

};


#endif //ASYNCLOG_TIMESTAMP_H
