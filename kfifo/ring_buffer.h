#ifndef	_APUE_H
#define	_APUE_H

#include <stdint.h>
#include <pthread.h>

struct ring_buffer
{
    void *buffer;               //缓冲区
    uint32_t size;              //大小
    uint32_t in;                //入口位置
    uint32_t out;               //出口位置
    pthread_mutex_t *f_lock;    //互斥锁
};

uint32_t ring_buffer_put(struct ring_buffer *ring_buf, void *buffer,
                         uint32_t size);
uint32_t ring_buffer_get(struct ring_buffer *ring_buf, void *buffer,
                         uint32_t size);
uint32_t ring_buffer_len(const struct ring_buffer *ring_buf);
struct ring_buffer *ring_buffer_init(void *buffer, uint32_t size,
                                     pthread_mutex_t * f_lock);
void ring_buffer_free(struct ring_buffer *ring_buf);
#endif
