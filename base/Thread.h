//
// Created by swagger on 2021/5/19.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H


#include <pthread.h>
#include <functional>

class Thread {
public:

    typedef std::function<void()> Functor;

    Thread(Functor cb);

    ~Thread();



private:

    pthread_t threadId_;
    pid_t pId_;



};


#endif //WEBSERVER_THREAD_H
