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



void *consumeFunc(void* arg){
    WorkLine* workLine = (WorkLine*)arg;
    ConsumeThread* consumeThread = new ConsumeThread(workLine);


}

void *producerFunc(void* arg){
    WorkLine* workLine = (WorkLine*)arg;
    ProducerThread producerThread = new ProducerThread(workLine);
}



class ConsumeThread{


public:
    WorkLine* workLine;

    ConsumeThread(WorkLine* workLine){
        this->workLine = workLine;
        while (1){

            Work* work = workLine->getWork();
            work->RUN();

        }

    }
};


class ProducerThread{

public:

    WorkLine* workLine;

    /**
     * 生产者线程
     * @param workLine
     * @param number
     */
    ProducerThread(WorkLine* workLine, int number){
        this->workLine = workLine;
        srand(time(nullptr));

        /* 生产新Work，并且加入其生产线中 */
        while (number > 0){
            Work* newWork = new Work(number, (rand() % 10));
            workLine->addWork(newWork);
            number --;
        }


    }


};


class ThreadPool{

    static int count;

    pthread_t *TP;


private:



    ThreadPool(int num, WorkLine* workLine){

        cout << "pool start..." << endl;
        TP = new pthread_t[num];
        /* 启动生产者线程 */
        pthread_create(&TP[0], nullptr, producerFunc, (void*)workLine);

        for (int i=1; i<num; i++){
            /* 启动消费者线程 */
            pthread_create(&TP[i], nullptr, consumeFunc, (void*)workLine);
        }
        for (int i=0; i<num; i++){
            pthread_join(TP[i], nullptr);

        }
    }

    ~ThreadPool(){
        // delete [] TP;
    }

public:

    static ThreadPool* getPool(int num, WorkLine* workLine){
        count = 1;
        if (count != 0)
            return nullptr;

        ThreadPool* ret = new ThreadPool(num, workLine);
        return ret;
    }


};


class Work{

private:

    int run_time;
    int id;

public:
    Work(int id, int number){
        this->run_time = number;
        this->id = id;
    }

    ~Work(){}

    /*
     * 这里的RUN完全可以替换为指针函数
     * 其中指向真正的工作方法
     * */
    void RUN(){
        while (run_time > 0){
               sleep(1);
               run_time --;
        }
        cout << "work " << id << " finished!" << endl;
    }
};


class WorkLine{

private:

    queue<Work*> workLine;

    pthread_mutex_t lock;

public:

    WorkLine(){
        lock = PTHREAD_MUTEX_INITIALIZER;

    }

    Work* getWork(){
        pthread_mutex_lock(&lock);
        Work* retWork;

        while ( (retWork = workLine.front()) == nullptr)

        workLine.pop();
        pthread_mutex_unlock(&lock);
        return retWork;

    }

    void addWork(Work* work){
        pthread_mutex_lock(&lock);
        workLine.push(work);
        pthread_mutex_unlock(&lock);

    }



};


void threadPoolMain(){
    WorkLine* workLine = new WorkLine;
    /* 1个生产者线程，8个消费者线程 */
    ThreadPool* pool = ThreadPool::getPool(9, workLine);


}



#endif //WEBSERVER_THREADPOOL_H
