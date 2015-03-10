#include <unistd.h>

ssize_t read_(int fd, void* buffer, size_t count) 
{
    ssize_t res;
    size_t rest = count;
    while ((res = read(fd, buffer, rest)) > 0) {
        buffer += res;
        rest -= res;
    }
    if (res < 0)
        return res;
    return count - rest;
}

ssize_t write_(int fd, const void* buffer, size_t count) 
{
    ssize_t res;
    size_t rest = count;
    while ((res = write(fd, buffer, rest)) > 0) {
        buffer += res;
        rest -= res;
    }
    if (res < 0)
        return res;
    return count - rest;
}
