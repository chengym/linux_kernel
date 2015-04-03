/*
 *  Filename:  main.c
 *
 *  Description:消费者和生产者的问题 
 *
 *  Author:  chengym <yomin.cheng@gmail.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 1
#define M 20

int in_pos = 0;
int out_pos = 0;
int buff[M] = { 0 };

sem_t empty_sem;
sem_t full_sem;
pthread_mutex_t mutex;

int product_id = 0;
int consumer_id = 0;

void print()
{
    int i;
    for (i = 0; i < M; i++) {
        printf("%d ", buff[i]);
    }
    printf("\n");
}

void *product(unsigned int *product_idx)
{
    int id = ++product_id;
    while (1) {

        /* wakeup if NOT-full */
        sem_wait(&empty_sem);
        /* access buffer with mutex */
        pthread_mutex_lock(&mutex);

        /* product pos */
        in_pos = in_pos % M;

        printf("product_%d\t%d\tin\t%d\tbitmap:\t", id, *product_idx, in_pos);
        buff[in_pos] = 1;
        print();
        ++in_pos;

        /* unlock mutex */
        pthread_mutex_unlock(&mutex);
        /* wakeup consumer */
        sem_post(&full_sem);
    }
}

void *consumer(unsigned int *consumer_idx)
{
    int id = ++consumer_id;
    while (1) {
        sleep(1);

        /* wakeup if have content in buffer */
        sem_wait(&full_sem);
        /* access buffer with mutex */
        pthread_mutex_lock(&mutex);

        out_pos = out_pos % M;
        printf("consumer%d\t%d\tout\t%d\tbitmap:\t", id, *consumer_idx, out_pos);
        buff[out_pos] = 0;
        print();
        ++out_pos;

        /* unlock mutex */
        pthread_mutex_unlock(&mutex);
        /* wakeup product */
        sem_post(&empty_sem);
    }
}

int main()
{
    pthread_t id1[N];
    pthread_t id2[N];
    int i;
    int ret[N];

    int ini1 = sem_init(&empty_sem, 0, M);
    int ini2 = sem_init(&full_sem, 0, 0);
    if (-1 == ini1 || -1 == ini2) {
        perror("sem init failed \n");
    }
    int ini3 = pthread_mutex_init(&mutex, NULL);
    if (ini3 != 0) {
        perror("mutex init failed \n");
    }

    for (i = 0; i < N; i++) {
        ret[i] = pthread_create(&id1[i], NULL, product, (void *) (&i));
        if (ret[i] != 0) {
            perror("product creation failed");
        }
    }

    for (i = 0; i < N; i++) {
        ret[i] = pthread_create(&id2[i], NULL, consumer, (void *) (&i));
        if (ret[i] != 0) {
            perror("consumer creation failed");
        }
    }

    for (i = 0; i < N; i++) {
        pthread_join(id1[i], NULL);
        pthread_join(id2[i], NULL);
    }

    exit(0);
}
