//
// Created by swagger on 2021/4/7.
//

#ifndef WEBSERVER_THREAD_TEST_H
#define WEBSERVER_THREAD_TEST_H


#include <pthread.h>
#include <iostream>


using namespace std;

int global = 88;


void *read_thread(void *arg){
    for (int i=0; i<100; i++){
        cout << global << endl;
    }
}


void *write_thread(void *arg){

    for (int i=0; i<100; i++){
        global ++;
    }
}

void pthreadTest(){
    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, nullptr, read_thread, nullptr);
    pthread_create(&t2, nullptr, write_thread, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
}



#endif //WEBSERVER_THREAD_TEST_H
