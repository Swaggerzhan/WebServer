//
// Created by swagger on 2021/4/8.
//
#include "threadPool.h"



void *consumeFunc(void* arg){
    cout << "consume thread number: " << pthread_self() << endl;
    WorkLine* workLine = (WorkLine*)arg;
    ConsumeThread* consumeThread = new ConsumeThread(workLine);
    return (void*)1;
}


void *producerFunc(void* arg){
    cout << "producer thread number: " << pthread_self() << endl;
    WorkLine* workLine = (WorkLine*)arg;
    ProducerThread *producerThread = new ProducerThread(workLine, 9);
    return (void*)1;
}


ConsumeThread::ConsumeThread(WorkLine* workLine){
    this->workLine = workLine;
    while (1) {
        Work *work = workLine->getWork();
        if (work == nullptr){
            cout << "not work to do, waiting...." << endl;
            // poll(nullptr, 0, 5);
            sleep(1);
            continue;
        }

        work->RUN();
    }
}


ProducerThread::ProducerThread(WorkLine* workLine, int total_work_number){
    this->workLine = workLine;
    srand(time(nullptr));

    /* 生产新Work，并且加入其生产线中 */
    while (total_work_number > 0){
        cout << "total_work_number: " << total_work_number << endl;
        Work* newWork = new Work(total_work_number, (rand() % 10));
        workLine->addWork(newWork);
        total_work_number --;
    }

}


ThreadPool::ThreadPool(int thread_number, WorkLine *workLine) {

    cout << "pool start..." << endl;
    TP = new pthread_t[thread_number];
    /* 启动生产者线程 */
    pthread_create(&TP[0], nullptr, producerFunc, (void*)workLine);

    for (int i=1; i<thread_number; i++){
        /* 启动消费者线程 */
        pthread_create(&TP[i], nullptr, consumeFunc, (void*)workLine);
    }
    for (int i=0; i<thread_number; i++){
        pthread_join(TP[i], nullptr);

    }
}


ThreadPool *ThreadPool::getPool(int thread_number, WorkLine *workLine) {
    static ThreadPool* instance;
    if ( instance == nullptr)
        return (instance = new ThreadPool(thread_number, workLine));
    return instance;
}


Work::Work(int work_id, int run_time) {
    this->run_time = run_time;
    this->id = work_id;
}

void Work::RUN() {
    while (run_time > 0){
        sleep(1);
        run_time --;
    }
    cout << "work " << id << " finished!" << endl;
}


WorkLine::WorkLine() {
    lock = PTHREAD_MUTEX_INITIALIZER;
}

/* 死锁问题 */
//Work* WorkLine::getWork() {
//    pthread_mutex_lock(&lock);
//    Work* retWork;
//
//    while ( (retWork = workLine.front()) == nullptr){}
//
//    workLine.pop();
//    pthread_mutex_unlock(&lock);
//    return retWork;
//
//}

Work* WorkLine::getWork() {
    if (pthread_mutex_trylock(&lock) == EBUSY){
        poll(nullptr, 0, 1);
    }
    Work* retWork;
    retWork = workLine.front();
    if (retWork == nullptr){
        pthread_mutex_unlock(&lock);
        return nullptr;
    }
    workLine.pop();
    pthread_mutex_unlock(&lock);
    return retWork;

}


void WorkLine::addWork(Work *work) {
    while (pthread_mutex_trylock(&lock) == EBUSY){

    }
    workLine.push(work);
    pthread_mutex_unlock(&lock);

}

void threadPoolMain() {

    WorkLine* workLine = new WorkLine;
    /* 1个生产者线程，8个消费者线程 */
    ThreadPool* pool = ThreadPool::getPool(9, workLine);

}


