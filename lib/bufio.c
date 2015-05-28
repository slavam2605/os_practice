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
    if (!buf || required > buf->capacity)
        abort();
    #endif
    ssize_t res;
    while (buf->size < required) {
        res = read(fd, buf->buffer + buf->size, buf->capacity - buf->size);
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

#define NO_DELIM_FOUND -1
#define READ_UNTIL_EOF -2
#define READ_ERROR     -3

ssize_t delim_lookup(char* buffer, int len, char delim) {
    for (size_t i = 0; i < len; i++) {
        if (buffer[i] == 0)
            break; 
        if (buffer[i] == delim)
            return i;
    }
    return NO_DELIM_FOUND;
}

ssize_t buf_readuntil(fd_t fd, struct buf_t * buf, char delim) {
    ssize_t pos;
    int res;
    while ((pos = delim_lookup(buf->buffer, buf->size, delim)) < 0) {
        res = read(fd, buf->buffer + buf->size, buf->capacity - buf->size);
        if (res == 0)
            break;
        if (res > 0) 
            buf->size += res;
        else 
            return READ_ERROR; 

    }
    if (res == 0)
        return READ_UNTIL_EOF;
    return pos;
}
