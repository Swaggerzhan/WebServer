//
// Created by swagger on 2021/4/7.
//

#ifndef WEBSERVER_DI_H
#define WEBSERVER_DI_H



#include <iostream>

using std::endl;
using std::cout;


class A;
class B;

class B{
public:

    void RUN();

};



class A{

public:
    B* b;

    A(B* b);
};


void DI_TEST();

#endif //WEBSERVER_DI_H
