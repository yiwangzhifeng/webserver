#pragma once
#include"MutexLock.h"
#include"Condition.h"

class CountDownLatch:noncopyable{
    public:
        CountDownLatch(int count_):mutex(),cond(mutex),count(count_){}
        void wait(){
            MutexLockGuard mutexLockGuard(mutex);
            while(count>0)cond.wait();
        }
        void countDown(){
            MutexLockGuard mutexLockGuard(mutex);
            --count;
            if(count==0)cond.signalAll();
        }
    private:
        mutable MutexLock mutex;
        Condition cond;
        int count;
};