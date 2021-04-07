//
// Created by swagger on 2021/4/7.
//



#include "DI.h"

void B::RUN(){
    cout << "RUN success" << endl;
}


A::A(B *b) {
    this->b = b;
    b->RUN();
}


void DI_TEST(){
    B* b = new B;
    A* a = new A(b);
}


