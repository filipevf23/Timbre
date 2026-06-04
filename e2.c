#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

int count = 0;
pthread_mutex_t lock;

void *inc(void * arg) {
    
    pthread_mutex_lock(&lock);
    for (int i=0; i<1000000; i++) {
        count++;
        if (count < 0) {
            printf(".");
        }
    }
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

void *dec(void * arg) {
    
    pthread_mutex_lock(&lock);
    for (int i=0; i<1000000; i++) {
        count--;
        if (count < 0) {
            printf(".");
        }
    }
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

int main() {
    printf("Start: %d\n", count);

    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    pthread_mutex_init(&lock, NULL);
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, inc, NULL);
    pthread_create(&tid2, NULL, dec, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&lock);


    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
    printf("\n%lf\n", elapsedTime);

    printf("End: %d\n", count);
}