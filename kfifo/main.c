/**@brief ring buffer测试程序，创建两个线程，一个生产者，一个消费者。
 * 生产者每隔1秒向buffer中投入数据，消费者每隔2秒去取数据。
 *@atuher Anker  date:2013-12-18
 * */
#include "ring_buffer.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE  1024 * 1024

typedef struct student_info
{
    uint64_t stu_id;
    uint32_t age;
    uint32_t score;
} student_info;


void print_student_info(const student_info * stu_info)
{
    assert(stu_info);
    printf("id:%lu\t", stu_info->stu_id);
    printf("age:%u\t", stu_info->age);
    printf("score:%u\n", stu_info->score);
}

student_info *get_student_info(time_t timer)
{
    student_info *stu_info = (student_info*)malloc(sizeof(student_info));
    if (!stu_info) {
        fprintf(stderr, "Failed to malloc memory.\n");
        return NULL;
    }

    stu_info->stu_id    = 10000;
    stu_info->age       = 5;
    stu_info->score     = 3;
    print_student_info(stu_info);

    return stu_info;
}

void *consumer_proc(void *arg)
{
    struct ring_buffer *ring_buf = (struct ring_buffer *) arg;
    student_info stu_info;
    while (1) {
        sleep(2);

        printf("------------------------------------------\n");
        printf("get a student info from ring buffer.\n");
        ring_buffer_get(ring_buf, (void *) &stu_info, sizeof(student_info));
        printf("ring buffer length: %u\n", ring_buffer_len(ring_buf));
        print_student_info(&stu_info);
        printf("------------------------------------------\n");
    }
    return (void *) ring_buf;
}

void *producer_proc(void *arg)
{
    time_t cur_time;
    struct ring_buffer *ring_buf = (struct ring_buffer *) arg;
    while (1) {
        time(&cur_time);
        srand(cur_time);
        int seed = rand() % 11111;

        printf("******************************************\n");
        student_info *stu_info = get_student_info(cur_time + seed);
        printf("put a student info to ring buffer.\n");
        ring_buffer_put(ring_buf, (void *) stu_info, sizeof(student_info));
        printf("ring buffer length: %u\n", ring_buffer_len(ring_buf));
        printf("******************************************\n");

        sleep(1);
    }
    return (void *) ring_buf;
}

int consumer_thread(void *arg)
{
    int err;
    pthread_t tid;
    err = pthread_create(&tid, NULL, consumer_proc, arg);
    if (err != 0) {
        fprintf(stderr,
                "Failed to create consumer thread.errno:%u, reason:%s\n", errno,
                strerror(errno));
        return -1;
    }
    return tid;
}

int producer_thread(void *arg)
{
    int err;
    pthread_t tid;
    err = pthread_create(&tid, NULL, producer_proc, arg);
    if (err != 0) {
        fprintf(stderr,
                "Failed to create consumer thread.errno:%u, reason:%s\n", errno,
                strerror(errno));
        return -1;
    }
    return tid;
}


int main()
{
    void        *buffer = NULL;
    uint32_t    size = 0;
    pthread_t   consume_pid, produce_pid;
    struct ring_buffer *ring_buf = NULL;

    pthread_mutex_t *f_lock =
        (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(f_lock, NULL) != 0) {
        fprintf(stderr, "Failed init mutex,errno:%u,reason:%s\n",
                errno, strerror(errno));
        return -1;
    }

    buffer = (void*)malloc(BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Failed to malloc memory.\n");
        return -1;
    }
    size = BUFFER_SIZE;
    ring_buf = ring_buffer_init(buffer, size, f_lock);
    if (!ring_buf) {
        fprintf(stderr, "Failed to init ring buffer.\n");
        return -1;
    }

    printf("multi thread test.......\n");
    produce_pid = producer_thread((void*)ring_buf);
    consume_pid = consumer_thread((void*)ring_buf);

    pthread_join(produce_pid, NULL);
    pthread_join(consume_pid, NULL);

    ring_buffer_free(ring_buf);
    pthread_mutex_destroy(f_lock);

    free(f_lock);
    return 0;
}
