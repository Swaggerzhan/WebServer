//
// Created by swagger on 2021/4/7.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <pthread.h>
#include <queue>
#include <sys/poll.h>
#include <unistd.h>
#include <ctime>
#include <iostream>


using std::queue;
using std::endl;
using std::cout;

class WorkLine;
class ThreadPool;
class ConsumeThread;
class ProducerThread;
class Work;

void *consumeFunc(void *arg);
void *producerFunc(void *arg);

void threadPoolMain();

void *consumeFunc(void* arg);

void *producerFunc(void* arg);



class ConsumeThread{


public:
    WorkLine* workLine;

    ConsumeThread(WorkLine* workLine);

};


class ProducerThread{

public:

    WorkLine* workLine;

    /**
     * 生产者线程
     * @param workLine
     * @param number
     */
    ProducerThread(WorkLine* workLine, int total_work_number);


};


class ThreadPool{



private:

    pthread_t *TP;


    ThreadPool(int num, WorkLine* workLine);

    ~ThreadPool();

public:

    static ThreadPool* getPool(int num, WorkLine* workLine);


};


class Work{

private:

    int run_time;
    int id;

public:
    Work(int work_id, int run_time);

    ~Work();

    /*
     * 这里的RUN完全可以替换为指针函数
     * 其中指向真正的工作方法
     * */
    void RUN();
};


class WorkLine{

private:

    queue<Work*> workLine;

    pthread_mutex_t lock;

public:

    WorkLine();

    Work* getWork();

    void addWork(Work* work);



};






#endif //WEBSERVER_THREADPOOL_H
