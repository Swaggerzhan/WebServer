//
// Created by swagger on 2021/4/10.
//

#ifndef WEBSERVER_POINTERSWAP_H
#define WEBSERVER_POINTERSWAP_H


#include <iostream>

class Pointer{
public:
    int x;
    Pointer(int x){
        this->x = x;
    }
};


void swap(Pointer** left, Pointer** right){
    Pointer** tmp = left;
    *left = *right;
    *right = *tmp;
}


void Swap(Pointer** left, Pointer** right){
    Pointer* tmp = *left;
    *left = *right;
    *right = tmp;
}


void PointerTestMain(){
    Pointer *data[10];
    Pointer *f1 = new Pointer(33);
    Pointer *f2 = new Pointer(99);
    data[3] = f1;
    data[9] = f2;
    std::cout << "before data[3]: " << data[3]->x << std::endl;
    std::cout << "before data[9]: " << data[9]->x << std::endl;
    Swap(&data[3], &data[9]);
    std::cout << "data[3]: " << data[3]->x << std::endl;
    std::cout << "data[9]: " << data[9]->x << std::endl;
}

#endif //WEBSERVER_POINTERSWAP_H
