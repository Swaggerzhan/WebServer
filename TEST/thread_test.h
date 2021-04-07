//
// Created by swagger on 2021/4/7.
//

#ifndef WEBSERVER_THREAD_TEST_H
#define WEBSERVER_THREAD_TEST_H


#include <pthread.h>
#include <iostream>
#include <sys/poll.h>


using namespace std;

int global = 88;




void *read_thread(void *arg){
    pthread_mutex_t *read_lock = (pthread_mutex_t*)arg;
    pthread_mutex_lock(read_lock);
    for (int i=0; i<100; i++){
        cout << global << endl;
        poll(nullptr, 0, 1);
    }
    pthread_mutex_unlock(read_lock);
}


void *write_thread(void *arg){

    pthread_mutex_t *write_lock = (pthread_mutex_t*)arg;

    pthread_mutex_lock(write_lock);

    for (int i=0; i<100; i++){
        global ++;
        poll(nullptr, 0, 1);
    }

    pthread_mutex_unlock(write_lock);

    pthread_t self = pthread_self();
    cout << "write_thread's id: " << self << endl;

    int *ret = new int;
    *ret = 10;

    return ret;

}

void pthreadTest(){
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, nullptr, read_thread, &lock);
    pthread_create(&t2, nullptr, write_thread, (void*)&lock);
    pthread_join(t1, nullptr);
    void *ret;
    pthread_join(t2, &ret);
    cout << "write_thread return value: " << *(int*)ret << endl;
}



#endif //WEBSERVER_THREAD_TEST_H
