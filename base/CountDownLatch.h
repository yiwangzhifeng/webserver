#pragma once
#include"MutexLock.h"
#include"ConditionLock.h"

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
        ConditionLock cond;
        int count;
};