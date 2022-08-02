#pragma once
#include<pthread.h>
#include <unistd.h>
#include<string>

#include "noncopyable.h"

class Thread:noncopyable{
    public:
        Thread();
        pid_t tid()const{return tid_;};
    private:
        pid_t tid_;
};