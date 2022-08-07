#pragma once
#include<errno.h>
#include<ctime>
#include"MutexLock.h"
#include"noncopyable.h"

class Condition:noncopyable{
    public:
        explicit Condition(MutexLock &mutex_):mutex(mutex_){
            pthread_cond_init(&cond,NULL);
        }
        ~Condition(){pthread_cond_destroy(&cond);}
        void wait(){pthread_cond_wait(&cond,mutex.get());}
        void signal(){pthread_cond_signal(&cond);}
        void signalAll(){pthread_cond_broadcast(&cond);}
        bool timeWait(int seconds) {
            timespec abstime;
            clock_gettime(CLOCK_REALTIME, &abstime);
            abstime.tv_sec += static_cast<time_t>(seconds);
            return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
        }
    private:
        MutexLock &mutex;
        pthread_cond_t cond;
};