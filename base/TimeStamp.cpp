//
// Created by swagger on 2021/5/31.
//

#include "TimeStamp.h"
#include <iostream>

const int TimeStamp::kSec2Usec = 1000 * 1000;



TimeStamp::TimeStamp(int64_t sec, int64_t usec)
:   sec_(sec),
    usec_(usec)
{
    to_string();
}

TimeStamp::~TimeStamp() {}

TimeStamp TimeStamp::now() {
    timeval time{};
    gettimeofday(&time, nullptr);
    return TimeStamp(time.tv_sec, time.tv_usec);
}

std::string TimeStamp::getString() {
    return str_time_;
}

std::string TimeStamp::getTimeStamp() {
    char buf1[17];
    char buf2[17];
    sprintf(buf1, "%ld", sec_);
    sprintf(buf2, "%ld", usec_);
    std::string tmp = buf1;
    tmp += "-";
    tmp += buf2;
    return tmp;
}


void TimeStamp::to_string() {
    struct tm tm_time{};
    time_t sec = static_cast<time_t>(sec_);
    gmtime_r(&sec, &tm_time);
    tm_time.tm_hour -= 4;
    char buf[48];
    char buf2[12];
    sprintf(buf2, "%ld", usec_);
    strftime(buf, 48, "%04Y%02m%02d %H:%M:%S", &tm_time);
    str_time_ += buf;
    str_time_ += " ";
    str_time_ += buf2;
    str_time_ += "Z";
}

