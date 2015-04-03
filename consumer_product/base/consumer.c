#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

int running = 1;
pthread_mutex_t mutex;
int buffer_has_item = 0;

void *producter_f(void *arg);
void *consumer_f(void *arg);

int main(void)
{
    pthread_t consumer_t;
    pthread_t producter_t;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&producter_t, NULL, (void *) producter_f, NULL);
    pthread_create(&consumer_t, NULL, (void *) consumer_f, NULL);

    pthread_join(consumer_t, NULL);
    pthread_join(producter_t, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *producter_f(void *arg)
{
    while (running) {
        pthread_mutex_lock(&mutex);

        if (buffer_has_item > 20) {
            pthread_mutex_unlock(&mutex);
            usleep(1);
        }

        buffer_has_item++;
        printf("Product:\t%d\n", buffer_has_item);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

void *consumer_f(void *arg)
{
    while (running) {
        pthread_mutex_lock(&mutex);

        buffer_has_item--;
        if (buffer_has_item < 0) {
            pthread_mutex_unlock(&mutex);
            usleep(1);
        }
        printf("Consumer:\t%d\n", buffer_has_item);
        pthread_mutex_unlock(&mutex);
    }
}
