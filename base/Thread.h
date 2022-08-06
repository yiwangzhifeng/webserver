#pragma once
#include<pthread.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>
#include<string>
#include"CountDownLatch.h"
#include "noncopyable.h"
using std::string;
class Thread:noncopyable{
    public:
        typedef std::function<void()> ThreadFunc;
        explicit Thread(const ThreadFunc&,const string &name=string());
        ~Thread();
        void start();
        int join();
        pid_t tid()const;
        const string &name()const{return name_;}
        bool started()const{return started_;}
    private:
        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        pid_t tid_;
        ThreadFunc func_;
        string name_;
        CountDownLatch latch_;
        void setDefaultName();
};