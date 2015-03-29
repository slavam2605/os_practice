#include "bufio.h"
#include <stdio.h>

struct buf_t * buf_new(size_t capacity)
{
    struct buf_t * result = (struct buf_t*) malloc(sizeof(struct buf_t));
    if (!result)
        return NULL;
    result->capacity = capacity;
    result->size = 0;
    result->buffer = (char*) malloc(capacity);
    if (!result->buffer) {
        free(result);
        return NULL;
    }
    return result;
}

void buf_free(struct buf_t * buf)
{
    #ifdef DEBUG
    if (!buf)
        abort();
    #endif
    free(buf->buffer);
    free(buf);
}

size_t buf_capacity(struct buf_t * buf) 
{
    #ifdef DEBUG
    if (!buf)
        abort();
    #endif
    return buf->capacity;
}

size_t buf_size(struct buf_t * buf) 
{
    #ifdef DEBUG
    if (!buf)
        abort();
    #endif
    return buf->size;
}

ssize_t buf_fill(fd_t fd, struct buf_t * buf, size_t required)
{
    #ifdef DEBUG
    if (!buf)
        abort();
    #endif
    ssize_t res;
    while (buf->size < required) {
        res = read(fd, buf->buffer + buf->size, buf->capacity - buf->size);
        //#ifdef DEBUG
        //if (buf->size + res > buf->capacity)
        //    abort();
        //#endif
        printf("# LOL %d\n", res);
        if (res == 0)
            break;
        if (res > 0) 
            buf->size += res;
        else 
            return -1; 
    }
    return buf->size;
}

ssize_t buf_flush(fd_t fd, struct buf_t * buf, size_t required)
{
    #ifdef DEBUG
    if (!buf)
        abort();
    #endif
    size_t flushed = 0;
    ssize_t res;
    while (flushed < required && flushed < buf->size) {
        res = write(fd, buf->buffer, buf->size - flushed);
        if (res < 0)
            break;
        flushed += res;
    }
    int i;
    buf->size -= flushed;
    for (i = 0; i < buf->size; i++)
        buf->buffer[i] = buf->buffer[i + flushed];
    if (res < 0)
        return -1;
}
