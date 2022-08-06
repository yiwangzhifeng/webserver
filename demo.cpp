#include <pthread.h>
#include <iostream>
#include <unistd.h>
 
 
using namespace std;
 
static pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
 
static void* func_1(void* arg)
{
    cout << "func_1 start" << endl;
 
    pthread_mutex_lock(&mtx);
    cout << "func_1 lock mtx" << endl;
 
    cout << "func_1 wait cond" << endl;
    pthread_cond_wait(&cond, &mtx);
 
    cout << "func_1 unlock mtx" << endl;
    pthread_mutex_unlock(&mtx);
    
    cout << "func_1 end" << endl;
    sleep(5);
    
    return NULL;
}
 
static void* func_2(void* arg)
{
    cout << "func_2 start" << endl;
    
    pthread_mutex_lock(&mtx);
    cout << "func_2 lock mtx" << endl;
 
    cout << "func_2 wait cond" << endl;
    pthread_cond_wait(&cond, &mtx);
 
    cout << "func_2 unlock mtx" << endl;
    pthread_mutex_unlock(&mtx);
 
    cout << "func_2 end" << endl;
    sleep(5);
    
    return NULL;
}
 
 
int main()
{
    pthread_t tid1, tid2;
 
    cout << "main create thread" << endl;
    pthread_create(&tid1, NULL, func_1, NULL);
    pthread_create(&tid2, NULL, func_2, NULL);
 
    sleep(3);
    cout << "main boradcast signal" << endl;
    pthread_cond_broadcast(&cond);
 
    cout << "main join thread" << endl;
 
    //pthread_join(tid1, NULL);
    //pthread_join(tid2, NULL);
 
    cout << "main end" << endl;
    return 0;
}